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
#include "stdafx.h"
#include "Globle.h"
#include <time.h>
#include <direct.h>
#include <fstream>
using namespace std;

SINGLETON_C_D_0(CGloble)

	
bool CGloble::bDeleted = false;//已经删除

//获得程序的路径
string CGloble::GetPragramDir()
{
	char exeFullPath[MAX_PATH]; // Full path  
	GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);

	string strPath(exeFullPath);
	strPath = strPath.substr(0, strPath.find_last_of("\\"));
	return strPath;
}

//获得相对路径对应的真实路径
string CGloble::GetRelativePath(string strRelativePath)
{
	return GetPragramDir() + strRelativePath;
}


//追加记录
void CGloble::AppendLog(string info)
{
	string logPath = GetRelativePath("\\调试信息.txt");

	ofstream   oflog(logPath, ios::app);

	time_t timep;
	time(&timep);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "[%Y-%m-%d %H:%M:%S]", localtime(&timep));

	info = tmp + info;

	oflog << info << endl;

	oflog.close();
}

//删除调试LOG
void CGloble::DeleteLogFile()
{
	if (!CGloble::bDeleted)
	{
		string logPath = GetRelativePath("\\调试信息.txt");

		remove(logPath.c_str());

		CGloble::bDeleted = true;
	}
}



