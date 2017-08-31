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
* @file       PageMaking.h
* 
* Describe    CPageMaking类，制作歌词页面
*/

#pragma once
#include "stdafx.h"
#include "MainDlg.h"

class CMainDlg;							//嵌套定义，先声明
class CPageMaking
{
	friend class CMainDlg;	

public:
	CPageMaking();
	~CPageMaking(){};

	void Init(SHostWnd *pMainWnd);		//初始化设置页面
	CMainDlg* M();						//获得主窗口类实例对象
	
	//各个页面按钮的响应函数

	void OnBtnSelectMusic1(LPCWSTR pFilePath = NULL);		//三个路径的选择	
	void OnBtnSelectLyric1(LPCWSTR pFilePath = NULL);
	void OnBtnSelectOutput(LPCWSTR pDirPath = NULL);
	void OnBtnLoad1();				//制作歌词：加载 (初始化 LyricMaker)
	void OnBtnRestart();			//我要重制
	void OnBtnStartMaking();		//开始制作
	void OnBtnPreview();
	void OnBtnOpenOutput();			//打开生成的文件

	void OnBtnSoundOpen1();			//打开声音
	void OnBtnSoundClose1();		//静音
	
	void OnSliderPos1(EventArgs *pEvt);//处理声音slider 位置的变化
	
protected:
	//第一个页面(歌词制作)：回到“加载按钮”按下后的状态
	void backToInit_1();						/* 回到载入完成后的状态，此函数是为了复用而特意作为一个函数来使用 */
	
private:
	//判断第一个页面(歌词制作) 的 三个路径是否都选择完毕 */
	bool isPathReady_1();
	/* 获得当前未初始化的第一个路径 在PATH_STATE_1 中对应的下标;都初始化了则返回-1 */
	int getPathNotReady_1();

	//线程执行地址(格式转换线程)
	static DWORD WINAPI ThreadConvertProc(LPVOID pParam);


	//消息
protected:
	EVENT_MAP_BEGIN()
		EVENT_CHECK_SENDER_ROOT(m_pMainWnd)
		
		EVENT_ID_COMMAND(R.id.btn_select_music_1 , OnBtnSelectMusic1)
		EVENT_ID_COMMAND(R.id.btn_select_lyric_1 , OnBtnSelectLyric1)
		EVENT_ID_COMMAND(R.id.btn_select_output  , OnBtnSelectOutput)
		EVENT_ID_COMMAND(R.id.btn_load_1 , OnBtnLoad1)
		EVENT_ID_COMMAND(R.id.btn_start_making , OnBtnStartMaking)
		EVENT_ID_COMMAND(R.id.btn_restart , OnBtnRestart)
		EVENT_ID_COMMAND(R.id.btn_preview , OnBtnPreview)
		EVENT_ID_COMMAND(R.id.btn_open_output , OnBtnOpenOutput)
		
		EVENT_ID_COMMAND(R.id.btn_sound_open_1 , OnBtnSoundOpen1)
		EVENT_ID_COMMAND(R.id.btn_sound_close_1 , OnBtnSoundClose1)
		EVENT_NAME_HANDLER(R.name.slider_sound_1, EVT_SLIDER_POS, OnSliderPos1)

	EVENT_MAP_BREAK()

private:

	//制作歌词页面 的路径 与选择状态
	PATH_STATE PATH_STATE_1[3] ;
	
	SEdit	*m_EditMusic;
	SEdit	*m_EditLyric;
	SEdit	*m_EditOutPath;

	SStatic *m_txtMusic;
	SStatic *m_txtLyric;
	
	SStatic *P1_Line1 ;		/* 制作页面第一行歌词控件 */
	SStatic *P1_Line2 ;		/* 制作页面第二行歌词控件 */
	SStatic *P1_Line3 ;		/* 制作页面第三行歌词控件 */
	SStatic *emptyTip1;		/* 制作页面空行提示控件 */
	SWindow *m_wndHighlight;

	SStatic* m_txtTime;
	SProgress* m_ProgressTime;

	SButton *m_btnStart;
	SButton *m_btnPreview;
	SButton *m_btnOpenOutput;
	SButton *m_btnLoad;
	SButton *m_btnRestart;


	SHostWnd *m_pMainWnd;		/* 主窗口指针 */
};