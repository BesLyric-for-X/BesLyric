#pragma once
#include "stdafx.h"
#include "core/shostwnd.h"
#include "core/smsgloop.h"
#include "core/SHostDialog.h"
#include "entity\CheckIntegrityThread.h"

class DlgCheckIntegrity : public SHostDialog
{
	SOUI_CLASS_NAME(DlgCheckIntegrity, L"dlgparamtable")
public:
	DlgCheckIntegrity(LPCTSTR pszXmlName) :SHostDialog(pszXmlName){};

	void OnClose();
	~DlgCheckIntegrity(void){}

	//为了与其他线程同步，在获得窗口句柄后才允许其他线程操作，而预先传入事件Handle，
	void SetEventHandleForWndHandle(HANDLE eventHandle, HWND* phWnd, CCheckIntegrityThread* pCheckThread);

	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	
	//更新UI显示
	// wParam 是百分比数，lParam 是提示宽字符串地址
	BOOL OnUpdateCheckProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	//关闭UI显示线程
	BOOL OnCloseProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	

protected:
	void OnCancel();//效果是关闭窗口

	EVENT_MAP_BEGIN()
		EVENT_ID_COMMAND(R.id.btn_cancel_intigrity_check, OnCancel)
	EVENT_MAP_END()


	BEGIN_MSG_MAP_EX(DlgCreateHole)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnCancel)
		
		MESSAGE_HANDLER(MSG_USER_UPDATE_CHECK_PROGRESS, OnUpdateCheckProgress)
		MESSAGE_HANDLER(MSG_USER_CLOSE_CHECK_PROGRESS, OnCloseProgress)

		CHAIN_MSG_MAP(SHostDialog)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

private:
	SStatic*	m_txtProgressTip;
	SProgress*	m_progressChecking;

	//用于与其他线程同步
	HANDLE		m_EventWndInitDone;		//取得窗口句柄后需要设置的事件
	HWND*		m_pHwnd;				//需要传出的窗口句柄指针

	CCheckIntegrityThread*	m_pCheckThread;//检测完整性的线程类
};
