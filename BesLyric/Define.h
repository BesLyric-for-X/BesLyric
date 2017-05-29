/*
	BesLyric  一款 操作简单、功能实用的 专门用于制作网易云音乐滚动歌词的 歌词制作软件。
    Copyright (C) 2017  BensonLaur

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
* @file       Define.h
* 
* Describe    定义了 程序使用的一些结构
*/

#pragma once
#include "stdafx.h"
using namespace std;



/* 枚举定义 */
enum ENCODING_TYPE			//文件编码类型
{
	ASCII,						
	UNICODE_LITTLE_ENDIAN,
	UNICODE_BIG_ENDIAN,
	UTF_8,
	OTHER
};

