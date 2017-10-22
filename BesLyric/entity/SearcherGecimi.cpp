#include "stdafx.h"
#include "SearcherGecimi.h"

bool SearcherGecimi::SearchLyric(SStringW strSong, SStringW strArtist, vector<LyricInfo>& vecLyricInfo)
{
	wstring strSearchUrl = L"http://gecimi.com/api/lyric";

	strSearchUrl += ( strSong.GetLength() == 0 ? L"" : (wstring(L"/") +  strSong.GetBuffer(0)));
	strSearchUrl += ( strArtist.GetLength() == 0 ? L"" : (wstring(L"/") +  strArtist.GetBuffer(0)));

	wstring strSaveBuffer;
	if(!CDownloader::DownloadString( strSearchUrl, strSaveBuffer))
	{
		m_strLastResult = L"网络连接失败，无法获取数据";
		return false;
	}

	vector<string>	vecLyricLink;

	//从 歌词迷获取的json数据获取歌词链接
	if(!GetLyricLinkFromJson(strSaveBuffer, vecLyricLink))
	{
		m_strLastResult = L"歌词迷数据格式异常，无法解析数据";
		return false;
	}

	//从 去链接中下载歌词文件，并获得具体信息置于 vecLyricInfo 中
	vecLyricInfo.clear();
	GetLyricInfoFromLinks(vecLyricLink, vecLyricInfo);

	if(vecLyricLink.size() > vecLyricInfo.size())
		m_strLastResult.Format(L"共查询到%d条数据，成功下载%d条数据",vecLyricLink.size(),vecLyricInfo.size());
	else
		m_strLastResult.Format(L"成功下载询到的%d条数据",vecLyricInfo.size());

	return true;
}

//从 去链接中下载歌词文件，并获得具体信息置于 vecLyricInfo 中
void SearcherGecimi::GetLyricInfoFromLinks(vector<string>& vecLyricLink, vector<LyricInfo>& vecLyricInfo)
{
	for(auto iter = vecLyricLink.begin(); iter != vecLyricLink.end(); iter++)
	{
		SStringW strlyricLink = S_CA2W( SStringA( iter->c_str()));
		wstring strLyricBuffer;
		if(!CDownloader::DownloadString( wstring(strlyricLink.GetBuffer(1)),strLyricBuffer))
			continue;
		
		//从歌词文本缓存获取 歌词 信息
		LyricInfo lyricInfo;
		GetOneLyricInfoFromLyricBuffer(strLyricBuffer, lyricInfo);
		
		vecLyricInfo.push_back(lyricInfo);
	}
}

//从歌词文本缓存获取 歌词 信息
void SearcherGecimi::GetOneLyricInfoFromLyricBuffer(wstring strLyricBuffer,LyricInfo& lyricInfo)
{
	LrcProcessor processor( SStringW(strLyricBuffer.c_str()));
	vector<TimeLineInfo> vecTimeLineInfo = processor.GetNeteaseLrc();

	lyricInfo.strSong = processor.m_strTitle.GetBuffer(1);
	lyricInfo.strArtist = processor.m_strArtist.GetBuffer(1);
	lyricInfo.strLyricFrom = L"歌词迷";
	
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

//从 歌词迷获取的json数据获取歌词链接
bool SearcherGecimi::GetLyricLinkFromJson(wstring strLyricJson, vector<string>& vecLyricLink)
{
	m_vecLyricLink.clear();
	vecLyricLink.clear();

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

	//从 歌词迷获取的json数据获取歌词链接置于 m_vecLyricLink中
	if(!AnalyseJson(root)) 
		return false;

	vecLyricLink = m_vecLyricLink;

	return true;
}


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
bool SearcherGecimi::AnalyseJson(Json::Value & value)
{
	if(value.type() == Json::objectValue) //{}
	{
		Json::Value::Members members(value.getMemberNames());

		for (Json::Value::Members::iterator it = members.begin(); it != members.end();++it) 
		{
			//节点里有 code、count、result 等属性成员
			//这里获取result数组
			const JSONCPP_STRING name = *it;
			if(name == "result")
			{
				Json::Value resultArray = value[name];
				if(resultArray.type() == Json::arrayValue) //[]
				{
					Json::ArrayIndex size = resultArray.size();
					for (Json::ArrayIndex index = 0; index < size; ++index) 
					{
						Json::Value oneLyric = resultArray[index];

						if(oneLyric.type() == Json::objectValue)//{}
						{
							//每一个歌词信息里有 aid、song、lrc 等属性成员
							//这里获取lrc  链接
							Json::Value::Members members(oneLyric.getMemberNames());

							for (Json::Value::Members::iterator it = members.begin(); it != members.end();++it) 
							{
								const JSONCPP_STRING member = *it;
								if(member == "lrc")
								{
									if(oneLyric[member].type() == Json::stringValue)
									{
										m_vecLyricLink.push_back(oneLyric[member].asString()); //获得歌词链接
									}
									else
										return false;
								}
							}
						}//if(oneLyric.type() == Json::objectValue)
						else
							return false;
					}
				}//if(resultArray.type() == Json::arrayValue)
				else
					return false;
			}//if(name == "result")
		}
	}
	else
		return false;

	return true;
}

