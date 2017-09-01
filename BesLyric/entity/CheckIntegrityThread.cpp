#include "stdafx.h"
#include "CheckIntegrityThread.h"
#include "AutoUpdateThread.h"
#include "Define.h"
#include <wininet.h>					//链接网络
#pragma comment( lib, "wininet.lib" ) 

#include "..\utility\SplitFile.h"
#include "..\DlgCheckIntegrity.h"


using namespace SOUI;

CCheckIntegrityThread* CCheckIntegrityThread::ms_Singleton = NULL;


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


	//启动检测线程
	m_handleThreadCheking = ::CreateThread(NULL, 0, ProcChecking, this, 0 ,NULL);
	m_handleThreadUI = ::CreateThread(NULL, 0, ProcUI, this, 0 ,NULL);

	return (m_handleThreadCheking != NULL && m_handleThreadUI!=NULL);
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

	::WaitForSingleObject(pThread->m_EventWndInitDone, INFINITE); //等待窗口准备完毕

	pThread->CheckFFmpeg();
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
		bool bFirstTrySucceed = AutoUpdateThread::DownloadFile(LINK_DOWNLOAD_SERVER + L"ffmpeg.exe", strFfmpeg);
		if(!bFirstTrySucceed)//下载不成功
		{
			//备用下载方案
			//开始从服务器下载ffmpeg
			wstring strNameExt = L"ffmpeg.ext"+ SERVER_FILE_EXTENTION_W;
			UpdateProgressUI(10,wstring( L"正在下载 "+ strNameExt +L" ...").c_str());

			wstring strFileExt = strDir +L"\\"+ strNameExt;
			wstring strLinkExt = LINK_SERVER_PATH + strNameExt;

			//下载 ffmpeg.ext.zip
			bool bRet = AutoUpdateThread::DownloadFile(strLinkExt, strFileExt);
		

			//下载 ffmpeg.1.zip - ffmpeg.4.zip 4个文件
			WCHAR szBuffer[MAX_BUFFER_SIZE/2];
			if(false != bRet)
				for(auto i=1; i<=4 ;i++)
				{
					wstring strNameSplited = wstring(L"ffmpeg.") + _itow(i,szBuffer, 10) + SERVER_FILE_EXTENTION_W;
					UpdateProgressUI(10 + i*(90-10)/4, wstring(L"正在下载 "+strNameSplited +L"...").c_str());

					wstring strFileSplited = strDir +L"\\"+ strNameSplited;
					wstring strLinkSplited = LINK_SERVER_PATH + strNameSplited;

					bRet = AutoUpdateThread::DownloadFile(strLinkSplited, strFileSplited);
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