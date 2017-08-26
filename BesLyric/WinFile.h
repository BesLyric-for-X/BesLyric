#pragma once
#include "stdafx.h"
#include <string>
#include <Windows.h> 
#include <vector>
using namespace std;

/*
*	@brief	负责文件的读写操作
*/
class FileOperator
{
public:
	static bool ReadAllText(const string file, string& fileContent);

	static bool ReadAllLines(const string file, OUT vector<string> *lines);

	static bool ReadAllLinesW(const wstring file, OUT vector<SStringW> *lines);

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
	static void SplitPath(const string& strPathName, OUT string *strDrive = NULL, OUT string *strDirectory = NULL, OUT string* strName = NULL, OUT string* strExt = NULL);
};

  
