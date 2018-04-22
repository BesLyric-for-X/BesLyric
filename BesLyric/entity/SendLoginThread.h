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
* Describe    启动一个线程，用于发送登录信息
*/

#pragma once
#include "stdafx.h"
#include "../utility/WinFile.h"
#include "../utility/SSingleton.h"
using namespace SOUI;


class  SendLoginThread : public Singleton<SendLoginThread>
{
public:
	SendLoginThread():m_handleThread(NULL){}

	//开始线程
	bool Start(BOOL bAnonymity);

private:
	
	//线程执行地址
	static DWORD WINAPI ThreadProc(LPVOID pParam);

	//发送登录信息（ip地址）
	void SendLoginInfo(BOOL bAnonymity);

	//俘获满足需求的IP字符串
	inline bool CatchIPStr(const wstring &line, OUT wstring& ip);
	

private:

	HANDLE		m_handleThread;			/* 当前线程句柄 */
	BOOL		m_bAnonymity;			/* 线程循环标记 */
};

