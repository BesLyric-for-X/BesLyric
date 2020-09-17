#include "stdafx.h"
#include "SendLoginThread.h"
#include "Define.h"
#include "../utility/Downloader.h"
#include "OsInfoHelper.hpp"
#include <wininet.h>					//链接网络
#pragma comment( lib, "wininet.lib" ) 

using namespace SOUI;

#include <regex>		//使用正则表达式匹配IP

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


#ifdef _DEBUG  

	//调试模式下不发送登录信息

	//发送登录信息记录
	pThread->SendLoginInfo(pThread->m_bAnonymity);

#endif

	return 0;
}

//发送登录信息（ip地址）
void SendLoginThread::SendLoginInfo(BOOL bAnonymity)
{
	//获得ip地址的网页源，3个备用
	const int SrcCount = 3;
	vector<wstring> ipSrc;
	ipSrc.push_back(L"http://ip.chacuo.net/");
	ipSrc.push_back(L"http://www.zzsky.cn/code/ip/index.asp");
	ipSrc.push_back(L"https://whatismyipaddress.com/");

	//单个源最大检测ip的次数
	int nMaxSingleCheckCount = 3;
	
	wstring strIP= L"unknown";
	wstring strTempFile;

	if(bAnonymity)
	{
		strIP = L"Anonymity";
		strTempFile =  FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" + FILE_NAME_TEMP;
		if(FileHelper::CheckFileExist(strTempFile))
			_wremove(strTempFile.c_str());
	}
	else
	{
		int nLeftCheckCount = nMaxSingleCheckCount * SrcCount; // 计算总共检测次数
		while(nLeftCheckCount-1)
		{
			strTempFile =  FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" + FILE_NAME_TEMP;
			if(FileHelper::CheckFileExist(strTempFile))
				_wremove(strTempFile.c_str());
	
			wstring url = ipSrc[(nLeftCheckCount-1) / nMaxSingleCheckCount];
			bool bRet = CDownloader::DownloadFile(url,strTempFile);
			if(bRet == false)
			{
				//可能没网络，或网络异常，也可能读取文件失败
				//等待5秒再检测
				nLeftCheckCount--;
				Sleep(5000);
				continue;
			}

			vector<SStringW> vecLine;
			bRet = FileOperator::ReadAllLinesW(strTempFile,&vecLine);
			if(bRet == false)
			{
				//文件读取异常，5秒后重新检测过程
				nLeftCheckCount--;
				Sleep(5000);
				continue;
			}

			bool bFound = false;
			for(auto iter = vecLine.begin(); iter != vecLine.end(); iter++)
			{
				// https://whatismyipaddress.com/ 和 http://ip.qq.com/ 的网页中IP都是 >ddd.ddd.ddd.ddd< 特征，查找该特征的字符串
				wstring ipStr = L"Invalid IP query source";
				
				if(CatchIPStr(iter->GetBuffer(1), ipStr))
				{
					strIP = ipStr;
					bFound = true;
					break;
				}
			}

			if(bFound)
				break;//已经获得ip，退出循环
			
			nLeftCheckCount--;
		}
	}

	wstring systemArch = S_CA2W(OsInfoHelper().GetOsBits().c_str()).GetBuffer(64);

	//访问链接，服务端负责记录登录信息
	wstring strSendLink = LINK_SEND_LOGIN + L"?ip=" + strIP + L"&version=beslyric-soui&vernum="+VERSION_NUMBER 
		+ L"&systemArchitecture=" + systemArch;
	CDownloader::DownloadFile(strSendLink, strTempFile);
}


//俘获满足需求的IP字符串
bool SendLoginThread::CatchIPStr(const wstring &line, OUT wstring& ip)
{
	//(http://ip.chacuo.net/)
	//value="..."

	// (https://whatismyipaddress.com/)
	// > . . . <

	//或 (http://www.zzsky.cn/code/ip/index.asp)
	//   您的IP：...&nbsp;

	// 参考 https://blog.csdn.net/effective_coder/article/details/9010337
	std::locale loc("");    
    std::wcout.imbue(loc);    
        
    std::wstring regString1(_T("value=\"(\\d+\\.\\d+\\.\\d+\\.\\d+)\"")); 

    std::wstring regString2(_T(">(\\d+\\.\\d+\\.\\d+\\.\\d+)<")); 

    std::wstring regString3(_T("您的IP：(\\d+\\.\\d+\\.\\d+\\.\\d+)&nbsp;"));   
    
	//先查找规则1
    if(GetIpByRegString(line, regString1, ip))
		return true;

	//先查找规则2
    if(GetIpByRegString(line, regString2, ip))
		return true;
	
	//查找规则3
	if( GetIpByRegString(line, regString3, ip))
		return true;
	
	return false;
}


bool SendLoginThread::GetIpByRegString(const wstring &line, std::wstring regString, OUT wstring& ip)
{
	// 表达式选项 - 忽略大小写     
    std::regex_constants::syntax_option_type fl = std::regex_constants::icase;    
     
    // 编译一个正则表达式语句     
    std::wregex regExpress(regString, fl);    

	// 保存查找的结果     
    std::wsmatch ms;    
    
    // 查找     
    if(std::regex_search(line, ms, regExpress))    
    {    
		if(ms.size() == 2)
		{
            ip = ms.str(1); 
			return true;
		}
		else
			return false;
    }    
    else    
    {    
        return false;
    }    
}
