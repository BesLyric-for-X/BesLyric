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
* @file       AutoUpdateThread.h
* 
* Describe    启动一个线程，用于检测更新
*/

#pragma once
#include "stdafx.h"
#include "../utility/WinFile.h"
#include "../utility/SSingleton.h"
using namespace SOUI;


class  AutoUpdateThread : public Singleton<AutoUpdateThread>
{
public:
	// 设置检查更新的间隔为10分钟
	AutoUpdateThread(int nDelay = 1000 * 60 * 10):m_nDelay(nDelay),m_handleThread(NULL),m_bLoop(true),m_bKeepUpdate(false){}

	//开始线程
	bool Start();

	//停止线程
	void Stop();
	
	//设置是否持续检测更新
	void SetBKeepUpdate(BOOL bValue)
	{
		m_bKeepUpdate = (bValue ? true : false);
		if(m_bKeepUpdate)
			//设置 m_EventStopWaiting 为有信号，以结束 ThreadProc 中的循环的等待
			SetEvent(m_EventStopWaiting);
	}
	
	//比较2个字符串版本号的大小，
	static int VersionCompare(const SStringW v1, const SStringW v2);
private:
	
	//线程执行地址
	static DWORD WINAPI ThreadProc(LPVOID pParam);

	//自动更新执行函数
	bool AutoUpdate(const SStringW& strVersionNew);

	//检测是否有更新
	bool IfUpdateAvailable(SStringW& versionNew);


private:

	HANDLE		m_handleThread;			/* 当前线程句柄 */
	HANDLE		m_EventStopWaiting;		/* 停止等待时间 */
	bool		m_bLoop;				/* 线程循环标记 */
	bool		m_bKeepUpdate;			/* 保持更新标志 */
	int			m_nDelay;				/* 检查更新的间隔，单位ms */
};

