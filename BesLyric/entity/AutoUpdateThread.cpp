#include "stdafx.h"
#include "AutoUpdateThread.h"
#include "Define.h"
#include <wininet.h>					//链接网络
#pragma comment( lib, "wininet.lib" ) 
#define MAXBLOCKSIZE 1024  

using namespace SOUI;

AutoUpdateThread* AutoUpdateThread::ms_Singleton = NULL;


//开始线程
bool AutoUpdateThread::Start()
{
	//启动线程
	if(m_handleThread != NULL)
		return true;

	//创建时间，事件有信号时，表示停止等待
	// ManualReset = false， 表示 WaitSingleObject 收到有信号的  m_EventStopWaiting 后，m_EventStopWaiting自动变为无信号
	m_EventStopWaiting = ::CreateEvent(NULL, FALSE, FALSE,NULL);

	m_handleThread = ::CreateThread(NULL, 0, ThreadProc, this, 0 ,NULL);

	return m_handleThread != NULL;
}

//停止线程
void AutoUpdateThread::Stop()
{
	if(m_handleThread == NULL)
		return;

	m_bLoop = false;

	//设置 m_EventStopWaiting 为有信号，以结束 ThreadProc 中的循环的等待
	SetEvent(m_EventStopWaiting);

	::WaitForSingleObject(m_handleThread, INFINITE);

	CloseHandle(m_handleThread);
	CloseHandle(m_EventStopWaiting);
}

//线程执行地址
DWORD WINAPI AutoUpdateThread::ThreadProc(LPVOID pParam)
{
	AutoUpdateThread* pThread = static_cast<AutoUpdateThread*>(pParam);
	
	
#ifdef _DEBUG
	//调试模式不记录登录信息
	pThread->m_bFirstEnter = false;
#endif

	if(pThread->m_bFirstEnter)
	{
		//发送登录信息记录
		pThread->SendLoginInfo();

		pThread->m_bFirstEnter = false;
	}


#if UPDATE_LOOP == 0
	pThread->m_bLoop = false;
#endif

	while(pThread->m_bLoop)
	{
		if(pThread->m_bKeepUpdate && pThread->IfUpdateAvailable())
			pThread->AutoUpdate();
		
		WaitForSingleObject(pThread->m_EventStopWaiting, pThread->m_nDelay);
		//Sleep(pThread->m_nDelay);
	}

	return 0;
}

//自动更新执行函数
bool AutoUpdateThread::AutoUpdate()
{
	wstring strVersionFile = FileHelper::GetCurrentDirectoryStr()+ FILE_NAME_LAST_VERSION_INFO;
	wstring strLastExe =  FileHelper::GetCurrentDirectoryStr()+ FILE_NAME_LAST_EXE_TEMP;

	/*有新版本，则直接下载最新的版本 执行文件 */
	//下载新的版本日志文件 versionLog.txt （服务器的名称为 versionLog.zip）
	wstring strVersionLog =  FileHelper::GetCurrentDirectoryStr()+ L"versionLog.txt";
	if(FileHelper::CheckFileExist(strVersionLog))
		_wremove(strVersionLog.c_str());
	bool bRet = DownloadFile( LINK_VERSION_LOG, strVersionLog );
	if(bRet == false)
		return false;

	//下载最新的执行文件  BesLyric.exe（服务器的名称为 BesLyricExe.zip） 到 strLastExe （BesLyric）中
	bRet = DownloadFile( LINK_LAST_EXE , strLastExe);
	if(bRet == false)
		return false;

	/*修改文件名称，达到替换旧版本目的 */
	wstring strCurrentExe = strLastExe + L".exe";
	wstring strBackupExe =  strLastExe+ L"."+ VERSION_NUMBER ;//+ ".exe";

	if(FileHelper::CheckFileExist(strBackupExe))//删除可能存在的备份文件
		_wremove(strBackupExe.c_str());
	_wrename(strCurrentExe.c_str(),strBackupExe.c_str());
	_wrename(strLastExe.c_str(),strCurrentExe.c_str());
	

	/*发送消息，提示用户重启以使用新版本 */
	SStringW strVersion = L"";
	vector<SStringW> vecLineW;
	bRet = FileOperator::ReadAllLinesW(strVersionFile, &vecLineW);

	
	//获取更新的内容记录
	SStringW strUpdateInfo = L"";

	if(bRet == false)
		strUpdateInfo = L"更新信息不见了！不要在意这些细节 ：)";
	else
	{
		if(vecLineW.size() == 1 || vecLineW.size() == 2)
			strUpdateInfo = L"更新信息不见了！不要在意这些细节 ：)";
		else
		{
			int nLine = 0;
			for(auto iter = vecLineW.begin(); iter!= vecLineW.end(); iter++,nLine++)
			{
				if(nLine >= 2)
				{
					strUpdateInfo += *iter;
					strUpdateInfo += L"\\n";
				}
			}
		}
	}
	
	SStringW strShownInfo = L"软件有更新，更新将在下一次启动生效  ：) \\n\\n最新版本更新内容：\\n";
	strShownInfo += strUpdateInfo;
	_MessageBox(NULL, strShownInfo, _T("软件更新提示"),MB_OK| MB_ICONINFORMATION);

	//提示一次之后，暂时不再检测更新
	m_bKeepUpdate = false;

	return true;
}

//检测是否有更新
bool AutoUpdateThread::IfUpdateAvailable()
{
	wstring strVersionFile = FileHelper::GetCurrentDirectoryStr()+ FILE_NAME_LAST_VERSION_INFO;
	wstring strLastExe =  FileHelper::GetCurrentDirectoryStr()+ FILE_NAME_LAST_EXE_TEMP;

	/*下载最新版本配置信息 */
	bool bRet = DownloadFile( LINK_LAST_VERSION_INFO, strVersionFile);
	if(bRet == false)
		return false;//访问出错

	SStringW strVersion = L"";
	vector<SStringW> vecLineW;
	FileOperator::ReadAllLinesW(strVersionFile, &vecLineW);

	if(vecLineW.begin() != vecLineW.end())
		strVersion = * vecLineW.begin();

	//识别更新文件不存在情况
	if(vecLineW.size() >= 2)
	{
		SStringW secondLine = *(vecLineW.begin()+1);
		if(secondLine.GetLength()!=0)
		{
			if(secondLine.GetLength() == 6 && secondLine == L"<html>")
			{
				//为页面不存在的提示，即版本文件不存在, 则不更新
				return false;
			}
		}
	}

	if(VersionCompare(SStringW(VERSION_NUMBER.c_str()), strVersion) >= 0 ) // 版本一致 或者 当前版本大于服务器版本（处于开发状态）
	{
		return false;
	}

	//有更新可用
	return true;
}

//比较2个字符串版本号的大小，
int AutoUpdateThread::VersionCompare(const SStringW v1, const SStringW v2)
{
	int nMainNum1 = 0, nSubNum1= 0, nModifidNum1= 0;
	int nMainNum2 = 0, nSubNum2= 0, nModifidNum2= 0;
	swscanf(v1,L"%d.%d.%d",&nMainNum1, &nSubNum1, &nModifidNum1);
	swscanf(v2,L"%d.%d.%d",&nMainNum2, &nSubNum2, &nModifidNum2);

	if(nMainNum1 > nMainNum2)
		return 1;
	else if(nMainNum1 < nMainNum2)
		return -1;
	else//主版本号相同
	{
		if(nSubNum1 > nSubNum2)
		return 1;
		else if(nSubNum1 < nSubNum2)
			return -1;
		else//次版本号相同
		{
			if(nModifidNum1 > nModifidNum2)
				return 1;
			else if(nModifidNum1 < nModifidNum2)
				return -1;
			else
				return 0;
		}
	}

}


/*将Url指向的地址的文件下载到save_as指向的本地文件*/
bool AutoUpdateThread::DownloadFile(const wstring strUrl, const wstring strSaveAs)
{
	byte Temp[MAXBLOCKSIZE];
	ULONG Number = 1;

	FILE *stream;
	HINTERNET hSession = InternetOpen(L"RookIE/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession != NULL)
	{
		HINTERNET handle2 = InternetOpenUrl(hSession, strUrl.c_str(), NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
		if (handle2 != NULL)
		{
			if ((_wfopen_s(&stream, strSaveAs.c_str(), L"wb")) == 0)
			{
				while (Number > 0)
				{
					InternetReadFile(handle2, Temp, MAXBLOCKSIZE - 1, &Number);

					fwrite(Temp, sizeof (char), Number, stream);
				}
				fclose(stream);
			}
			else
				return false;

			InternetCloseHandle(handle2);
			handle2 = NULL;
		}
		else
			return false;
		InternetCloseHandle(hSession);
		hSession = NULL;
	}

	return true;
}




//发送登录信息（ip地址）
void AutoUpdateThread::SendLoginInfo()
{
	//获得ip地址
	//DownloadFile(L"https://whatismyipaddress.com/",L"E://git//BesLyric//Release//ip.txt");
	//DownloadFile(L"http://ip.qq.com/",L"E://git//BesLyric//Release//ip.txt");

	//最大检测ip的次数
	int nMaxCheckCount = 5;
	
	wstring strIP= L"unknown";
	wstring strTempFile;

	while(nMaxCheckCount)
	{
		strTempFile =  FileHelper::GetCurrentDirectoryStr()+ L"temp";
		if(FileHelper::CheckFileExist(strTempFile))
			_wremove(strTempFile.c_str());
	
		bool bRet = DownloadFile(L"http://www.ip138.com/ip2city.asp",strTempFile);
		if(bRet == false)
		{
			//可能没网络，或网络异常，也可能读取文件失败
			//等待5秒再检测
			nMaxCheckCount--;
			Sleep(5000);
			continue;
		}

		vector<SStringW> vecLine;
		bRet = FileOperator::ReadAllLinesW(strTempFile,&vecLine);
		if(bRet == false)
		{
			//文件读取异常，5秒后重新检测过程
			nMaxCheckCount--;
			Sleep(5000);
			continue;
		}

		for(auto iter = vecLine.begin(); iter != vecLine.end(); iter++)
		{
			if(iter->Find(L'[') != -1)
			{
				auto beg = iter->Find(L'[')+1;
				auto end = iter->Find(L']');
				strIP = iter->Right(beg).Left(end);
				break;
			}
		}

		//已经获得ip，退出循环
		break;
	}

	//访问链接，服务端负责记录登录信息
	wstring strSendLink = LINK_SEND_LOGIN + L"?ip=" + strIP;
	DownloadFile(strSendLink, strTempFile);
}
