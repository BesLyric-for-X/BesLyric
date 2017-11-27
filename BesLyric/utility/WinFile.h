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
#include <Windows.h> 
#include <vector>
#include "WinFile.h"
using namespace std;


/*
*   @brief	文件读取类，支持Windows多种编码
*					应用RAII思想，管理文件文件的资源
*/
class File{

public:
	File():m_pf(NULL),m_lpszPathFile(NULL),m_lpszMode(NULL){}
	
	//读取文件打开时，支持ascii\unicode little endian\ unicode big endian \utf-8 编码格式
	//写入打开文件时，直接由函数 wideCharToMultiChar 以ascii 方式打开
	//mode : “r”“w”
	File(LPCTSTR pathFile,LPCTSTR mode);

	inline BOOL isValidFile(){return m_pf!=NULL;}

	~File(){
		//如果已存在，则释放资源
		if(m_pf) fclose(m_pf);
	}
private:
	//测试数据是否是UTF-8 无Bom格式  
	bool IsUTF8WithNoBom(const void* pBuffer, long size);   

public:
	LPCTSTR m_lpszPathFile;		/* 文件路径和名字串*/
	LPCTSTR m_lpszMode;			/* 打开文件的模式 */

	FILE *m_pf;					/* 存放当前打开文件的指针*/
	ENCODING_TYPE::encoding_type m_encodingType;	/* 存放文件编码格式 */
};


/*
*	@brief	负责文件的读写操作
*/
class FileOperator
{
public:
	static bool ReadAllText(const string file, string& fileContent);

	static bool ReadAllLines(const string file, OUT vector<string> *lines);

	//读取文件所有行，支持4种windows notepad 基本编码文件，自动去除所有空行
	static bool ReadAllLinesW(const wstring file, OUT vector<SStringW> *lines);
	static bool ReadAllLinesW(File& encodingFile,  OUT vector<SStringW> *lines);

	//以ascii 编码保存
	static bool SaveToFile(const string file, string& fileContent);
	
	//以 UTF-8 编码写到文件
	static bool WriteToUtf8File(const wstring file, wstring fileContent);

	static bool WriteToUtf8File(const wstring file, vector<SStringW> lines);
};


/*
*	@biref	辅助文件操作，如获得路径，判断路径是否为文件夹等
*/
class FileHelper
{
public:
	/* 获得应用程序当前的路径 */
	static wstring GetCurrentDirectoryStr();

	/**
	*   @brief 检查文件名是否符合要求格式（仅仅检查名字上的格式）
	*	@param  format  支持的检查格式		普通文件格式：如 *.txt、 *.mp3  ("*." 是必须的；且后缀必须至少有一个字符)
	*										文件夹格式：..
	*			toChecked 被检查的路径字符串
	*	@return TRUE 符合要求
	*	@note
	*/
	static bool CheckPathName(LPCTSTR format, LPCTSTR toChecked);

	/*
	*   @brief 判断传递的路径是否为路径
	*/
	static bool IsDirectory(SStringW path);


	/*
	*   @brief 查询文件是否存在
	*/
	static bool  CheckFileExist(const wstring &strPath);

	/*
	*   @brief 查询文件夹是否存在
	*/
	static bool  CheckFolderExist(const wstring &strPath);

	/**
	*	@brief 分割路径名
	*	@note	路径可以是全路径，如 C:\\document\\desktop\\test.pan
	*				也可以不是，  如 ..\..\test\test.pan
	*/
	static void SplitPath(const wstring& strPathName, OUT wstring *strDrive = NULL, OUT wstring *strDirectory = NULL, OUT wstring* strName = NULL, OUT wstring* strExt = NULL);
};

  
