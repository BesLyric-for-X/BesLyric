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
* @file       LyricSearcherFactory.h
* 
* @Describe   工厂类，产生不同的 歌词搜索器 用于搜索歌词； 
*/

#pragma once
#include "stdafx.h"
#include "ISearcher.h"
#include "SearcherGecimi.h"
#include "SearcherNetEaseCloud.h"

enum SEARCH_FROM{
	SF_NETEASE = 0,
	SF_GECIMI = 1,
	UNDEFINED		//将 UNDEFINED 置于最后，PageSearchLyric.cpp 中会遍历UNDEFINED 前的所有搜索来源
};


class CLyricSearcherFactory
{
public:
	static void CreateSearcher(SEARCH_FROM search_from,OUT CAutoRefPtr<ISearcher>& p)
	{
		_CreateSearcher(search_from, (IObjRef**)&p);
	}

private:
    static void _CreateSearcher(SEARCH_FROM search_from, IObjRef** ref)
	{
		switch(search_from)
		{
		case SF_GECIMI:
			*ref = new SearcherGecimi();
			break;
		case SF_NETEASE:
			*ref = new SearcherNetEaseCloud();
			break;
		default:
			SASSERT(false && "未知的歌词搜索类型");
			break;
		}

	}
    
};
