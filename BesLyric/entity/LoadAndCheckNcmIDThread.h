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
* @file       LoadAndCheckNcmIDThread.h
* 
* Describe    启动一个线程，用于载入ncm ID 并联网检测 ID的合法性 (之所以使用线程，是因为联网检测可能比较耗时，在启动时检测会导致界面载入延后)
*/

#pragma once
#include "stdafx.h"
#include "../utility/SSingleton.h"


class  CLoadAndCheckNcmIDThread : public Singleton<CLoadAndCheckNcmIDThread>
{
public:
	// 检查程序的完整性
	CLoadAndCheckNcmIDThread():m_handleThread(NULL){}

	//开始线程
	bool Start();

	//结束线程
	void Stop();
	
private:

	// 线程执行地址
	static DWORD WINAPI ProcLoadAndCheckNcmID(LPVOID pParam);

private:
	HANDLE m_handleThread;
};

