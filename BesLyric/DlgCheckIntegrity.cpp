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
