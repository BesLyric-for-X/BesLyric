#pragma once
#include "stdafx.h"
#include "WinFile.h"
#include "SSingleton.h"
using namespace SOUI;


class  AutoUpdateThread : public Singleton<AutoUpdateThread>
{
public:
	// 设置检查更新的间隔为10分钟
	AutoUpdateThread(int nDelay = 1000 * 60 * 10):m_nDelay(nDelay),m_handleThread(NULL),m_bLoop(true),m_bKeepUpdate(false),m_bFirstEnter(true){}

	//开始线程
	bool Start();

	//停止线程
	void Stop();
	
	//设置是否持续检测更新
	void SetBKeepUpdate(BOOL bValue)
	{
		m_bKeepUpdate = (bValue ? true : false);
		if(m_bKeepUpdate)
			//设置 m_EventStopWaiting 为有信号，以结束 ThreadProc 中的循环的等待
			SetEvent(m_EventStopWaiting);
	}
private:
	
	//线程执行地址
	static DWORD WINAPI ThreadProc(LPVOID pParam);

	//自动更新执行函数
	bool AutoUpdate();

	//检测是否有更新
	bool IfUpdateAvailable();

	//比较2个字符串版本号的大小，
	int VersionCompare(const SStringW v1, const SStringW v2);

	//从网络下载文件
	bool DownloadFile(const wstring strUrl, const wstring strSaveAs);
	

	//发送登录信息（ip地址）
	void SendLoginInfo();

	bool m_bFirstEnter;					/* 第一次启动线程 */
private:

	HANDLE		m_handleThread;			/* 当前线程句柄 */
	HANDLE		m_EventStopWaiting;		/* 停止等待时间 */
	bool		m_bLoop;				/* 线程循环标记 */
	bool		m_bKeepUpdate;			/* 保持更新标志 */
	int			m_nDelay;				/* 检查更新的间隔，单位ms */
};

