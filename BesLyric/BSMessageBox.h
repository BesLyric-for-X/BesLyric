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
* @file       BSMessageBox.h
* 
* Describe    BSMessageBox 类，实现SOUI的SMessageBoxImpl接口，实现自定义图标的显示
*/

#pragma once
#include "stdafx.h"


class BSMessageBox: public SMessageBoxImpl
{
public:
	//重载设置图标的函数
	BOOL OnSetIcon( UINT uType )
	{
		SIconWnd *pIcon=(SIconWnd *)FindChildByName(NAME_MSGBOX_ICON);
        if(!pIcon) return FALSE;
		pIcon->SetVisible(FALSE,TRUE);

		SWindow *iconInfo = FindChildByID(R.id.img_icon_information);
		SWindow *iconQues = FindChildByID(R.id.img_icon_question);
		SWindow *iconExcl = FindChildByID(R.id.img_icon_exclamation);

        switch(uType&0xF0)
        {
        case MB_ICONINFORMATION:
			
			if(iconInfo && !iconInfo->IsVisible())
				iconInfo->SetVisible(TRUE,TRUE);
			if(iconQues && iconQues->IsVisible())
				iconQues->SetVisible(FALSE,TRUE);
			if(iconExcl && iconExcl->IsVisible())
				iconExcl->SetVisible(FALSE,TRUE);

            break;

        case MB_ICONQUESTION:

			if(iconInfo && iconInfo->IsVisible())
				iconInfo->SetVisible(FALSE,TRUE);
			if(iconQues && !iconQues->IsVisible())
				iconQues->SetVisible(TRUE,TRUE);
			if(iconExcl && iconExcl->IsVisible())
				iconExcl->SetVisible(FALSE,TRUE);
            break;

		case MB_ICONWARNING: //MB_ICONEXCLAMATION
			
			if(iconInfo && iconInfo->IsVisible())
				iconInfo->SetVisible(FALSE,TRUE);
			if(iconQues && iconQues->IsVisible())
				iconQues->SetVisible(FALSE,TRUE);
			if(iconExcl && !iconExcl->IsVisible())
				iconExcl->SetVisible(TRUE,TRUE);

            break;

        case MB_ICONHAND:
        default:
            
			if(iconInfo && iconInfo->IsVisible())
				iconInfo->SetVisible(FALSE,TRUE);
			if(iconQues && iconQues->IsVisible())
				iconQues->SetVisible(FALSE,TRUE);
			if(iconExcl && iconExcl->IsVisible())
				iconExcl->SetVisible(FALSE,TRUE);

            break;
        }
        return TRUE;
	}
};


//发送自定义图标的消息
int _MessageBox(HWND hwnd,LPCTSTR content, LPCTSTR tiltle, UINT uType);