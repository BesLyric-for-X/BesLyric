/*
*	Copyright (C) 2017  BensonLaur
*	note: Looking up header file for license detail
*/

// LyricPlyer.cpp :  实现  LyricPlayer类 的接口	
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LyricPlayer.h"
#include "../utility/FileHelper.h"
#include "../utility/WinFile.h"
#include <algorithm>
#include <mmsystem.h> 
#pragma comment (lib, "winmm.lib")

//构造函数
LyricPlayer::LyricPlayer():
		 m_nCurLine(0),	
		 m_nTotalLine(0)
{
	m_szMusicPathName[0]= _T('\0');		
	m_szLyricPathName[0]= _T('\0');
}

//设置各个路径
void LyricPlayer::setMusicPath(LPCTSTR pathName, HWND hostWnd)
{
	_tcscpy(m_szMusicPathName,pathName);
	m_musicPlayer.init(m_szMusicPathName,hostWnd);
}
	
void LyricPlayer::setLyricPath(LPCTSTR pathName)
{
	_tcscpy(m_szLyricPathName,pathName);
}

//重置 LyricPlayer的 歌词数据为空
void LyricPlayer::reloadPlayer()
{
	m_vLineInfo.clear();
}

//播放歌曲与滚动歌词 预览开始
bool LyricPlayer::playingStart(SHostWnd *wnd)
{
	//更新基本的行数记录的数据
	m_nTotalLine = this->m_vLineInfo.size();
	m_nCurLine = 1;	//初值设为1，定时器循环的需要

	//启动间隔为1毫秒的Timer来更新页面的显示
	wnd->SetTimer(102,1);

	return playMusic();
}

//预览结束
void LyricPlayer::playingEnd(SHostWnd *wnd)
{
	//关闭Timer
	wnd->KillTimer(102);
	stopMusic();
}

//播放音乐
bool LyricPlayer::playMusic()
{
	if(m_musicPlayer.openTest() == false)
	{
		return false;
	}

	m_musicPlayer.openStart();
	
	return true;
}

//停止音乐
void LyricPlayer::stopMusic()
{
	m_musicPlayer.closeStop();
}

//如果快进或者后退都会导致，当前行发生变化，故需要先更新再取值
void LyricPlayer::updateCurLine()
{
	//根据当前歌曲播放的位置，判断当前在哪一行
	int pos = this->m_musicPlayer.getPosition();
	auto i= m_vLineInfo.begin();
	int j;
	for(j=1; i != m_vLineInfo.end(); i++,j++)
		if( i->m_nmSesonds > pos)//歌词时间 大于当前播放位置时
		{
			//取前一个位置，则为 m_nCurLine 的值
			this->m_nCurLine = j-1; 
			break;
		}
}

//当前是否在第0行的位置
bool LyricPlayer::isCurrentZeroLine()
{
	bool isZero;

	//根据当前歌曲播放的位置，判断当前在哪一行
	int pos = this->m_musicPlayer.getPosition();
	if( pos < m_vLineInfo.begin()->m_nmSesonds )
		isZero = true;
	else
		isZero = false;
	
	return isZero;
}

//从文件获取带时间信息的每行歌词的集合向量
vector<TimeLineInfo> LyricPlayer::getLyricWithLineInfo(File& lyricFile)
{
	vector<TimeLineInfo> lines;

	
	vector<SStringW> allLines;
	if(true == FileOperator::ReadAllLinesW(lyricFile, &allLines))
	{
		LrcProcessor lrcProcessor( allLines);

		if(lrcProcessor.IsLrcFileValid())
		{
			lines = lrcProcessor.GetNeteaseLrc();

			//输出覆盖原来的lrc文件，因为用户可能看到可以预览LRC，就直接将网易云不支持的歌词上传上去
			lrcProcessor.GenerateNeteaseLrcFile(SStringW(lyricFile.m_lpszPathFile));
		}
	}

	//vector<SStringW> allLines;
	//if(true == FileOperator::ReadAllLinesW(lyricFile, &allLines))
	//{
	//	for(auto iter = allLines.begin();iter!=allLines.end();  iter++)
	//	{
	//		TimeLineInfo newInfo(iter->GetBuffer(1));
	//		lines.push_back(newInfo);
	//	}
	//}

	return lines;
}



//使用读取的的文件的所有行初始化Lrc处理器
LrcProcessor::LrcProcessor(vector<SStringW> vecLines, bool bDealOffset)
{
	m_bDealOffset = bDealOffset;
	m_strTitle = L"";
	m_strArtist = L"";
	m_strAlbum = L"";
	m_strEditor = L"";
	m_nOffset = 0;

	m_bIsLrcFileValid = true;

	//遍历处理所有行
	for(auto iter = vecLines.begin(); iter != vecLines.end(); iter++)
	{
		if( false == DealOneLine( *iter ) )
		{
			m_bIsLrcFileValid = false;	
			return;
		}
	}

	//对所有歌词进行排序
	sort(m_vecNeteaseLrc.begin(), m_vecNeteaseLrc.end());

	//去除重复的空行，连续出现多个空行时，保留第一个空行的时间
	bool isLastLineEmpty = false;
	for(auto iter = m_vecNeteaseLrc.begin(); iter != m_vecNeteaseLrc.end(); )
	{
		if(iter->m_strLine.GetLength() == 0)
		{
			if(isLastLineEmpty)
			{
				iter = m_vecNeteaseLrc.erase(iter);
			}
			else
			{
				iter++;
				isLastLineEmpty = true;
			}
		}
		else
		{
			iter++;
			isLastLineEmpty = false;
		}
	}

	//对整体的时间进行偏移
	if(m_bDealOffset)
	{
		for(auto iter = m_vecNeteaseLrc.begin(); iter != m_vecNeteaseLrc.end(); iter++)
		{
			iter->m_nmSesonds = iter->m_nmSesonds + m_nOffset < 0 ? 0 : iter->m_nmSesonds + m_nOffset;
		}
	}
}

//当前歌词文件是否有效
bool LrcProcessor::IsLrcFileValid()
{
	return m_bIsLrcFileValid;
}

//获得网易云音乐支持的格式
vector<TimeLineInfo> LrcProcessor::GetNeteaseLrc()
{
	return m_vecNeteaseLrc;
}

//生成Lrc文件
bool LrcProcessor::GenerateNeteaseLrcFile(SStringW strFilePath)
{
	vector<SStringW> vecLines;

	WCHAR szTimeBuf[MAX_BUFFER_SIZE/2];
	for(auto iter = m_vecNeteaseLrc.begin(); iter != m_vecNeteaseLrc.end(); iter++)
	{
		int ms = iter->m_nmSesonds;

		szTimeBuf[0] = L'\0';
		int minutes = ms/60000;
		ms = ms%60000;
		double seconds = ms*1.0/1000;
		_stprintf(szTimeBuf,_T("[%02d:%06.3lf]"),minutes,seconds);

		SStringW newLine = szTimeBuf;
		newLine.Append( iter->m_strLine );
		newLine.Append(L"\n");

		vecLines.push_back(newLine);
	}

	return FileOperator::WriteToUtf8File( strFilePath.GetBuffer(1), vecLines);
}

//处理1行
bool LrcProcessor::DealOneLine(SStringW strLine)
{
	int nPos1 = -1;
	int nPos2 = -1;
	if( -1 != strLine.Find(L"[ti:"))//标题
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strTitle = strLine.Left(nPos2);
		m_strTitle = m_strTitle.Right(m_strTitle.GetLength()-1 - nPos1);
	}
	else if( -1 != strLine.Find(L"[ar:"))//艺术家
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strArtist = strLine.Left(nPos2);
		m_strArtist = m_strArtist.Right(m_strArtist.GetLength()-1 - nPos1);
	}
	else if( -1 != strLine.Find(L"[al:"))//专辑
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strAlbum = strLine.Left(nPos2);
		m_strAlbum = m_strAlbum.Right(m_strAlbum.GetLength()-1 - nPos1 );
	}
	else if( -1 != strLine.Find(L"[by:"))//歌词制作者
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strEditor = strLine.Left(nPos2);
		m_strEditor = m_strEditor.Right(m_strEditor.GetLength()-1 - nPos1);
	}
	else if( -1 != strLine.Find(L"[offset:"))
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		SStringW m_strOffset = strLine.Left(nPos2);
		m_strOffset = m_strOffset.Right(m_strOffset.GetLength()-1 - nPos1);
		m_nOffset = _wtoi( m_strOffset.GetBuffer(1));
	}
	else //其他情况认为是标记时间的行
	{
		wstring sLine = strLine.GetBuffer(1);
		wstring sTimeLabel = L"";
		wstring sLyric = L"";
		int nPosColon = -1;
		sLyric = sLine.substr( sLine.find_last_of(L']') + 1);
		do{
			nPos1 = sLine.find_first_of(L'[');
			nPosColon =  sLine.find_first_of(L':');
			nPos2 =  sLine.find_first_of(L']');
			if(nPos1 ==0 && nPos1 < nPosColon && nPosColon < nPos2) // strLine头还有时间标签
			{
				sTimeLabel = sLine.substr(nPos1, nPos2+1);

				TimeLineInfo timeInfoLine(SStringW( (sTimeLabel + sLyric).c_str()));

				m_vecNeteaseLrc.push_back(timeInfoLine); //添加一行到网易云歌词

				sLine = sLine.substr(nPos2+1); //去除前面标签
			}
			else
				break;
		}
		while( true);
	}

	return true;
}

//按歌词时间升序比较
bool LrcProcessor::CompareWithIncreaceTime(const TimeLineInfo & L1, const TimeLineInfo & L2)
{
	return L1.m_nmSesonds < L2.m_nmSesonds;
}

//bool m_bDealOffset;						/* 表示是否处理LRC文件中的时间偏移 */
//SStringW	m_strTitle;					/* 歌曲标题 */
//SStringW	m_strArtist;				/* 艺术家 */
//SStringW	m_strAlbum;					/* 专辑 */
//SStringW	m_strEditor;				/* 编辑的人 */
//int			m_nOffset;					/* 时间偏移量，为正数表示整体提前 */

//vector<TimeLineInfo> m_vecNeteaseLrc;	/* 储存用于网易云的歌词信息 */

//bool	m_bIsLrcFileValid;				/* 表示歌词文件是否有效 */
