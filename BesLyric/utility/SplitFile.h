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

//用于分割文件，组成文件
class CSplitFile
{
public:
	/*
	*	@brief	分割文件 
	*	@param	strSrcFile		文件路径
	*	@param	strToPath		分割后储存路径(不以“\”结尾)
	*	@param	strToName		分割后储存的文件名
	*	@param	nBlockSize		问个单元大小
	*	@return	
	*/
	static bool SplitFile(wstring strSrcFile, wstring strToPath, wstring strToName, unsigned int nBlockSize);

	/* 
	*	@brief	合并被分割的文件 
	*	@param	strSplitFilePath	文件所在路径（被分割后的文件） (不以“\”结尾)
	*	@param	strFileName			文件名前缀（被分割后的文件）
	*	@param	strMergeToPath		合并文件后储存的路径 (不以“\”结尾)
	*	@return	
	*/
	static bool MergeFile(wstring strSplitFilePath, wstring strFileName, wstring strMergeToPath);

private:
	/* 以下函数由 MergeFile 调用  */

	//获取被分割的文件数
	static bool GetMaxSplitedCount(wstring strSplitFilePath,wstring strFileName, int &nMaxCount);

	//获取文件拓展名和 文件大小
	static bool GetMergedFileExtAndSize(wstring strMergedFile, WCHAR* strExt, int nSize,int &nSizeOfFileRecord);

	//合并为同一个文件
	static bool MergeToOneFile(wstring strMergedFilePathName,wstring  strTargetFileNamePreW, int nMaxCount, int &nCumulateSize);
};