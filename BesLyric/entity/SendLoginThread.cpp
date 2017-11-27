#include "stdafx.h"
#include "SendLoginThread.h"
#include "Define.h"
#include "../utility/Downloader.h"
#include <wininet.h>					//链接网络
#pragma comment( lib, "wininet.lib" ) 

using namespace SOUI;


//开始线程
bool SendLoginThread::Start(BOOL bAnonymity)
{
	//启动线程
	if(m_handleThread != NULL)
		return true;

	m_bAnonymity = bAnonymity;
	m_handleThread = ::CreateThread(NULL, 0, ThreadProc, this, 0 ,NULL);

	return m_handleThread != NULL;
}

//线程执行地址
DWORD WINAPI SendLoginThread::ThreadProc(LPVOID pParam)
{
	SendLoginThread* pThread = static_cast<SendLoginThread*>(pParam);


#ifndef _DEBUG  

	//调试模式下不发送登录信息

	//发送登录信息记录
	pThread->SendLoginInfo(pThread->m_bAnonymity);

#endif

	return 0;
}

//发送登录信息（ip地址）
void SendLoginThread::SendLoginInfo(BOOL bAnonymity)
{
	//获得ip地址
	//DownloadFile(L"https://whatismyipaddress.com/",L"E://git//BesLyric//Release//ip.txt");
	//DownloadFile(L"http://ip.qq.com/",L"E://git//BesLyric//Release//ip.txt");

	//最大检测ip的次数
	int nMaxCheckCount = 5;
	
	wstring strIP= L"unknown";
	wstring strTempFile;

	if(bAnonymity)
	{
		strIP = L"Anonymity";
		strTempFile =  FileHelper::GetCurrentDirectoryStr()+ L"temp";
		if(FileHelper::CheckFileExist(strTempFile))
			_wremove(strTempFile.c_str());
	}
	else
	{
		while(nMaxCheckCount)
		{
			strTempFile =  FileHelper::GetCurrentDirectoryStr()+ L"temp";
			if(FileHelper::CheckFileExist(strTempFile))
				_wremove(strTempFile.c_str());
	
			bool bRet = CDownloader::DownloadFile(L"http://www.ip138.com/ip2city.asp",strTempFile);
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
					strIP = iter->Left(end);
					strIP = strIP.substr(beg);
					break;
				}
			}

			//已经获得ip，退出循环
			break;
		}
	}

	//访问链接，服务端负责记录登录信息
	wstring strSendLink = LINK_SEND_LOGIN + L"?ip=" + strIP;
	CDownloader::DownloadFile(strSendLink, strTempFile);
}
