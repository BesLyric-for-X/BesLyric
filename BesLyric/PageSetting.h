/*
	BesLyric  一款 操作简单、功能实用的 专门用于制作网易云音乐滚动歌词的 歌词制作软件。
    Copyright (C) 2017  BensonLaur

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
* @file       PageSetting.h
* 
* Describe    CSettingPage类，处理设置页面的业务逻辑
*/

#pragma once
#include "stdafx.h"

class CSettingPage
{
public:
	CSettingPage(){LoadSetting();};
	~CSettingPage(){SaveSetting();};

	void Init(SHostWnd *pMainWnd);		//初始化设置页面
	void SaveSetting();					//保存设置
	void LoadSetting();					//加载设置
	void LoadShiftTime();				//从文件获取获取时间，显示到edit_time_shift 编辑框中
	void SaveShiftTime();				//将设置的时间保存到文件中
	
	void OnTimeShiftEditNotify(EventArgs *pEvt);	//偏移时间编辑框通知响应
	void OnBtnModifyShiftTime();					//修改偏移时间
	void OnBtnSelectDefaultMusicPath();				//选择默认音乐路径
	void OnBtnSelectDefaultLyricPath();				//选择默认歌词路径
	void OnBtnSelectDefaultOutputPath();				//选择默认LRC歌词输出路径

	void OnCheckAutoUpdateChanged();				//自动升级check 改变状态时

	//消息
protected:
	EVENT_MAP_BEGIN()
		EVENT_CHECK_SENDER_ROOT(m_pMainWnd)
		EVENT_NAME_COMMAND(L"btn_modify_setting_shift_time", OnBtnModifyShiftTime)
		
		EVENT_NAME_COMMAND(L"btn_select_default_music_path", OnBtnSelectDefaultMusicPath)
		EVENT_NAME_COMMAND(L"btn_select_default_lyric_path", OnBtnSelectDefaultLyricPath)
		EVENT_NAME_COMMAND(L"btn_select_default_output_path", OnBtnSelectDefaultOutputPath)

		EVENT_NAME_COMMAND(R.name.check_auto_update, OnCheckAutoUpdateChanged)
		
        EVENT_NAME_HANDLER(R.name.edit_time_shift, EVT_RE_NOTIFY, OnTimeShiftEditNotify);
		//EVENT_NAME_HANDLER(R.name.edit_time_shift, EventRENotify::EventID, OnTimeShiftEditNotify)
	EVENT_MAP_BREAK()


public:
	int m_nTimeShift;				/* 时间轴偏移量，单位为毫秒，为正数时表示按键时间向前偏移 */
	string m_default_music_path;	/* 默认音乐路径 */
	string m_default_lyric_path;	/* 默认歌词路径 */
	string m_default_output_path;	/* 默认输出路径 */
	BOOL   m_check_auto_update;		/* 是否自动升级 */

private:
	SButton* btn_modify_shift_time;		/* 修改时间轴偏移时间按钮 */
	SEdit* edit_time_shift;				/* 偏移时间编辑框 */
	
	SStatic* text_default_music_path;	/* 默认音乐路径 */
	SStatic* text_default_lyric_path;	/* 默认歌词路径 */
	SStatic* text_default_output_path;	/* 默认输出路径 */

	SCheckBox* check_auto_update;		/* 是否自动升级 */

	SHostWnd *m_pMainWnd;		/* 主窗口指针 */
	
};