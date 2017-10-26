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
* @file       GuessLyricInfoThread.h
* 
* Describe    启动一个线程，用于根据歌曲文件以及名称等信息，猜测歌词的歌曲名和歌手信息
*/

#pragma once
#include "stdafx.h"
#include "../utility/WinFile.h"
#include "../utility/SSingleton.h"
using namespace SOUI;


class  CGuessLyricInfoThread : public Singleton<CGuessLyricInfoThread>
{
public:
	// 检查程序的完整性
	CGuessLyricInfoThread():m_handleThreadGuess(NULL),m_bIsGuessing(false){}

	//开始线程
	bool Start(HWND hMainWnd, SStringW& strMusicPath);

private:
	//从mp3文件中获取猜测结果
	bool GetGuessResultFromMp3(SongInfoGuessResult* pGuessRes );

	//从网易云接口分析前个字符串是否为歌手
	bool GuessWhetherFirstArtistFromNetease(SStringW& strFirst, SStringW& strSecond);

	// 线程执行地址
	static DWORD WINAPI ProcGuessLyricInfo(LPVOID pParam);

private:

	HANDLE		m_handleThreadGuess;	/* 当前线程句柄 */	
	HWND		m_hMainWnd;				/* 主窗口句柄 */

	bool m_bIsGuessing;				//正在猜测分析当中

	SStringW	m_strMusicPath;
	SStringW	m_strMusicName;			
	SStringW	m_strMusicArtist;
};

