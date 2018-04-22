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
* @file       Globle.h
* 
* Describe    定义了 单例使用的全局变量和方法
*/


#pragma once
#include "stdafx.h"
#include <iostream>
#include <string>
#include "lib\ZSingleton.hpp"
using namespace std;


class CGloble{

	SINGLETON_0(CGloble)

public:
	//获得程序的路径
	string GetPragramDir();

	//获得相对路径对应的真实路径
	string GetRelativePath(string strRelativePath);

	//追加记录
	void AppendLog(string info);

	//删除调试LOG
	void DeleteLogFile();

	static bool bDeleted;//已经删除

};
