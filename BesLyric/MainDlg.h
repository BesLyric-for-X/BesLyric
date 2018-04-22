/*
	BesLyric  一款 操作简单、功能实用的 专门用于制作网易云音乐滚动歌词的 歌词制作软件。
    Copyright (C) 2017  
	Author: BensonLaur <BensonLaur@163.com>
	Author:

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
* @file       MainDlg.h
* 
* Describe    主窗口类，提供界面的消息处理接口； 以及部分页面业务逻辑的接口和实现
*/

#pragma once
#include "entity/LyricMaker.h"
#include "entity/LyricPlayer.h"
#include "BSMessageBox.h"
#include  "PageSetting.h"
#include "PageMaking.h"
#include "PageResult.h"
#include "PageSearchLyric.h"
#include "PageSearchNcmID.h"

#include "controlEx\MagnetFrame.h"
#include "DlgDesktopLyric.h"

class CPageMaking;						//嵌套定义，先声明
class CPageResult;
class CPageSearchLyric;
class CPageSearchNcmID;

/* 程序的主窗口类 */
class CMainDlg : public SHostWnd, CMagnetFrame
{
	friend class CPageMaking;
	friend class CPageResult;
	friend class CPageSearchLyric;
	friend class CPageSearchNcmID;

public:
	CMainDlg();
	~CMainDlg();

	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	void OnSize(UINT nType, CSize size);

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

	//初始化各个页面内容
	void initPage();

	//初始化etc文件并清理相关文件
	void initFloderAndFile();

	//初始化桌面歌词
	void initDesktopLyric();

	void OnPageChanged(EventArgs *pEvt);	//主页面切换时

	//在拖动文件到 Edit控件时
	int MsgDropFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	void OnDropFile(SEdit* pEdit, wstring strFilePath);
	
	//接受键盘输入
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	//接收音乐消息
	void OnMusicCommand(UINT lParam, UINT wParam);

	//接收定时器消息，用于滚动歌词动画的实现
	void OnTimer(UINT_PTR nIDEvent);

	//静音按钮 实际相应操作
	void OnBtnSoundOpen();
	void OnBtnSoundClose();
	
	//处理声音slider 位置的变化
	void OnSliderPos(bool isPos1);

	 //用于处理格式转换线程结束后，通知主线程播放
	int MessageButtonCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	
	//处理消息，显示搜索到的歌词
	int MessageShowLyricResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	//处理消息，显示搜索到的ID
	int MessageShowIDResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	

	//处理消息，使用猜测的结果自动搜索歌词
	int MessageSearchWithGuessResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	//处理消息，使用猜测的结果自动搜索ID
	int MessageSearchIDWithGuessResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

private:
	//设置程序的背景图片
	void setBackSkin();

	// 由于希望在 CPageMaking 类中直接访问CMainDlg 成员，CPageMaking，CMainDlg会嵌套定义，互相之间只能仅仅声明指针
	// 而CMainDlg 中宏定义 CHAIN_EVENT_MAP_MEMBER 实际上会直接使用 CPageMaking 的 _HandleEvent；故封装该函数，在实现中调用_HandleEvent
	BOOL PageMakingChainEvent(CPageMaking* pPageMaking,EventArgs* pEvt);
	BOOL PageResultChainEvent(CPageResult* pPageResult,EventArgs* pEvt); //同上
	BOOL PageSearchLyricChainEvent(CPageSearchLyric* pPageSearchLyric,EventArgs* pEvt);
	BOOL PageSearchNcmIDChainEvent(CPageSearchNcmID* pPageSearchNcmID,EventArgs* pEvt);

	void test();//just for test
protected:
	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)

		//页面切换响应
		EVENT_NAME_HANDLER(R.name.tab_main, EventTabSelChanged::EventID, OnPageChanged)

		//子窗口的消息
		CHAIN_EVENT_MAP_MEMBER(m_settingPage)

		 if(PageMakingChainEvent(m_pageMaking,pEvt))return TRUE;           
		 if(PageResultChainEvent(m_pageResult,pEvt))return TRUE;           
		 if(PageSearchLyricChainEvent(m_pageSearchLyric,pEvt))return TRUE; 
		 if(PageSearchNcmIDChainEvent(m_pageSearchNcmID,pEvt))return TRUE; 

	EVENT_MAP_END()

	//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CMainDlg)

		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)

		//处理键盘按键消息
		MSG_WM_KEYDOWN(OnKeyDown)

		//接收 mciSendCommand 函数 执行音乐播放 相关操作时 发出的通知消息
		if (uMsg == MM_MCINOTIFY ) 
		{ 
			SetMsgHandled(TRUE); 
			OnMusicCommand((UINT)lParam, (UINT)(wParam)); 
			lResult = 0; 
			if(IsMsgHandled()) 
				return TRUE; 
		}

		MSG_WM_TIMER(OnTimer)
			
		MESSAGE_HANDLER(MSG_USER_MAKING_START_BUTTON, MessageButtonCommand)   //用于格式转换线程结束后，通知主线程播放
		MESSAGE_HANDLER(MSG_USER_PLAYING_START_BUTTON, MessageButtonCommand)

		MESSAGE_HANDLER(MSG_USER_DROP_FILE, MsgDropFile)//拖放文件消息
		
		MESSAGE_HANDLER(MSG_USER_SHOW_LYRIC_RESULT, MessageShowLyricResult)//显示歌词结果消息
		MESSAGE_HANDLER(MSG_USER_SHOW_ID_RESULT, MessageShowIDResult)//显示歌词结果消息
		

		MESSAGE_HANDLER(MSG_USER_SEARCH_WITH_GUESS_RESULT, MessageSearchWithGuessResult) //使用猜测的结果自动搜索
		MESSAGE_HANDLER(MSG_USER_SEARCH_NCM_WITH_GUESS_RESULT, MessageSearchIDWithGuessResult) //使用猜测的结果自动搜索(搜索ID)
		

		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

public:
	CSettingPage m_settingPage;				/* 设置页面 */
	CPageMaking *m_pageMaking;				/* 歌词制作页面 */
	CPageResult *m_pageResult;				/* 歌词预览页面 */
	CPageSearchLyric *m_pageSearchLyric;	/* 搜索歌词页面 */

	CPageSearchNcmID *m_pageSearchNcmID;	/* 搜索歌曲网易云音乐ID页面 */

	DlgDesktopLyric *m_wndDesktopLyric;			/* 桌面歌词窗口 */	

public:
	LyricMaker maker;						/* 歌词制作器 */
	LyricPlayer player;						/* 歌词滚动播放器 */

private:
	BOOL			m_bLayoutInited;
	BOOL			m_bIsLyricMaking;		/* 是否正在制作歌词当中，觉得是否处理键盘消息 ，在制作歌词过程中不能预览歌词*/
	BOOL			m_bIsLyricPlaying;		/* 是否正在预览歌词当中，在预览歌词过程中，不能制作歌词 */
	
	int m_nVolumn;							/* 当前音量 0 ~ 1000 */
	int m_nVolumnBeforeMute;				/* 储存静音前的音量*/

	// 程序的两个页面都需要播放音乐，在音乐自然结束时， void OnMusicCommand(UINT lParam, UINT wParam) 会被调用
	// 然而， lParam, wParam 都无法提供更多的关于 消息发送者的信息
	// 故在此定义FootPrintPage。
	// FootPrintPage 里保留着最后播放时所在的页面；由此决定当前 音乐结束是来自哪个页面
	// FootPrintPage 分别会在 歌词制作页面“开始制作”按钮，和滚动预览“开始播放”按钮被按下之后被记录页面状态,分别是 [0,1]
	int FootPrintPage;

};

