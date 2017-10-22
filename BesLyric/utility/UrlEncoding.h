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
using namespace std;

//处理字符串相关操作
//参考链接： http://blog.csdn.net/evense/article/details/51289635
class CUrlEncodinig
{
public:  
    void UTF_8ToGB2312(string &pOut, char *pText, int pLen);//utf_8转为gb2312  
    void GB2312ToUTF_8(string& pOut,char *pText, int pLen); //gb2312 转utf_8  
    string UrlGB2312(char * str);                           //urlgb2312编码  
    string UrlUTF8(char * str);                             //urlutf8 编码  
    string UrlUTF8Decode(string str);                  //urlutf8解码  
    string UrlGB2312Decode(string str);                //urlgb2312解码  
  
private:  
    void Gb2312ToUnicode(WCHAR* pOut,char *gbBuffer);  
    void UTF_8ToUnicode(WCHAR* pOut,char *pText);  
    void UnicodeToUTF_8(char* pOut,WCHAR* pText);  
    void UnicodeToGB2312(char* pOut,WCHAR uData);  
    char CharToInt(char ch);  
    char StrToBin(char *str);  
};