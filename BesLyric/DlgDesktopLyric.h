#pragma once
#include "stdafx.h"
#include "core/shostwnd.h"
#include "stdafx.h"

class DlgDesktopLyric : public SHostWnd
{
	//SOUI_CLASS_NAME(DlgDesktopLyric, L"dlgDesktopLyric")
public:
	DlgDesktopLyric(LPCTSTR pszResName = NULL):SHostWnd(pszResName),m_txtLyric(NULL){}

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
		DLOG("==============================");
		DLOG(string("m_txtLyric:")+ SStringA().Format("%x",m_txtLyric).GetBuffer(1));

		
		if(!m_txtLyric)
			m_txtLyric = this->FindChildByID(R.id.txt_current_lyric);
		
		DLOG(string("m_txtLyric:")+ SStringA().Format("%x",m_txtLyric).GetBuffer(1));
		DLOG(string("String:")+ S_CW2A(strText).GetBuffer(1));
		if(m_txtLyric)
		{
			m_txtLyric->SetWindowTextW(strText);
			DLOG("Log!");
		}

		DLOG("------------------------------");
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
