#include "stdafx.h"
#include "CheckIntegrityThread.h"
#include "AutoUpdateThread.h"
#include "Define.h"

#include <wininet.h>					//链接网络
#pragma comment( lib, "wininet.lib" ) 

#include "..\utility\Downloader.h"
#include "..\utility\SplitFile.h"
#include "..\DlgCheckIntegrity.h"
#include "LinkHelper.h"

#include <fstream> 
using namespace std;

using namespace SOUI;

//开始线程
bool CCheckIntegrityThread::Start(bool bShowPassTip)
{
	if(m_bIsChecking)
	{
		if(m_bIsProcUIClose) //之前已经关闭了
		{
			if(m_handleThreadUI!=0)
				CloseHandle(m_handleThreadUI);

			m_handleThreadUI = ::CreateThread(NULL, 0, ProcUI, this, 0 ,NULL);
		
			::WaitForSingleObject(m_EventWndInitDone, INFINITE); //等待窗口准备完毕

			RestoreProcess();
		}

		SwitchToThisWindow(m_hCheckWnd,TRUE);  // 第二个参数，bRestore ： 如果被极小化，则恢复窗口

		return true;
	}

	m_bShowPassTip = bShowPassTip;
	
	//创建事件，事件有信号时，表示停止等待
	// ManualReset = false， 表示 WaitSingleObject 收到有信号的  m_EventWndInitDone 后，m_EventWndInitDone自动变为无信号
	m_EventWndInitDone = ::CreateEvent(NULL, FALSE, FALSE,NULL);

	m_EventUpdateDownloadDone = ::CreateEvent(NULL, TRUE, FALSE,NULL);

	//启动检测线程
	m_handleThreadCheking = ::CreateThread(NULL, 0, ProcChecking, this, 0 ,NULL);

	//启动UI线程
	m_handleThreadUI = ::CreateThread(NULL, 0, ProcUI, this, 0 ,NULL);

	return (m_handleThreadCheking != NULL);
}


//结束线程
void CCheckIntegrityThread::Stop()
{
}


//检测（与下载） 线程执行地址
DWORD WINAPI CCheckIntegrityThread::ProcChecking(LPVOID pParam)
{
	CCheckIntegrityThread* pThread = static_cast<CCheckIntegrityThread*>(pParam);

	pThread->m_bIsChecking = true;

	//先下载update 并标记是否需要更新
	pThread->DownloadUpdateFileAndMark();
	
	//等下载对比完将结果填入temp后再启动窗口
	::SetEvent(pThread->m_EventUpdateDownloadDone);
	
	::WaitForSingleObject(pThread->m_EventWndInitDone, INFINITE); //等待窗口准备完毕，才能进行接下来的更新过程

	//从服务器更新链接数据
	LinkHelper::getSingleton().UpdateLinkFromServer();

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

	
	::WaitForSingleObject(pThread->m_EventUpdateDownloadDone, INFINITE); //等待update 文件下载完毕,才弹出窗口
	
	pThread->m_bIsProcUIClose = false;  //标记未关闭
	
	if(pThread->PreCheckWhetherNeedToShowCheckDialog())
		dlg.DoModal(NULL);
	else
		::SetEvent(pThread->m_EventWndInitDone);  //不需要弹框，但是 ProcChecking 中执行检测前还是要等这个 事件，所以在这里置一下事件有效

	pThread->m_bIsProcUIClose = true;  //标记已经关闭
	return 0;
}

 //预先检查是否需要显示检测dialog
bool CCheckIntegrityThread::PreCheckWhetherNeedToShowCheckDialog()
{
	bool bNeed = false;

	wstring strTemp = updateHelper.GetEtcDir() + FILE_NAME_NEED_UPDATE ;
	wstring buffer;
	if(!FileOperator::ReadAllText(strTemp, buffer))
		return false;
	
	if(buffer[0] == L'1')		//有更新标记
		bNeed = true;
	else
	{
		//没有更新标志，再看看ffmpeg
		//检查ffpmeg是否存在
		wstring strDir = FileHelper::GetCurrentDirectoryStr() + TEMP_WAV_FLODER_NAME ;
		wstring strFfmpeg = strDir + L"\\ffmpeg.exe";
	
		bool bFileExist =  FileHelper::CheckFileExist(strFfmpeg);
		string strMd5;
		bool bRet = updateHelper.GetFileMd5(strFfmpeg,strMd5);
		if(!bFileExist || !bRet || 
			(bFileExist && !updateHelper.IsValidFFmpegMd5(strMd5))) //检测发现不一致，待会需要弹框
			bNeed = true;
	}	
	return bNeed;
}

//检查所有文件是否为最新
bool CCheckIntegrityThread::CheckUpdateFile()
{
	wstring strTemp = updateHelper.GetEtcDir() + FILE_NAME_NEED_UPDATE ;
	wstring buffer;
	if(!FileOperator::ReadAllText(strTemp, buffer))
		return false;
	
	if(buffer[0] == L'1')
	{		
		//为了防止下载中途异常退出，而导致下次检测更新认为上一次已完成更新，通过 lastUpdateDone 来辨认是否真的更新完毕
		//写入 lastUpdateDone 文件， 0 表示未完成最后一次的更新
		FileOperator::WriteAllText(updateHelper.GetEtcDir() + L"lastUpdateDone",L"0");  

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
		if(!updateHelper.GetUpdateItem(updateHelper.GetEtcDir() + L"update", updateItems))			
		{
			UpdateProgressUI(100, wstring( L"获取更新内容失败...").c_str());
			return false;
		}

		//逐个对比，不存在则下载创建，存在则重命名，下载替换

		//收集需要更新的项
		vector<UpdateItem> updateList;
		for(auto iter = updateItems.begin(); iter != updateItems.end(); iter++)
		{
			wstring taget = FileHelper::GetCurrentDirectoryStr() + iter->local + iter->fileName;
			
			//计算本地md5 
			string md5;
			updateHelper.GetFileMd5(taget, md5);

			if(md5 != iter->md5)
			{
				//需要记录该文件
				updateList.push_back(*iter);
			}
		}
		
		UpdateProgressUI(30, wstring( L"正在下载需要更新的项 ...").c_str());
		//下载项到tempDir中

		wstring strTempDir = updateHelper.GetEtcDir() + L"tempDir\\" ;

		int nCount = 0;
		for(auto iter = updateList.begin(); iter != updateList.end(); iter++)
		{
			nCount += (90-30) / int(updateList.size());
			UpdateProgressUI(35+nCount, (wstring( L"正在下载") + iter->fileName).c_str());

			//先确保 临时目录 中iter->local 目录存在
			if(!updateHelper.MakeSureRelativeLocalExist(strTempDir, iter->local))
			{
				UpdateProgressUI(100, wstring( L"创建临时目录失败，下载组件失败") .c_str());
				return false;
			}

			//下载到相对位置 iter->local 中
			wstring strTarget = strTempDir + iter->local + iter->fileName;

			bool fail = false;
			if(!CDownloader::DownloadFile(iter->link, strTarget))
				fail = true;
			else
			{
				//计算下载下来的文件的 md5
				string md5;
				updateHelper.GetFileMd5(strTarget, md5);
				if(md5 != iter->md5)
					fail = true;
			}

			if(fail)
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
			if(!updateHelper.MakeSureRelativeLocalExist(FileHelper::GetCurrentDirectoryStr(), iter->local))
			{
				UpdateProgressUI(100, wstring( L"创建目录失败，下载组件失败") .c_str());
				return false;
			}
			wstring target = FileHelper::GetCurrentDirectoryStr() + iter->local + iter->fileName;
			wstring targetTemp =  updateHelper.GetEtcDir() + L"tempDir\\" + iter->fileName;
			CopyFileW(targetTemp.c_str(), target.c_str(), FALSE);
		}

		//保存用于下次启动后删除
		wstring strContent;
		for(auto iterOld = oldFileToDelete.begin(); iterOld != oldFileToDelete.end(); iterOld++)
		{
			strContent += *iterOld + L"\n";
		}
		FileOperator::WriteAllText(updateHelper.GetEtcDir() + L"fileToDelete",strContent);  


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
		FileOperator::WriteAllText(updateHelper.GetEtcDir() + L"lastUpdateDone",L"1"); 
	}
	
	UpdateProgressUI(100, wstring( L"完成组件下载") .c_str());

	return true;
}

//下载更新文件然后标记是否需要更新
bool CCheckIntegrityThread::DownloadUpdateFileAndMark()
{
	//更新项文件自检
	updateHelper.UpdateItemSelfCheck();

	//写入 needUpdate 文件， 1 表示需要，0 表示不需要
	wstring strTemp = updateHelper.GetEtcDir() + FILE_NAME_NEED_UPDATE ;
	FileOperator::WriteAllText(strTemp,L"0");  //先写入一个不需要更新updateItem标记

	//保证temp文件夹存在
	wstring strTempDir = updateHelper.GetEtcDir() + L"tempDir";
		
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
	wstring strTempUpdate = updateHelper.GetEtcDir() + L"tempDir\\update.xml" ;
	if(!CDownloader::DownloadFile(LINK_UPDATE_ITEM_FILE, strTempUpdate))
	{
		//不提示失败，不然每次不联网启动都提示这一句
		//_MessageBox(NULL, L"无法下载update.xml文件，检测更新失败", L"提示", MB_OK|MB_ICONWARNING);
		return false;
	}

	if(!updateHelper.IsValidXml(strTempUpdate))
	{
		return false;
	}

	//计算2个update文件的md5,不同则表明需要后续操作需要检测所有update item的文件是否需要更新

	wstring strUpdate = updateHelper.GetEtcDir() + L"update" ;
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
		bool bRet = updateHelper.GetFileMd5(strUpdate, md5Now);
		bRet = updateHelper.GetFileMd5(strTempUpdate, md5Temp);
		bool bNeedToUpdate = false;

		if(md5Now == md5Temp)
		{
			//和上次文件相同之外，还必须读取 lastUpdateDone 里面的状态，进行确认是否真的完成更新
			//因为有可能 最后一次更新了 1半 断网导致更新中断
			wstring strTemp = updateHelper.GetEtcDir() + L"lastUpdateDone" ;
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

	bool bFileExist =  FileHelper::CheckFileExist(strFfmpeg);
	string strMd5;
	bool bRet = updateHelper.GetFileMd5(strFfmpeg,strMd5);
	if(!bFileExist || !bRet 
		|| (bFileExist && !updateHelper.IsValidFFmpegMd5(strMd5))) //检测
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
		
		vector<LinkValue> links = LinkHelper::getSingleton().GetAllLinksFFmpeg();

		bool bTrySuceed = false;
		for(size_t i = 0; i < links.size(); ++i)
		{
			UpdateProgressUI(60 + i, wstring( L"下载转换器 ffmpeg(42.41MB),请耐心等待 ... (try "+links[i].value +L")").c_str());
			if(!CDownloader::DownloadFile( links[i].link, strFfmpeg, &m_hCheckWnd))
				continue;
			
			bool bRet = updateHelper.GetFileMd5(strFfmpeg,strMd5);
			if(!bRet || !updateHelper.IsValidFFmpegMd5(strMd5))
				continue;
			else
			{
				bTrySuceed = true;
				break;
			}
		}

		if(!bTrySuceed)//下载不成功
		{
			UpdateProgressUI(65, wstring( L"下载转换器 ffmpeg(42.41 MB)，请耐心等待 ...(try final)").c_str());
			bool bHaveDownload = true;

			//备用下载方案
			//开始从服务器下载ffmpeg
			wstring strNameExt = L"ffmpeg.ext"+ SERVER_FILE_EXTENTION_W;
			UpdateProgressUI(10,wstring( L"正在下载 "+ strNameExt +L" ...").c_str());

			wstring strFileExt = strDir +L"\\"+ strNameExt;
			wstring strLinkExt = LINK_SERVER_PATH_2 + strNameExt;
			int nCountSplit = 11;

			//下载 ffmpeg.ext.zip
			if( CDownloader::DownloadFile(strLinkExt, strFileExt, &m_hCheckWnd))
			{
				//文件下载限制：{"success":false,"message":"该文件已超过当日下载流量(200MB)的下载限制"}
				//下载 ffmpeg.1.zip - ffmpeg.11.zip 11个文件
				WCHAR szBuffer[MAX_BUFFER_SIZE/2];
				WCHAR szBuffer2[MAX_BUFFER_SIZE/2];
				for(auto i=1; i<= nCountSplit ;i++)
				{
					wstring strNameSplited = wstring(L"ffmpeg.") + _itow(i,szBuffer, 10) + SERVER_FILE_EXTENTION_W;
					UpdateProgressUI(10 + i*(90-10)/nCountSplit, wstring(L"正在下载 "+strNameSplited 
						+ L"... ("+_itow(i,szBuffer, 10)+ L"/" +_itow(nCountSplit,szBuffer2, 10) + L", 共 42.41 MB)").c_str());

					wstring strFileSplited = strDir +L"\\"+ strNameSplited;
					wstring strLinkSplited = LINK_SERVER_PATH_2 + strNameSplited;

					bRet = CDownloader::DownloadFile(strLinkSplited, strFileSplited, &m_hCheckWnd);
					if(false == bRet)
					{
						bHaveDownload = false;
						break;
					}
				}
			}
			else
				bHaveDownload = false;

			if(!bHaveDownload)
			{
				UpdateProgressUI(100, L"下载失败！可尝试在“设置”页面重新点击“完整性检测”");

				//提示用户确实 转换文件，并提示如何处理
				wstring strTip = L"无法成功下载文件：\\n";
				strTip+= strFfmpeg + L"\\n\\n";
		
				strTip+= wstring(L"可能原因：\\n");
				strTip+= wstring(L"1、程序无法连接网络，无法下载\\n");
				strTip+= wstring(L"2、获取文件的服务端异常（github服务器不稳定比较正常，建议多尝试几次下面方案1）\\n\\n");

				strTip+= wstring(L"解决方案：\\n");
				strTip+= wstring(L"1、先尝试：保证程序能正常访问网络,然后重新在“设置”页面进行“完整性检测”\\n");
				strTip+= wstring(L"2、如果上述无法解决，尝试自己下载ffmpeg.exe 文件。点击“确定”后，点击“检测程序完整性”下的“手动下载ffmpeg.exe”链接\\n");
			
				strTip+= wstring(L"\\n");
				strTip+= wstring(L"温馨提示:程序播放 mp3 时需用到 ffmpeg.exe");

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
					//到此为止，用户已经等待了多个下载点了，已经没有结果了

					UpdateProgressUI(100, L"下载失败！可尝试在“设置”页面重新点击“完整性检测”");

					wstring strTip = L"无法完整生成文件：\\n";
					strTip += strFfmpeg + L"\\n\n";			
			
					strTip+= wstring(L"可能原因：\\n");
					strTip+= wstring(L"1、程序无法连接网络，无法下载\\n");
					strTip+= wstring(L"2、获取文件的服务端异常（github服务器不稳定比较正常，建议多尝试几次下面方案1）\\n\\n");

					strTip+= wstring(L"解决方案：\\n");
					strTip+= wstring(L"1、先尝试：保证程序能正常访问网络,然后重新在“设置”页面进行“完整性检测”\\n");
					strTip+= wstring(L"2、如果上述无法解决，尝试自己下载ffmpeg.exe 文件。点击“确定”后，点击“检测程序完整性”下的“手动下载ffmpeg.exe”链接\\n");
					strTip+= wstring(L"\\n");
				    strTip+= wstring(L"温馨提示:程序播放 mp3 时需用到 ffmpeg.exe");
					_MessageBox(NULL,strTip.c_str(), L"提示", MB_OK|MB_ICONWARNING);
					bFail = true;
				}

				//删除临时下载的文件
				if(!bFail)
					UpdateProgressUI(95, L"删除临时文件...");

				_wremove(strFileExt.c_str());
				wchar_t szBuffer[255];
				for(auto i=1; i<= nCountSplit ;i++)
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