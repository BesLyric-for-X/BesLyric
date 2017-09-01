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
		for(auto iter = allLines.begin();iter!=allLines.end();  iter++)
		{
			TimeLineInfo newInfo(iter->GetBuffer(1));
			lines.push_back(newInfo);
		}
	}

	return lines;
}