#include "stdafx.h"
#include "LoadAndCheckNcmIDThread.h"
#include "NcmIDManager.h"

//开始线程
bool CLoadAndCheckNcmIDThread::Start()
{
	//启动线程
	m_handleThread = ::CreateThread(NULL, 0, ProcLoadAndCheckNcmID, this, 0 ,NULL);

	return (m_handleThread != NULL);
}

//结束线程
void CLoadAndCheckNcmIDThread::Stop(){}

//检测（与下载） 线程执行地址
DWORD WINAPI CLoadAndCheckNcmIDThread::ProcLoadAndCheckNcmID(LPVOID pParam)
{
	CNcmIDManager::GetInstance()->LoadAndCheckDataPairs();

	return 0;
}
