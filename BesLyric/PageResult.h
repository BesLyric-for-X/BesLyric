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
* @file       PageResult.h
* 
* Describe    CPageResult类，查看歌词结果，预览歌词页面
*/

#pragma once
#include "stdafx.h"
#include "MainDlg.h"
#include "PageMaking.h"

class CMainDlg;							//嵌套定义，先声明
class CPageResult
{
	friend class CMainDlg;	
	friend class CPageMaking;

public:
	CPageResult();
	~CPageResult(){};

	void Init(SHostWnd *pMainWnd);		//初始化设置页面
	CMainDlg* M();					//获得主窗口类实例对象
	
	//各个页面按钮的响应函数

	void OnBtnSelectMusic2();		//两个路径的选择
	void OnBtnSelectLyric2();	
	void OnBtnLoad2();
	void OnBtnEndPreview();
	void OnBtnStartPlaying();		//开始播放歌词
	void OnBtnManualAdjust();		//手动打开文件调整时间
	
	void OnBtnSoundOpen2();			//打开声音
	void OnBtnSoundClose2();		//静音

	void OnSliderPos2(EventArgs *pEvt);//处理声音slider 位置的变化

protected:
	//第二个页面(滚动预览)：回到“加载按钮”按下后的状态
	void backToInit_2();	

	//在滚动预览的页面，执行歌词显示 m_nCurLine
	void scrollToLyricCurLine();

private:
	
	//判断第二个页面(滚动预览) 的 两个路径是否都选择完毕 */
	bool isPathReady_2();

	/* 获得当前未初始化的第一个路径 在PATH_STATE_2 中对应的下标;都初始化了则返回-1 */
	int getPathNotReady_2();
	
	//线程执行地址(格式转换线程)
	static DWORD WINAPI ThreadConvertProc(LPVOID pParam);

	//消息
protected:
	EVENT_MAP_BEGIN()
		EVENT_CHECK_SENDER_ROOT(m_pMainWnd)
		
		EVENT_ID_COMMAND(R.id.btn_select_music_2 , OnBtnSelectMusic2)
		EVENT_ID_COMMAND(R.id.btn_select_lyric_2 , OnBtnSelectLyric2)
		EVENT_ID_COMMAND(R.id.btn_load_2 , OnBtnLoad2)
		EVENT_ID_COMMAND(R.id.btn_end_preview , OnBtnEndPreview)
		EVENT_ID_COMMAND(R.id.btn_start_playing , OnBtnStartPlaying)
		EVENT_ID_COMMAND(R.id.btn_manual_adjust , OnBtnManualAdjust)
		
		EVENT_ID_COMMAND(R.id.btn_sound_open_2 , OnBtnSoundOpen2)
		EVENT_ID_COMMAND(R.id.btn_sound_close_2 , OnBtnSoundClose2)
		EVENT_NAME_HANDLER(R.name.slider_sound_2, EVT_SLIDER_POS, OnSliderPos2)

	EVENT_MAP_BREAK()

	
private:
	
	//滚动预览页面 的路径 与选择状态
	PATH_STATE PATH_STATE_2[2];
	
	SEdit	*m_EditMusic;
	SEdit	*m_EditLyric;
	
	SStatic *m_txtMusic;
	SStatic *m_txtLyric;
	
	SStatic *P2_Line1 ;		/* 预览页面第一行歌词控件 */
	SStatic *P2_Line2 ;		/* 预览页面第二行歌词控件 */
	SStatic *P2_Line3 ;		/* 预览页面第三行歌词控件 */
	SWindow *m_wndHighlight;
	
	SStatic *emptyTip2;		/* 预览页面空行提示控件 */

	SStatic* m_txtTime;
	SProgress* m_ProgressTime;
	
	SButton *m_btnStart;
	SButton *m_btnAdjust;
	SButton *m_btnEndPreview;
	SButton *m_btnLoad;

	SHostWnd *m_pMainWnd;		/* 主窗口指针 */
	
};