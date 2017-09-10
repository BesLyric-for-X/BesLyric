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
* @file       ISearcher.h
* 
* @Describe   定义搜索歌词需要的抽象接口； 
*/

#pragma once
#include "stdafx.h"
#include <vector>
using namespace std;
#include <unknown/obj-ref-i.h>

class ISearcher: public IObjRef
{
public:
	//搜索歌词
    virtual bool SearchLyric(SStringW strSong, SStringW strArtist, vector<LyricInfo>& vecLyricInfo) = 0;
    
	virtual void GetLastResult(SStringW& strResult)
	{
		strResult = m_strLastResult;
	}

protected:
	SStringW m_strLastResult;
};
