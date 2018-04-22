#include "stdafx.h"
#include "CheckIntegrityThread.h"
#include "AutoUpdateThread.h"
#include "Define.h"

#include <wininet.h>					//链接网络
#pragma comment( lib, "wininet.lib" ) 

#include "..\lib\md5\md5.h"

 #include "..\lib\xml\tinyxml2.h"
using namespace tinyxml2;

#include "..\utility\Downloader.h"
#include "..\utility\SplitFile.h"
#include "..\DlgCheckIntegrity.h"

#include <fstream> 
using namespace std;

using namespace SOUI;


//开始线程
bool CCheckIntegrityThread::Start(bool bShowPassTip)
{
	if(m_bIsChecking)
	{
		_MessageBox(NULL,L"已经正在检测当中.....",L"检测提示", MB_OK|MB_ICONINFORMATION);

		if(m_handleThreadUI!=0)
			CloseHandle(m_handleThreadUI);

		m_handleThreadUI = ::CreateThread(NULL, 0, ProcUI, this, 0 ,NULL);
		
		::WaitForSingleObject(m_EventWndInitDone, INFINITE); //等待窗口准备完毕
		RestoreProcess();
		return true;
	}

	m_bShowPassTip = bShowPassTip;
	
	//创建事件，事件有信号时，表示停止等待
	// ManualReset = false， 表示 WaitSingleObject 收到有信号的  m_EventWndInitDone 后，m_EventWndInitDone自动变为无信号
	m_EventWndInitDone = ::CreateEvent(NULL, FALSE, FALSE,NULL);

	m_EventUpdateDownloadDone = ::CreateEvent(NULL, FALSE, FALSE,NULL);

	//启动检测线程
	m_handleThreadCheking = ::CreateThread(NULL, 0, ProcChecking, this, 0 ,NULL);

	::WaitForSingleObject(m_EventUpdateDownloadDone, INFINITE); //等待update 文件下载完毕,才弹出窗口
	
	m_handleThreadUI = ::CreateThread(NULL, 0, ProcUI, this, 0 ,NULL);

	return (m_handleThreadCheking != NULL && m_handleThreadUI!=NULL);
}


//结束线程
void CCheckIntegrityThread::Stop()
{
}


//获取文件的 md5 码
bool CCheckIntegrityThread::GetFileMd5(wstring filePath, string& strMd5)
{
	//不存在文件则返回
	if(!FileHelper::CheckFileExist(filePath))
		return false;

	MD5_CTX md5 = MD5_CTX();
    char md5Str[33];
    md5.GetFileMd5(md5Str, filePath.c_str());

	strMd5 = md5Str;

	return true;
}

//检测（与下载） 线程执行地址
DWORD WINAPI CCheckIntegrityThread::ProcChecking(LPVOID pParam)
{
	CCheckIntegrityThread* pThread = static_cast<CCheckIntegrityThread*>(pParam);

	pThread->m_bIsChecking = true;


	//先下载update 并标记是否需要更新
	pThread->DownloadUpdateFileAndMark();

	//由于程序每次启动都要下载update，为了使得下载update的过程不弹出窗口
	//等下载对比完键结果填入temp后再启动窗口
	::SetEvent(pThread->m_EventUpdateDownloadDone);

	::WaitForSingleObject(pThread->m_EventWndInitDone, INFINITE); //等待窗口准备完毕，才能进行接下来的更新过程

	pThread->CheckUpdateFile();	//检查所有的dll是否为最新
	pThread->CheckFFmpeg();		//检测 FFmpeg.exe 是否已存在

	pThread->m_bIsChecking = false;
	return 0;
}


//UI 线程执行地址
DWORD WINAPI CCheckIntegrityThread::ProcUI(LPVOID pParam)
{
	CCheckIntegrityThread* pThread = static_cast<CCheckIntegrityThread*>(pParam);
	
	//打开一个窗口，用于显示进度条 和 提供取消操作
	DlgCheckIntegrity dlg(L"xml_check_integrity");

	dlg.SetEventHandleForWndHandle(pThread->m_EventWndInitDone, &pThread->m_hCheckWnd, pThread);

	int ret = dlg.DoModal(NULL);

	return 0;
}


//检查所有文件是否为最新
bool CCheckIntegrityThread::CheckUpdateFile()
{
	wstring strTemp = m_wstrEtcFloder + FILE_NAME_NEED_UPDATE ;
	wstring buffer;
	if(!FileOperator::ReadAllText(strTemp, buffer))
		return false;
	
	if(buffer[0] == L'1')
	{		
		//为了防止下载中途异常退出，而导致下次检测更新认为上一次已完成更新，通过 lastUpdateDone 来辨认是否真的更新完毕
		//写入 lastUpdateDone 文件， 0 表示未完成最后一次的更新
		FileOperator::WriteAllText(m_wstrEtcFloder + L"lastUpdateDone",L"0");  

		//【版本 v2.1.11 做特殊的更新处理】
		wstring strLastExe = L"";
		if(AutoUpdateThread::VersionCompare( VERSION_NUMBER.c_str(), L"2.1.11") == 0)
		{
			UpdateProgressUI(2, wstring( L"正在为 版本 v2.1.11 做特殊的更新处理...").c_str());
			
			//版本 2.1.11 将是 基于 soui 2.2 版本的分支的最后一个版本，详见该分支 【Sun Apr 22 14:20:12 2018 +0800】的提交信息
			
			//这里在实施下载其他组件之前，需要先更新 当前的 exe 文件为 soui 2.6 编译出来的 exe
			strLastExe =  FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" + FILE_NAME_LAST_EXE_TEMP;
	
			//下载最新的soui 2.6 编译出来的 exe,  BesLyric.exe（服务器的名称为 BesLyricExe.rar） 到 strLastExe （BesLyric）中
			bool bRet = CDownloader::DownloadFile( LINK_LAST_EXE_2 , strLastExe);
			if(bRet == false)
			{
				UpdateProgressUI(100, wstring( L"下载文件失败，网络连接异常...").c_str());
				return false;
			}
		}


		UpdateProgressUI(5, wstring( L"读取更新项目内容，请耐心等待 ...").c_str());
		//读取更新文件得到所有待校验的文件信息
		vector<UpdateItem> updateItems;
		GetUpdateItem(updateItems);

		//逐个对比，不存在则下载创建，存在则重命名，下载替换

		//收集需要更新的项
		vector<UpdateItem> updateList;
		for(auto iter = updateItems.begin(); iter != updateItems.end(); iter++)
		{
			wstring taget = FileHelper::GetCurrentDirectoryStr() + iter->local + iter->fileName;
			
			//计算本地md5 
			string md5;
			GetFileMd5(taget, md5);

			if(md5 != iter->md5)
			{
				//需要记录该文件
				updateList.push_back(*iter);
			}
		}
		
		UpdateProgressUI(30, wstring( L"正在下载需要更新的项 ...").c_str());
		//下载项到tempDir中

		wstring strTempDir = m_wstrEtcFloder + L"tempDir\\" ;

		int nCount = 0;
		for(auto iter = updateList.begin(); iter != updateList.end(); iter++)
		{
			nCount += (90-30) / int(updateList.size());
			UpdateProgressUI(35+nCount, (wstring( L"正在下载") + iter->fileName).c_str());

			//先确保 临时目录 中iter->local 目录存在
			if(!MakeSureRelativeLocalExist(strTempDir, iter->local))
			{
				UpdateProgressUI(100, wstring( L"创建临时目录失败，下载组件失败") .c_str());
				return false;
			}

			//下载到相对位置 iter->local 中
			wstring strTarget = strTempDir + iter->local + iter->fileName;

			if(!CDownloader::DownloadFile(iter->link, strTarget))
			{
				_MessageBox(NULL, L"无法下载待更新文件，更新失败", L"提示", MB_OK|MB_ICONWARNING);
				UpdateProgressUI(100, wstring( L"下载组件失败") .c_str());
				return false;
			}
		}

		UpdateProgressUI(95, wstring( L"正在替换组件...") .c_str());
		vector<wstring> oldFileToDelete;

		//重命名所有的旧文件
		for(auto iter = updateList.begin(); iter != updateList.end(); iter++)
		{
			wstring target = FileHelper::GetCurrentDirectoryStr() + iter->local + iter->fileName;
			wstring targetTemp = target + L".temp";
			_wrename(target.c_str(), targetTemp.c_str());
			oldFileToDelete.push_back(iter->local + iter->fileName +  L".temp");
		}

		//将下载的到tempDir的文件复制到目标位置
		for(auto iter = updateList.begin(); iter != updateList.end(); iter++)
		{
			wstring target = FileHelper::GetCurrentDirectoryStr() + iter->local + iter->fileName;
			wstring targetTemp =  m_wstrEtcFloder + L"tempDir\\" + iter->fileName;
			CopyFileW(targetTemp.c_str(), target.c_str(), FALSE);
		}

		//保存用于下次启动后删除
		wstring strContent;
		for(auto iterOld = oldFileToDelete.begin(); iterOld != oldFileToDelete.end(); iterOld++)
		{
			strContent += *iterOld + L"\n";
		}
		FileOperator::WriteAllText(m_wstrEtcFloder + L"fileToDelete",strContent);  


		//【版本 v2.1.11 做特殊的更新处理】//上面对应的部分为下载，为了更大概率地实现全部替换，先所有内容下载完后，再在此替换exe文件
		if(AutoUpdateThread::VersionCompare( VERSION_NUMBER.c_str(), L"2.1.11") == 0)
		{
			/*修改文件名称，达到替换旧版本目的 */
			wstring strCurrentExe = FileHelper::GetCurrentDirectoryStr() + FILE_NAME_LAST_EXE_TEMP + L".exe";
			wstring strBackupExe =  FileHelper::GetCurrentDirectoryStr() + FILE_NAME_LAST_EXE_TEMP + L"."+ VERSION_NUMBER ;//+ ".exe";

			if(FileHelper::CheckFileExist(strBackupExe))//删除可能存在的备份文件
				_wremove(strBackupExe.c_str());
			_wrename(strCurrentExe.c_str(),strBackupExe.c_str());

			//复制新的exe到原来目录
			CopyFileW(strLastExe.c_str(), strCurrentExe.c_str(), FALSE);
		}

		//写入 lastUpdateDone 文件， 1 表示已完成最后一次的更新
		FileOperator::WriteAllText(m_wstrEtcFloder + L"lastUpdateDone",L"1"); 
	}
	
	UpdateProgressUI(100, wstring( L"完成组件下载") .c_str());

	return true;
}

//确保相对路径目录存在
bool CCheckIntegrityThread::MakeSureRelativeLocalExist(wstring basePath, wstring relativePath)
{

	return true;
}

//获得更新文件内容
bool CCheckIntegrityThread::GetUpdateItem(vector<UpdateItem>& updateItems)
{
	updateItems.clear();

	//update 文件路径
	wstring strUpdate = m_wstrEtcFloder + L"update" ;

	string strUpdatePath = S_CW2A(SStringW(strUpdate.c_str()));
	if(FileHelper::CheckFileExist(strUpdate))
	{
		//读取XML文件
		tinyxml2::XMLDocument doc;
		doc.LoadFile(strUpdatePath.c_str());
		if(doc.Error())
		{
			return false;
		}

		//根
		XMLElement *pRoot = doc.RootElement();
		SASSERT(pRoot);

		XMLElement* ele = pRoot->FirstChildElement();
		while(ele)
		{
			UpdateItem item;
			const char* szName = ele->Attribute("name");
			const char* szLink = ele->Attribute("link");
			const char* szLocal = ele->Attribute("local");
			const char* szMd5 = ele->Attribute("md5");
			
			wstring wStrName = S_CA2W(SStringA(szName),CP_UTF8);
			wstring wStrLink = S_CA2W(SStringA(szLink),CP_UTF8);
			wstring wStrLocal = S_CA2W(SStringA(szLocal),CP_UTF8);
			string strMd5 = szMd5;

			item.fileName = wStrName;
			item.link = wStrLink;
			item.local = wStrLocal; 
			item.md5 = strMd5;

			updateItems.push_back(item);

			//下一兄弟结点
			ele = ele->NextSiblingElement();
		}
	}



	return true;
}

//下载更新文件然后标记是否需要更新
bool CCheckIntegrityThread::DownloadUpdateFileAndMark()
{
	//写入 needUpdate 文件， 1 表示需要，0 表示不需要
	wstring strTemp = m_wstrEtcFloder + FILE_NAME_NEED_UPDATE ;
	FileOperator::WriteAllText(strTemp,L"0");  //先写入一个不需要更新updateItem标记

	//保证temp文件夹存在
	wstring strTempDir = m_wstrEtcFloder + L"tempDir";
		
	// 先保证temp文件目录存在
	if(!FileHelper::CheckFolderExist(strTempDir))
	{
		if(RET_SUCCEEDED != _wmkdir(strTempDir.c_str()))
		{
			wstring strTip = L"程序无法创建目录：\\n";
			strTip += strTempDir;
			_MessageBox(NULL, strTip.c_str(), L"提示", MB_OK|MB_ICONWARNING);
			return false;
		}
	}

	//临时下载的update文件路径
	wstring strTempUpdate = m_wstrEtcFloder + L"tempDir\\update.xml" ;
	if(!CDownloader::DownloadFile(LINK_UPDATE_ITEM_FILE, strTempUpdate))
	{
		_MessageBox(NULL, L"无法下载update.xml文件，检测更新失败", L"提示", MB_OK|MB_ICONWARNING);
		return false;
	}

	//计算2个update文件的md5,不同则表明需要后续操作需要检测所有update item的文件是否需要更新

	wstring strUpdate = m_wstrEtcFloder + L"update" ;
	if(!FileHelper::CheckFileExist(strUpdate)) 
	{
		//文件不存在
		FileOperator::WriteAllText(strTemp,L"1");  //写入一个需要更新updateItem标记
	}
	else
	{
		//比较2个文件md5
		string md5Now;
		string md5Temp;
		bool bRet = GetFileMd5(strUpdate, md5Now);
		bRet = GetFileMd5(strTempUpdate, md5Temp);
		bool bNeedToUpdate = false;

		if(md5Now == md5Temp)
		{
			//和上次文件相同之外，还必须读取 lastUpdateDone 里面的状态，进行确认是否真的完成更新
			//因为有可能 最后一次更新了 1半 断网导致更新中断
			wstring strTemp = m_wstrEtcFloder + L"lastUpdateDone" ;
			wstring buffer;
			if(!FileOperator::ReadAllText(strTemp, buffer))
				bNeedToUpdate = true;   //找不到lastUpdateDone 文件，认为需要更新
			else
			{
				if(buffer[0] == L'0')
					bNeedToUpdate = true;
				else
					bNeedToUpdate = false;	 //只有 和上次文件相同，且 lastUpdateDone 中的标准 不为0 时，才认为不用更新
			}
		}
		else
		{
			bNeedToUpdate = true;
		}

		FileOperator::WriteAllText(strTemp, bNeedToUpdate ? L"1" : L"0");  //写入一个需要更新updateItem标记
	}

	//将新下载的文件复制到目标位置
	CopyFileW(strTempUpdate.c_str(), strUpdate.c_str(), FALSE);
	return true;
}


//检查ffmpeg.exe 是否存在，不存在则下载
bool CCheckIntegrityThread::CheckFFmpeg()
{
	//检查ffpmeg是否存在
	wstring strDir = FileHelper::GetCurrentDirectoryStr() + TEMP_WAV_FLODER_NAME ;
	wstring strFfmpeg = strDir + L"\\ffmpeg.exe";
	
	UpdateProgressUI(0, wstring(L"检测 "+strFfmpeg + L"是否存在...").c_str());

	if(!FileHelper::CheckFileExist(strFfmpeg)) 
	{
		UpdateProgressUI(5, wstring(L"检测 "+strDir + L"是否存在...").c_str());

		//不存在ffpmeg.exe， 先保证文件目录存在
		if(!FileHelper::CheckFolderExist(strDir))
		{
			if(RET_SUCCEEDED != _wmkdir(strDir.c_str()))
			{
				UpdateProgressUI(100, L"结束！(无法创建目录)");

				wstring strTip = L"程序无法创建目录：\\n";
				strTip += strDir +L"\\n";
				strTip += L"这将导致部分mp3文件【可能】无法正常播放\\n";
				
				_MessageBox(NULL, strTip.c_str(), L"提示", MB_OK|MB_ICONWARNING);
				return false;
			}
		}
		
		UpdateProgressUI(60, wstring( L"下载转换器 ffmpeg(34.84 MB)，请耐心等待 ...").c_str());
		//先从 LINK_DOWNLOAD_SERVER 下载 ffmpeg
		bool bFirstTrySucceed = CDownloader::DownloadFile(LINK_DOWNLOAD_SERVER + L"ffmpeg.exe", strFfmpeg);
		if(!bFirstTrySucceed)//下载不成功
		{
			//备用下载方案
			//开始从服务器下载ffmpeg
			wstring strNameExt = L"ffmpeg.ext"+ SERVER_FILE_EXTENTION_W;
			UpdateProgressUI(10,wstring( L"正在下载 "+ strNameExt +L" ...").c_str());

			wstring strFileExt = strDir +L"\\"+ strNameExt;
			wstring strLinkExt = LINK_SERVER_PATH + strNameExt;

			//下载 ffmpeg.ext.zip
			bool bRet = CDownloader::DownloadFile(strLinkExt, strFileExt);
		

			//下载 ffmpeg.1.zip - ffmpeg.4.zip 4个文件
			WCHAR szBuffer[MAX_BUFFER_SIZE/2];
			if(false != bRet)
				for(auto i=1; i<=4 ;i++)
				{
					wstring strNameSplited = wstring(L"ffmpeg.") + _itow(i,szBuffer, 10) + SERVER_FILE_EXTENTION_W;
					UpdateProgressUI(10 + i*(90-10)/4, wstring(L"正在下载 "+strNameSplited +L"...").c_str());

					wstring strFileSplited = strDir +L"\\"+ strNameSplited;
					wstring strLinkSplited = LINK_SERVER_PATH + strNameSplited;

					bRet = CDownloader::DownloadFile(strLinkSplited, strFileSplited);
					if(false == bRet)
						break;
				}

			//检查 ffmpeg.ext.zip 和 ffmpeg.1.zip - ffmpeg.4.zip 5个文件是否下载完毕
			bool bHaveDownload = true;
			if(!FileHelper::CheckFileExist(strFileExt))
				bHaveDownload = false;
		
			for(auto i=1; i<=4 ;i++)
			{
				wstring strFileSplited = strDir + wstring(L"\\ffmpeg.") + _itow(i,szBuffer, 10) + SERVER_FILE_EXTENTION_W;
				if(!FileHelper::CheckFileExist(strFileExt))
				{
					bHaveDownload = false;
					break;
				}
			}

			if(false == bRet)
				bHaveDownload = false;
		
			if(!bHaveDownload)
			{
				UpdateProgressUI(100, L"结束！(缺失文件）");

				//提示用户确实 转换文件，并提示如何处理
				wstring strTip = L"缺少文件：\\n";
				strTip+= strFfmpeg + L"\\n\\n";
			
				strTip+= wstring(L"可能原因：\\n ");
				strTip+= wstring(L"1、启动程序时，程序无法连接网络，自动下载\\n");
				strTip+= wstring(L"2、使用过程中，删除或移动了ffmpeg.exe 文件\\n");
				strTip+= wstring(L"3、程序服务端异常\n\n");

				strTip+= wstring(L"解决方案：\\n");
				strTip+= wstring(L"1、保存程序能正常访问网络,然后重新在设置页面进行“完整性检测”\\n");
				strTip+= wstring(L"2、手动下载ffmpeg.exe 文件。访问程序“本软件”页面的博客链接，查看第4小节“常见问题”下载教程\\n");
				strTip+= wstring(L"");

				_MessageBox(NULL,strTip.c_str(), L"完整性提示", MB_OK|MB_ICONWARNING);
				return false;
			}
			else
			{
				UpdateProgressUI(90, L"合并下载的文件,生成ffmpeg.exe ...");
				//已经下载完毕，合并文件得到 ffmpeg.exe
				bRet = CSplitFile::MergeFile(strDir,L"ffmpeg", strDir);
				bool bFail = false;
				if(!bRet)
				{
					UpdateProgressUI(100, L"结束！(生成文件 完整的 ffmpeg.exe 失败）");

					wstring strTip = L"无法完整生成文件：\\n";
					strTip += strFfmpeg + L"\\n\n";			
				
					strTip+= wstring(L"可能原因：\\n ");
					strTip+= wstring(L"1、服务端网络不稳定(不是你的问题)，导致文件下载不完整\\n");
					strTip+= wstring(L"2、本地网络不稳地\\n\\n");

					strTip+= wstring(L"解决方案：\\n");
					strTip+= wstring(L"1、重新在设置页面进行“完整性检测”\\n");
					strTip+= wstring(L"2、手动下载ffmpeg.exe 文件。访问程序“本软件”页面的博客链接，查看第4小节“常见问题”手动下载 ffmpeg.exe\\n");
					strTip+= wstring(L"");
					_MessageBox(NULL,strTip.c_str(), L"提示", MB_OK|MB_ICONWARNING);
					bFail = true;
				}

				//删除临时下载的文件
				if(!bFail)
					UpdateProgressUI(95, L"删除临时文件...");

				_wremove(strFileExt.c_str());
				for(auto i=1; i<=4 ;i++)
				{
					wstring strNameSplited = wstring(L"ffmpeg.") + _itow(i,szBuffer, 10) + SERVER_FILE_EXTENTION_W;
					wstring strFileSplited = strDir +L"\\"+ strNameSplited;
					_wremove(strFileSplited.c_str());
				}

				if(bFail)
					return false;
			}
		}
	}
	
	UpdateProgressUI(100, L"更新完成！");
	
	CloseProgressUI();

	if(m_bShowPassTip)
	{
		_MessageBox(NULL, L"当前程序已完整", L"完整性提示", MB_OK|MB_ICONINFORMATION);
	}

	return true;
}


//更新UI界面
void CCheckIntegrityThread::UpdateProgressUI(int nPercentage, const WCHAR* pszTip)
{
	m_nPercentage = nPercentage;
	wcscpy_s(m_szTip, MAX_BUFFER_SIZE ,pszTip);
	::SendMessageW(m_hCheckWnd, MSG_USER_UPDATE_CHECK_PROGRESS, (WPARAM)nPercentage,(LPARAM)pszTip);
}

void CCheckIntegrityThread::RestoreProcess()
{
	::SendMessageW(m_hCheckWnd, MSG_USER_UPDATE_CHECK_PROGRESS, (WPARAM)m_nPercentage,(LPARAM)m_szTip);
}


void CCheckIntegrityThread::CloseProgressUI()
{
	::SendMessageW(m_hCheckWnd, MSG_USER_CLOSE_CHECK_PROGRESS, (WPARAM)m_nPercentage,(LPARAM)m_szTip);
}