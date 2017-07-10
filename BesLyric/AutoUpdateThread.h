#pragma once
#include "stdafx.h"
#include "WinFile.h"
#include "SSingleton.h"
using namespace SOUI;


class  AutoUpdateThread : public Singleton<AutoUpdateThread>
{
public:
	AutoUpdateThread(int nDelay = 1000):m_nDelay(nDelay),m_handleThread(NULL),m_bKeepUpdate(true),m_bHasUpdate(false),m_bFirstEnter(true){}

	//开始线程
	bool Start();

	//挂起线程
	DWORD Pause();

	//恢复线程
	DWORD Resume();

	//停止线程
	void Stop();

private:
	
	//线程执行地址
	static DWORD WINAPI ThreadProc(LPVOID pParam);

	//自动更新执行函数
	bool AutoUpdate();

	//比较2个字符串版本号的大小，
	int VersionCompare(const SStringW v1, const SStringW v2);

	//从网络下载文件
	bool DownloadFile(const wstring strUrl, const wstring strSaveAs);
	

	//发送登录信息（ip地址）
	void SendLoginInfo();

	bool m_bFirstEnter;					/* 第一次启动线程 */
private:

	HANDLE		m_handleThread;			/* 当前线程句柄 */
	bool		m_bKeepUpdate;			/* 保持更新标志 */
	bool		m_bHasUpdate;			/* 是否已经更新 */
	int			m_nDelay;				/* 检查更新的延时，单位ms */
};

