#include "stdafx.h"
#include "AutoUpdateThread.h"

#include <wininet.h>					//链接网络
#pragma comment( lib, "wininet.lib" ) 
#define MAXBLOCKSIZE 1024  

using namespace SOUI;
extern int _MessageBox(HWND hwnd,LPCTSTR content, LPCTSTR tiltle, UINT uType);

AutoUpdateThread* AutoUpdateThread::ms_Singleton = NULL;

//开始线程
bool AutoUpdateThread::Start()
{
#ifdef _DEBUG
	//调试模式不记录登录信息
	m_bFirstEnter = false;
#endif

	if(m_bFirstEnter)
	{
		//发送登录信息记录
		SendLoginInfo();

		m_bFirstEnter = false;
	}

	//启动线程
	if(m_handleThread != NULL)
		return true;

	m_handleThread = ::CreateThread(NULL, 0, ThreadProc, this, 0 ,NULL);

	return m_handleThread != NULL;
}


//挂起线程
DWORD AutoUpdateThread::Pause()
{
	return ::SuspendThread(m_handleThread);
}

//恢复线程
DWORD AutoUpdateThread::Resume()
{
	return ::ResumeThread(m_handleThread);
}

//停止线程
void AutoUpdateThread::Stop()
{
	if(m_handleThread == NULL)
		return;

	m_bKeepUpdate = false;


	::WaitForSingleObject(m_handleThread, INFINITE);

	CloseHandle(m_handleThread);
}

//线程执行地址
DWORD WINAPI AutoUpdateThread::ThreadProc(LPVOID pParam)
{
	AutoUpdateThread* pThread = static_cast<AutoUpdateThread*>(pParam);
	
#if _KEEP_UPDATE == 0
	pThread->m_bKeepUpdate = false;
#endif

	while(pThread->m_bKeepUpdate)
	{
		//如果还没更新
		if(!pThread->m_bHasUpdate)
			pThread->AutoUpdate();
		
		Sleep(pThread->m_nDelay);
	}

	return 0;
}

//自动更新执行函数
bool AutoUpdateThread::AutoUpdate()
{
	bool bRet;
	string strVersionFile = FileHelper::GetCurrentDirectoryStr()+ "version";
	string strLastExe =  FileHelper::GetCurrentDirectoryStr()+ "BesLyric";

	/*下载最新版本配置信息 */
	bRet = DownloadFile(L"http://files.cnblogs.com/files/BensonLaur/lastVersion.zip", wstring(S_CA2W( SStringA(strVersionFile.c_str()).GetBuffer(1) )));
	if(bRet == false)
		return false;

	SStringW strVersion = L"";
	vector<SStringW> vecLineW;
	FileOperator::ReadAllLinesW(strVersionFile, &vecLineW);

	if(vecLineW.begin() != vecLineW.end())
		strVersion = * vecLineW.begin();

	//识别文件不存在情况
	if(vecLineW.size() >= 2)
	{
		SStringW secondLine = *(vecLineW.begin()+1);
		if(secondLine.GetLength()!=0)
		{
			if(secondLine.GetLength() == 6 && secondLine == L"<html>")
			{
				//为页面不存在的提示，即版本文件不存在，设置为已更新
				m_bHasUpdate = true;

				return false;
			}
		}
	}

	if(VersionCompare(VERSION_NUMBER, strVersion) >= 0 ) // 版本一致 或者 当前版本大于服务器版本（处于开发状态）
	{
		m_bHasUpdate = true;
		return true;
	}
	
	/*有新版本，则直接下载最新的版本 执行文件 */
	//下载新的版本日志文件 versionLog.txt （服务器的名称为 versionLog.zip）
	string strVersionLog =  FileHelper::GetCurrentDirectoryStr()+ "versionLog.txt";
	if(FileHelper::CheckFileExist(strVersionLog))
		remove(strVersionLog.c_str());
	bRet = DownloadFile(L"http://files.cnblogs.com/files/BensonLaur/versionLog.zip",wstring(S_CA2W( SStringA(strVersionLog.c_str()).GetBuffer(1) )));
	if(bRet == false)
		return false;

	//下载最新的执行文件  BesLyric.exe（服务器的名称为 BesLyricExe.zip） 到 strLastExe （BesLyric）中
	bRet = DownloadFile(L"http://files.cnblogs.com/files/BensonLaur/BesLyricExe.zip",wstring(S_CA2W( SStringA(strLastExe.c_str()).GetBuffer(1) )));
	if(bRet == false)
		return false;

	/*修改文件名称，达到替换旧版本目的 */
	string strCurrentExe = strLastExe + ".exe";
	string strBackupExe =  strLastExe+ "."+ S_CW2A(VERSION_NUMBER).GetBuffer(1) ;//+ ".exe";

	if(FileHelper::CheckFileExist(strBackupExe))//删除可能存在的备份文件
		remove(strBackupExe.c_str());
	rename(strCurrentExe.c_str(),strBackupExe.c_str());
	rename(strLastExe.c_str(),strCurrentExe.c_str());
	

	/*发送消息，提示用户重启以使用新版本 */
	//获取更新的内容记录
	SStringW strUpdateInfo = L"";
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
	
	SStringW strShownInfo = L"软件有更新，更新将在下一次启动生效  ：) \\n\\n最新版本更新内容：\\n";
	strShownInfo += strUpdateInfo;
	_MessageBox(NULL, strShownInfo, _T("软件更新提示"),MB_OK| MB_ICONINFORMATION);



	//设置为已更新
	m_bHasUpdate = true;

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

	string strTempFile =  FileHelper::GetCurrentDirectoryStr()+ "temp";
	if(FileHelper::CheckFileExist(strTempFile))
		remove(strTempFile.c_str());
	DownloadFile(L"http://www.ip138.com/ip2city.asp",wstring(S_CA2W( SStringA(strTempFile.c_str()).GetBuffer(1) )));
	
	string strIP="";
	vector<string>	vecLine;
	FileOperator::ReadAllLines(strTempFile,&vecLine);
	for(auto iter = vecLine.begin(); iter != vecLine.end(); iter++)
	{
		if(iter->find_first_of('[') != string::npos)
		{
			auto beg = iter->find_first_of('[')+1;
			auto end = iter->find_first_of(']');
			strIP = iter->substr(beg, end - beg);
			break;
		}
	}

	//访问链接，服务端负责记录登录信息
	string strSendLink = "http://bensonlaur.vicp.io/BesBlog/beslyric/login.action?ip=" + strIP;
	DownloadFile(wstring(S_CA2W( SStringA(strSendLink.c_str()).GetBuffer(1) )),wstring(S_CA2W( SStringA(strTempFile.c_str()).GetBuffer(1) )));
}
