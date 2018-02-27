#pragma once
#include "stdafx.h"
#include "core/shostwnd.h"
#include "core/smsgloop.h"
#include "core/SHostDialog.h"
#include "utility\Downloader.h"
#include "utility\WinFile.h"

class DlgDownloadNcmMp3 : public SHostDialog
{
	SOUI_CLASS_NAME(DlgDownloadNcmMp3, L"dlgDownloadMP3")
public:
	void SetNcmNameAndID(wstring strName, wstring strID)
	{
		m_strName = strName;
		m_strID = strID;
	}


	DlgDownloadNcmMp3(LPCTSTR pszXmlName) :SHostDialog(pszXmlName)
	{
		m_bDone=  false;
		m_bErrorOccur = false;

	};

	~DlgDownloadNcmMp3(void)
	{
		//这里KillTimer 会报错，hWnd 窗口已经不存在了
		//this->KillTimer(100);
		//this->KillTimer(101);
	}

private:
	
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam)
	{
		
		//m_progress = NULL;

		//m_progress = FindChildByID2<SProgress>(R.id.progressing_tip);
	
		//SASSERT( m_progress);
		
		m_txtFilePath = NULL;
		m_txtFilePath = FindChildByID2<SStatic>(R.id.text_file_path);
		SASSERT( m_txtFilePath);
		
		m_txtFilePath->SetWindowTextW((m_strName+L".mp3").c_str());

		this->SetTimer(100,1);
		//this->SetTimer(101,1);
		return 0;
	}
	
	//接收定时器消息，用于检测到过程结束后退出窗口
	void OnTimer(UINT_PTR nIDEvent)
	{
		static int nCount = 0;
		switch(nIDEvent)
		{
			case 100:
				nCount++;
				if(nCount == 1)
				{
					//初次进来，启动下载过程

					//构建下载路径 和下载链接
					wstring strLink = L"http://music.163.com/song/media/outer/url?id="+ m_strID +L".mp3";
					wstring strMp3FilePath = FileHelper::GetCurrentDirectoryStr() + TEMP_MP3_FLODER_NAME + L"\\" + m_strName + L".mp3";

					if(!CDownloader::DownloadFile( strLink, strMp3FilePath))
						m_bErrorOccur = true; //发生连接错误
						
					m_bDone = true;
				}
				else
				{
					//检测过程是佛完成
					if(m_bDone)
					{
						nCount = 0; //下一次进来从0开始
						if(m_bErrorOccur)
							OnCancel();
						else
							OnOk();
					}
				}
			break;
			//case 101: //刷进度条
			//	nPercent++;
			//	/*m_progress->SetValue(nPercent >= 100 ? 99: nPercent);*/
			break;
		}
	}

protected:
	void OnOk()
	{
		SHostDialog::OnOK();
	};
	void OnCancel()
	{
		SHostDialog::OnCancel();
	};

	//EVENT_MAP_BEGIN()
	//	EVENT_ID_COMMAND(R.id.btn_cancel_intigrity_check, OnCancel)
	//EVENT_MAP_END()


	BEGIN_MSG_MAP_EX(DlgCreateHole)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnCancel)
		
		MSG_WM_TIMER(OnTimer)

		CHAIN_MSG_MAP(SHostDialog)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

private:
	/*
	SProgress*	m_progress;*/
	SStatic*	m_txtFilePath;

	bool m_bDone;		/* 标记过程是否完成 */
	bool m_bErrorOccur;	/* 标记是否有错误出现 */
	
	wstring m_strName;	/* 下载的文件名 */
	wstring m_strID;		/* 歌曲的网易云音乐的ID */
};
