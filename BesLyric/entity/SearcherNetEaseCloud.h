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
* @file       SearcherNetEaseCloud.h
* 
* @Describe   定义网易云音乐搜索歌词类，实现搜索歌词接口； 
*/

#pragma once
#include "stdafx.h"
#include "ISearcher.h"
#include "LrcHandler.h"
#include "..\utility\Downloader.h"
#include "..\utility\HttpRequest.h"
#include "..\utility\UrlEncoding.h"
#include "..\lib\json\json.h"
#include <unknown/obj-ref-impl.hpp>

class SearcherNetEaseCloud: public TObjRefImpl<ISearcher>
{
public:
	//搜索歌词
    virtual bool SearchLyric(SStringW strSong, SStringW strArtist, vector<LyricInfo>& vecLyricInfo);
    
	//获得结果中的歌曲信息列表
	static bool GetSongListFromJson(wstring strJsonRes, vector< SONGINFO >& vecSongList);
private:

	//从单个歌词json数据获得歌词
	bool GetOneLyricFromJson(wstring strLyricJson,  LyricInfo& oneLyricInfo);

	//从歌词文本缓存获取 歌词 信息
	void GetOneLyricInfoFromLyricBuffer(wstring strLyricBuffer,LyricInfo& lyricInfo);
	
	/*
		@brief	从 网易云获取的json数据获取歌词 置于 m_vecIdList 中
		@note
		获取的信息的结构为：
		{
		     "code" : 200,
			   "klyric" : {
				  "lyric" : "..."
				  "version" : 6
			   },
			   "lrc" : {
				  "lyric" : "[00:00.00] 作曲 : 王力宏\n[00:01.00] 作词 : 王力宏/陳信延\n[00:15.750]已经听了一百遍 怎么听都不会倦\n[00:23.140]从白天唱到黑夜 你一直在身边（一直在身边）\n[00:29.450]如果世界太危险 只有音乐最安全  带着我进梦里面 让歌词都实现\n[00:42.519]无论是开心还是难过我的爱一直不变\n[00:46.389]（不必担心时间流逝带走一切）\n[00:50.789]无论是HIP-HOP还是摇滚我的爱一直不变\n[00:54.389]（所有美好回忆记录在里面）\n[00:56.149]这种Forever Love 那么深\n[00:58.649]我们的歌 那么真\n[01:00.279]无国界 跨时代\n[01:02.179]再不会叫我KISS GOODBYE\n[01:03.669]要每一句能够动人心弦  YE~~\n[01:10.570]情人总分分合合\n[01:13.509]可是我们却越爱越深\n[01:17.389]认识你让我的幸福如此  悦耳\n[01:24.240]能不能不要切歌  继续唱我们的歌\n[01:31.139]让感动一辈子都记得\n[01:37.580]en heng~~~\n[01:40.699]wo~  o~\n[01:48.309]已经听了一百遍 怎么听都不会倦\n[01:55.158]从白天唱到黑夜 你一直在身边（一直在身边）\n[02:02.800]如果世界太危险 只有音乐最安全  带着我进梦里面 让歌词都实现\n[02:15.150]无论是开心还是难过我的爱一直不变\n[02:19.150]（不必担心时间流逝带走一切）\n[02:22.000]无论是HIP-HOP还是摇滚我的爱一直不变\n[02:25.670]（所有美好回忆记录在里面）\n[02:28.720]这种Forever Love那么深 我们的歌那么真\n[02:32.850]无国界 跨时代\n[02:34.750]再不会叫我KISS GOODBYE\n[02:36.228]要每一句能够动人心弦 YE~~\n[02:43.130]情人总分分合合\n[02:46.140]可是我们却越爱越深\n[02:50.300]认识你让我的幸福如此 悦耳\n[02:56.880]能不能不要切歌 继续唱我们的歌\n[03:03.660]让感动一辈子都记得\n[03:08.370]\n[03:18.440]（一辈子都记得）\n[03:36.240]情人总分分合合\n[03:39.280]可是我们却越爱越深\n[03:43.860]认识你让我的幸福如此 悦耳\n[03:51.400]能不能不要切歌 继续唱我们的歌\n[03:57.400]让感动一辈子都记得\n",
				  "version" : 18
			   },
			   "qfy" : false,
			   "sfy" : false,
			   "sgc" : true,
			   "tlyric" : {
				  "lyric" : null,
				  "version" : 0
			   }
		}
	*/
	bool AnalyseLyricJson(Json::Value & value, wstring& strLyric);

	/*
		@brief	从 网易云获取的json数据获取歌词id 置于 m_vecIdList 中
		@note
		获取的信息的结构为：
		{
		   "code" : 200,
		   "result" : {
			  "songCount" : 3,
			  "songs" : [
				 {
					"album" : {...},
					"alias" : [],
					"artists" : [{... ,"name" : "王力宏", ...},{...}]
					...
					"id" : 25642952,
					"mvid" : 5293365,
					"name" : "我们的歌",
					...
				 }
				 ,
				 {...}
				]
			}
		}
	*/
	static bool AnalyseIDJson(Json::Value & value, vector< SONGINFO >& vecSongList);

};




