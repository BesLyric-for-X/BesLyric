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
* @file       SearcherGecimi.h
* 
* @Describe   定义搜索歌词需要的抽象接口； 
*/

#pragma once
#include "stdafx.h"
#include "ISearcher.h"
#include "LrcHandler.h"
#include "..\utility\Downloader.h"
#include "..\lib\json\json.h"
#include <unknown/obj-ref-impl.hpp>

class SearcherGecimi: public TObjRefImpl<ISearcher>
{
public:
	//搜索歌词
    virtual bool SearchLyric(SStringW strSong, SStringW strArtist, vector<LyricInfo>& vecLyricInfo);
    
private:
	//从 去链接中下载歌词文件，并获得具体信息置于 vecLyricInfo 中
	void GetLyricInfoFromLinks(vector<string>& vecLyricLink, vector<LyricInfo>& vecLyricInfo);

	//从歌词文本缓存获取 歌词 信息
	void GetOneLyricInfoFromLyricBuffer(wstring strLyricBuffer,LyricInfo& lyricInfo);

	//从 歌词迷获取的json数据获取歌词链接
	bool GetLyricLinkFromJson(wstring strLyricJson, vector<string>& vecLyricLink);


	/*
		@brief	从 歌词迷获取的json数据获取歌词链接置于 m_vecLyricLink中
		@note
		歌词迷获取的歌词的结构为：
		{
		   "code" : 0,
		   "count" : 6,
		   "result" : [
			  {
				 "aid" : 3179315,
				 "artist_id" : 11807,
				 "lrc" : "http://s.gecimi.com/lrc/388/38847/3884774.lrc",
				 "sid" : 3884774,
				 "song" : "我们的歌"
			  },
			  {
			  ...
			  }
			]
		}
	*/
	bool AnalyseJson(Json::Value & value);

private:
	vector<string>	m_vecLyricLink;		//存放获取到的歌词链接
};
