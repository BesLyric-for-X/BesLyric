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
* @file       PageSearchLyric.h
* 
* Describe    CPageSearchLyric类，搜索原歌词，lrc 歌词
*/

#pragma once
#include "stdafx.h"
#include "MainDlg.h"

class CMainDlg;							//嵌套定义，先声明
class CPageSearchLyric
{
	friend class CMainDlg;	

public:
	CPageSearchLyric();
	~CPageSearchLyric(){};

	void Init(SHostWnd *pMainWnd);		//初始化设置页面
	CMainDlg* M();					//获得主窗口类实例对象
	
	//各个页面按钮的响应函数
	void OnBtnSearchBaidu();

	//消息
protected:
	EVENT_MAP_BEGIN()
		EVENT_CHECK_SENDER_ROOT(m_pMainWnd)
		
		EVENT_ID_COMMAND(R.id.btn_search_lyric_using_baidu , OnBtnSearchBaidu)

	EVENT_MAP_BREAK()

	
private:
	SEdit	*m_editSearchLyricName;
	SEdit	*m_editSearchLyricArtist;

	SButton *m_btnSearchHere;
	SButton *m_btnSearchBaidu;

	SWindow *m_wndSearchLyricTip;
	SStatic *m_txtSearchNameTip;
	SStatic *m_txtSearchArtistTip;
	SWindow *m_wndSearchArtistTip;
	SStatic *m_txtSearchResultTip;

	SEdit	*m_editOriginLyricPath;
	SEdit	*m_editLrcLyricPath;
	
	SButton *m_btnSelectOriginLyricPath;
	SButton *m_btnSelectLrcLyricPath;
	
	SEdit	*m_editOriginLyricName;
	SEdit	*m_editOriginLyricArtist;
	SEdit	*m_editLrcLyricName;
	SEdit	*m_editLrcLyricArtist;
	
	SEdit	*m_editOriginLyricContent;
	SEdit	*m_editLrcLyricContent;
	
	SHostWnd *m_pMainWnd;		/* 主窗口指针 */
};