#pragma once
#include "stdafx.h"
#include "core/shostwnd.h"
#include "stdafx.h"

class DlgDesktopLyric : public SHostWnd
{
	//SOUI_CLASS_NAME(DlgDesktopLyric, L"dlgDesktopLyric")
public:
	DlgDesktopLyric(LPCTSTR pszResName = NULL):SHostWnd(pszResName){}

	void CreateAndInitWindow()
	{
		Create(NULL,WS_POPUP,WS_EX_TOPMOST,0,0,0,0);

		//CRect rect = GetWindowRect();
		//m_wndDesktopLyric->SetWindowPos( HWND_TOPMOST, rect.left, rect.top, rect.Width(), rect.Height(),SWP_NOACTIVATE);

		BringWindowToTop();
		
		SetCurrentLyric(SStringW(L"¸è´Ê"));
	}


	void SetCurrentLyric(SStringW strText)
	{
		if(m_txtLyric)
			m_txtLyric = this->FindChildByName(L"txt_current_lyric");
		
		if(m_txtLyric)
			m_txtLyric->SetWindowTextW(strText);
	}

	void ShowDesktopLyric()
	{
		ShowWindow(SW_SHOW);
	}

	void HideDesktopLyric()
	{
		ShowWindow(SW_HIDE);
	}

private:
	SWindow *m_txtLyric;
};
