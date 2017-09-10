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
* @file       LrcHandler.h
* 
* Describe    定义处理LRC 文件的相关类
*/

#pragma once
#include "stdafx.h"
#include "../utility/WinDialog.h"

#include <vector>
using namespace std;

/*
*	@brief 储存处理一行歌词文件； 处理lrc文件（带时间轴的歌词文件）的辅助类
*/
class TimeLineInfo
{
public:
	TimeLineInfo(){}//无参构造函数

	TimeLineInfo(SStringT timeLine);
	
	 bool operator < (const TimeLineInfo &m)const { return m_nmSesonds < m.m_nmSesonds; }

private:
	//从时间标签字符串得到对应的毫秒时间
	int TimeStringToMSecond(LPCTSTR timeStr, int length);

	//返回无符号十进制串对应的数字（十进制串可以是 023、12、04 等形式，数值为0到999）
	int DecStrToDecimal(LPCTSTR timeStr);

public:
	SStringT m_strTimeLine;		/* 直接存储 从文件读取的一整行 */
	SStringT m_strLine;			/* 存储去除时间标记之后的内容 */
	int	m_nmSesonds;			/* 存储时间标记对应的毫秒时间 */
	bool m_bIsEmptyLine;		/* 是否为空行（只有时间标记） */
};


class LrcProcessor{
public:
	//使用读取的的文件的所有行初始化Lrc处理器
	LrcProcessor(vector<SStringW> vecLines, bool bDealOffset = true);

	//使用整个LRC文本数据初始化Lrc处理器
	LrcProcessor(SStringW AllContent, bool bDealOffset = true);

	//当前歌词文件是否有效
	bool IsLrcFileValid();

	//获得网易云音乐支持的格式
	vector<TimeLineInfo> GetNeteaseLrc();

	//生成Lrc文件
	bool GenerateNeteaseLrcFile(SStringW strFilePath);

private:
	//处理 vector 行数据
	void ProcessData(const vector<SStringW>& vecLines, bool bDealOffset = true);

	//处理1行
	bool DealOneLine(const SStringW& strLine);

	//按歌词时间升序比较
	bool CompareWithIncreaceTime(const TimeLineInfo & L1, const TimeLineInfo & L2);

	//保证时间行有效（无法保证则返回 false）
	bool MakeSureTimeLineValid(wstring& line);

public:
	SStringW	m_strTitle;					/* 歌曲标题 */
	SStringW	m_strArtist;				/* 艺术家 */
	SStringW	m_strAlbum;					/* 专辑 */
	SStringW	m_strEditor;				/* 编辑的人 */

private:
	bool m_bDealOffset;						/* 表示是否处理LRC文件中的时间偏移 */
	
	int			m_nOffset;					/* 时间偏移量，为正数表示整体提前 */

	vector<TimeLineInfo> m_vecNeteaseLrc;	/* 储存用于网易云的歌词信息 */

	bool	m_bIsLrcFileValid;				/* 表示歌词文件是否有效 */
};

