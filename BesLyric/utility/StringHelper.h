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

#pragma once
#include "stdafx.h"
#include <string>
#include <vector>
using namespace std;

//处理字符串相关操作
class CStringHelper
{
public:
	/*
	*	@brief	去除前后字符
	*	@param	strToTrim				被处理字符串
	*	@param	strCharactorRemoved		去除的字符
	*	@return	
	*/
	static SStringW Trim(SStringW strToTrim, SStringW strCharactorRemoved = L" \t");
	static wstring Trim(wstring strToTrim, wstring strCharactorRemoved = L" \t");

	//获得Trim之后的长度
	static int GetTrimLength(SStringW strToTrim, SStringW strCharactorRemoved = L" \t");
	static int GetTrimLength(wstring strToTrim, wstring strCharactorRemoved = L" \t");

	//将字符串分割成行
	static void SplitStringByToLines(SStringW strToSplted,WCHAR splitChar, vector<SStringW>& vecLines);

private:
	//字符是否在字符串内出现
	static bool IsCharInString(WCHAR ch, SStringW str);
};