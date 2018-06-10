#include "stdafx.h"
#include <helper/SAdapterBase.h>
#include <iomanip>
#include "DlgCheckIntegrity.h"


void DlgCheckIntegrity::SetEventHandleForWndHandle(HANDLE eventHandle, HWND* phWnd,CCheckIntegrityThread* pCheckThread)
{
	m_pHwnd = phWnd;
	m_EventWndInitDone = eventHandle;

	m_pCheckThread = pCheckThread;
}

BOOL DlgCheckIntegrity::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	m_txtProgressTip = NULL;
	m_progressChecking = NULL;

	m_txtProgressTip = FindChildByID2<SStatic>(R.id.text_status_intigrity_check);
	m_progressChecking = FindChildByID2<SProgress>(R.id.progress_check_integrity);
	
	SASSERT( m_txtProgressTip!=NULL);
	SASSERT( m_progressChecking!=NULL);

	//将窗口句柄传出，并设置事件
	*m_pHwnd = this->m_hWnd;

	::SetEvent(m_EventWndInitDone);

	return TRUE;
}


BOOL DlgCheckIntegrity::OnUpdateCheckProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	int nPercentage = (int)wParam;
	WCHAR* pProgressTip = (WCHAR*)lParam;

	m_txtProgressTip->SetWindowTextW(pProgressTip);
	m_progressChecking->SetValue(nPercentage);

	return TRUE;
}

//WPARAM 为下载速度，单位 byte/ms  
BOOL DlgCheckIntegrity::OnUpdateProgressSpeed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	double dBytePerSecond = (ULONG)wParam * 1.0 * 1000;
	SStringW strSpeed;
	if(dBytePerSecond < 1024)
		strSpeed.Format(L"速度 %d B/s", (int)dBytePerSecond);
	else if(dBytePerSecond < 1024 * 1024)
		strSpeed.Format(L"速度 %.2f KB/s", dBytePerSecond/1024);
	else 
		strSpeed.Format(L"速度 %.2f MB/s", dBytePerSecond/1024/1024);
	
	FindChildByID2<SStatic>(R.id.text_speed)->SetWindowTextW(strSpeed);
	
	return TRUE;
}

//WPARAM 接收总数，单位 byte  
BOOL DlgCheckIntegrity::OnUpdateProgressTotal(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{	
	ULONG nTotalByte = (ULONG)wParam;
	
	SStringW strTotal;
	
	if(nTotalByte < 1024)
		strTotal.Format(L"已接收 %d B;", nTotalByte);
	else if(nTotalByte < 1024 * 1024)
		strTotal.Format(L"已接收 %.2f KB;", 1.0 * nTotalByte/1024);
	else 
		strTotal.Format(L"已接收 %.2f MB;", 1.0 * nTotalByte/1024/1024);
	
	FindChildByID2<SStatic>(R.id.text_total_recieve)->SetWindowTextW(strTotal);
	
	return TRUE;
}

BOOL DlgCheckIntegrity::OnCloseProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	OnClose();
	return TRUE;
}

void DlgCheckIntegrity::OnClose()
{
	SHostDialog::OnOK();
}

void DlgCheckIntegrity::OnCancel()
{
	m_pCheckThread->Stop(); //结束线程（这里实际没做什么）

	SHostDialog::OnCancel();
}
