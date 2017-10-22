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
	typedef struct _SONG_INFO{
		int nID;
		string strArtists;
		string strSong;
	} SONGINFO;

public:
	//搜索歌词
    virtual bool SearchLyric(SStringW strSong, SStringW strArtist, vector<LyricInfo>& vecLyricInfo)
	{
		CHttpRequest httpRequest;
		string astrSong = S_CW2A(strSong).GetBuffer(1);
		string astrArtist = S_CW2A(strArtist).GetBuffer(1);


		string strSongUrl = CUrlEncodinig().UrlUTF8(S_CW2A(strSong).GetBuffer(1));
		string strArtistUrl = CUrlEncodinig().UrlUTF8(S_CW2A(strArtist).GetBuffer(1));

		wstring strRes;
		if(!httpRequest.Post( "music.163.com/api/search/get/web", 
									"csrf_token=&s="+ strSongUrl +"&type=1&offset=0&total=True&limit=8",
									strRes))
		{
			m_strLastResult = L"网络连接失败，无法获取歌词索引数据";
			return false;
		}
		
		strRes = strRes.substr( strRes.find_first_of('{'), strRes.find_last_of('}') - strRes.find_first_of('{')+1);

		//获取id列表
		vector< SONGINFO > vecSongList;
		if(!GetSongListFromJson(strRes, vecSongList))
		{
			m_strLastResult = L"网易云歌词数据格式异常，无法解析数据";
			return false;
		}

		for(auto iter = vecSongList.begin(); iter != vecSongList.end(); iter++)
		{
			if( string::npos == iter->strSong.find(astrSong))//歌名不包含于结果歌词名则跳过
				continue;

			wstring strLyricJson;
			char szID[MAX_BUFFER_SIZE];

			//获取id对应的歌词
			if(!httpRequest.Get("music.163.com/api/song/lyric",string("os=osx&id=") + itoa( iter->nID,szID, 10) + string("&lv=-1&kv=-1&tv=-1"), strLyricJson))
			{
				m_strLastResult = L"网络连接失败，无法获取歌词内容数据";
				return false;
			}

			//从json 数据获取一个歌词添加到
			LyricInfo oneLyricInfo;
			strLyricJson = strLyricJson.substr( strLyricJson.find_first_of('{'), strLyricJson.find_last_of('}') - strLyricJson.find_first_of('{')+1);
			
			if(!GetOneLyricFromJson(strLyricJson, oneLyricInfo))
				break;
			
			oneLyricInfo.strSong = S_CA2W( SStringA(iter->strSong.c_str())).GetBuffer(1);
			oneLyricInfo.strArtist = S_CA2W( SStringA(iter->strArtists.c_str())).GetBuffer(1);
			vecLyricInfo.push_back(oneLyricInfo);
		}

		return true;
	}
    
private:

	//获得结果中的歌曲id列表
	bool GetSongListFromJson(wstring strJsonRes, vector< SONGINFO >& vecSongList)
	{
		vecSongList.clear();

		string strJson = S_CW2A(SStringW(strJsonRes.c_str())).GetBuffer(1);

		JSONCPP_STRING input = strJson;
		Json::Features features;
		bool parseOnly = true;
		Json::Value root;

		Json::Reader reader(features);
		bool parsingSuccessful = reader.parse(input.data(), input.data() + input.size(), root); //解析数据
		if (!parsingSuccessful) {
			return false;
		}

		//从 获取的json数据获取歌词链接置于 m_vecLyricLink中
		if(!AnalyseIDJson(root, vecSongList)) 
			return false;

		return true;
	}


	bool GetOneLyricFromJson(wstring strLyricJson,  LyricInfo& oneLyricInfo)
	{
		string strJson = S_CW2A(SStringW(strLyricJson.c_str())).GetBuffer(1);

		JSONCPP_STRING input = strJson;
		Json::Features features;
		bool parseOnly = true;
		Json::Value root;

		Json::Reader reader(features);
		bool parsingSuccessful = reader.parse(input.data(), input.data() + input.size(), root); //解析数据
		if (!parsingSuccessful) {
			return false;
		}

		string strLyric;
		//从 获取的json数据获取歌词链接置于 m_vecLyricLink中
		if(!AnalyseLyricJson(root, strLyric)) 
			return false;

		GetOneLyricInfoFromLyricBuffer(S_CA2W(SStringA(strLyric.c_str())).GetBuffer(1), oneLyricInfo);
		return true;
	}

	//从歌词文本缓存获取 歌词 信息
	void GetOneLyricInfoFromLyricBuffer(wstring strLyricBuffer,LyricInfo& lyricInfo)
	{
		LrcProcessor processor( SStringW(strLyricBuffer.c_str()));
		vector<TimeLineInfo> vecTimeLineInfo = processor.GetNeteaseLrc();

		lyricInfo.strSong = processor.m_strTitle.GetBuffer(1);
		lyricInfo.strArtist = processor.m_strArtist.GetBuffer(1);
		lyricInfo.strLyricFrom = L"网易云音乐";
	
		wstring strPlaneText = L"";
		wstring strLabelText = L"";;

		WCHAR szTimeBuf[MAX_BUFFER_SIZE/2];
		for(auto iter = vecTimeLineInfo.begin(); iter != vecTimeLineInfo.end(); iter++)
		{
			strPlaneText += iter->m_strLine;
			strLabelText += iter->m_strTimeLine;
		}
	
		lyricInfo.strPlaneText = strPlaneText;
		lyricInfo.strLabelText = strLabelText;
	}



	
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
	bool AnalyseLyricJson(Json::Value & value, string& strLyric)
	{
		if(value.type() == Json::objectValue) //{}
		{
			Json::Value::Members members(value.getMemberNames());

			for (Json::Value::Members::iterator it = members.begin(); it != members.end();++it) 
			{
				//节点里有 code、klyric、lrc 等属性成员
				//这里获取lrc 
				const JSONCPP_STRING name = *it;
				if(name == "lrc")
				{
					Json::Value lrc = value[name]; //获得lrc 节点
					if(value.type() == Json::objectValue) //{}
					{
						Json::Value::Members lrcMembers(lrc.getMemberNames());
						for (Json::Value::Members::iterator itLM = lrcMembers.begin(); itLM != lrcMembers.end();++itLM) 
						{
							//"lyric" 和 version" 2个成员，直接获取lyric 内容
							const JSONCPP_STRING name = *itLM;
							if(name == "lyric")
							{
								if(lrc[name].type() == Json::stringValue)  //这里lyric 字符串数据
								{
									strLyric = lrc[name].asString();
									return true;
								}
								else
									return false;
							}
						}
					}
					else 
						return false;
				}//if(name == "lrc")
			}
		}

		return false;
	}

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
	bool AnalyseIDJson(Json::Value & value, vector< SONGINFO >& vecSongList)
	{
		if(value.type() == Json::objectValue) //{}
		{
			Json::Value::Members members(value.getMemberNames());

			for (Json::Value::Members::iterator it = members.begin(); it != members.end();++it) 
			{
				//节点里有 code、result 等属性成员
				//这里获取result数组
				const JSONCPP_STRING name = *it;
				if(name == "result")
				{
					Json::Value result = value[name]; //获得result 节点

					Json::Value::Members members(result.getMemberNames());
					for(Json::Value::Members::iterator itRes = members.begin(); itRes != members.end();++itRes)
					{
						//result 下有2个成员：songCount、songs
						//这里获取 songs 数组
						const JSONCPP_STRING name = *itRes;
						if(name == "songs")
						{
							Json::Value songsArray = result[name];  //获得songs节点
							if(songsArray.type() == Json::arrayValue) //[]
							{
								Json::ArrayIndex size = songsArray.size();
								for (Json::ArrayIndex index = 0; index < size; ++index) 
								{
									Json::Value oneSong = songsArray[index];

									if(oneSong.type() == Json::objectValue)//{}
									{
										//每一个歌信息里有 id、name、artist 等属性成员
										Json::Value::Members members(oneSong.getMemberNames());

										int nID;
										string strArtists, strSong;
										for (Json::Value::Members::iterator it = members.begin(); it != members.end();++it) 
										{
											const JSONCPP_STRING member = *it;
											if(member == "id")
											{
												if(oneSong[member].type() == Json::intValue)  //这里id是整型数据
												{
													nID = oneSong[member].asInt(); //获得歌曲id
												}
												else
													return false;
											}
											else if(member == "name")
											{
												if(oneSong[member].type() == Json::stringValue)  //这里name是字符串
												{
													strSong = oneSong[member].asString(); //获得歌曲id
												}
												else
													return false;
											}
											else if(member =="artists")
											{
												Json::Value artistArray = oneSong[member];  //获得artists节点
												if(artistArray.type() == Json::arrayValue) //[]
												{
													int count = 0; //记录歌手个数
													Json::ArrayIndex size = artistArray.size();
													for (Json::ArrayIndex index = 0; index < size; ++index) //遍历所有artist
													{
														Json::Value oneArtist = artistArray[index];

														if(oneArtist.type() == Json::objectValue)//{}
														{
															//每一个艺术家里有 img1v1Url、name 等属性成员
															//这里获取 name, 追加在一起到  strArtists 字符串中
															Json::Value::Members artistMembers(oneArtist.getMemberNames());
															for (Json::Value::Members::iterator itA = members.begin(); itA != members.end();++itA) 
															{
																const JSONCPP_STRING member = *itA;
																if(member == "name")
																{
																	if(count++ > 0)
																	{
																		strArtists += "、";
																	}
																	strArtists += oneArtist[member].asString();
																	break;
																}
															}
														}
														else
															return false;
													}
												}else 
													return false;
											}//oneSong 的其他成员不考虑
										}
										SONGINFO songInfo;
										songInfo.nID = nID;
										songInfo.strArtists = strArtists;
										songInfo.strSong = strSong;
										vecSongList.push_back(songInfo);  //将从一首歌收集的信息储存

									}//if(oneLyric.type() == Json::objectValue)
									else
										return false;
								}
							}//if(resultArray.type() == Json::arrayValue)
							else
								return false;
						}//if(name == "songs")
					}
				}//if(name == "result")
			}
		}
		else
			return false;

		return true;
	}

};




