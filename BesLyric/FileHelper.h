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
* @file       FileHelper.h
* 
* Describe    File类（管理打开的文件） 和CFileDialogEx 类（用对话框选择文件和文件夹）的定义
*/



#pragma once
#include "stdafx.h"
#include <windows.h>
#include <Shlobj.h>

//定义歌词文件每一行的最多字符数
#define MAX_CHAR_COUNT_OF_LINE 200
#define MAX_WCHAR_COUNT_OF_LINE MAX_CHAR_COUNT_OF_LINE/2

//改写GetOpenFileName时用到的系统使用控件的资源ID
#define  ID_COMBO_ADDR 0x47c
#define  ID_LEFT_TOOBAR 0x4A0

//使用GetOpenFileName打开文件夹时 用来替换 m_ofn.lpfnHook 的回调函数 
UINT_PTR __stdcall  MyFolderProc(  HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam );
LRESULT __stdcall  _WndProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam  );

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
public:
	LPCTSTR m_lpszPathFile;		/* 文件路径和名字串*/
	LPCTSTR m_lpszMode;			/* 打开文件的模式 */

	FILE *m_pf;					/* 存放当前打开文件的指针*/
	ENCODING_TYPE m_encodingType;	/* 存放文件编码格式 */
};


/*
*	@brief 用于打开文件 和 文件夹(方式一)
*/
class CBrowseDlg
{
	public:
		CBrowseDlg(void);
		~CBrowseDlg(void);

	protected:
		char m_pszDirPath[MAX_PATH]; //选择的目录
		char m_pszFilePath[MAX_PATH];  //选择中的文件地址
		wchar_t m_pszFilePath2[MAX_PATH];  //选择中的文件地址

	public:
		BOOL DoDirBrowse(HWND hwndOwner, LPCTSTR pszDisplayName, BOOL bAddNewFolder);  //文件夹浏览
		BOOL DoFileBrowse(HWND hWnd, LPCSTR pFilter, const char *pInitialDir);  //文件选择器

		LPSTR GetDirPath();  //获取目录路径
		LPSTR GetFilePath();  //获取文件路径

};


/*
*	@brief 用于打开文件 和 文件夹； 以及保存文件(方式二)
*/
class CFileDialogEx
{
public:

    OPENFILENAME m_ofn;
    BOOL m_bOpenFileDialog;            // TRUE for file open, FALSE for file save
    TCHAR m_szFileTitle[_MAX_FNAME];   // contains file title after return
    TCHAR m_szFileName[_MAX_PATH];     // contains full path name after return

	//构造函数
    CFileDialogEx(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
        LPCTSTR lpszDefExt = NULL,
        LPCTSTR lpszFileName = NULL,
        DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT ,
        LPCTSTR lpszFilter = NULL,
        HWND hWndParent = NULL,
		BOOL bFloder = FALSE);

	//打开选择窗口
    INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow());

public:
	/**
	*   @brief 检查文件名是否符合要求格式（仅仅检查名字上的格式）
	*	@param  format  支持的检查格式		普通文件格式：如 *.txt、 *.mp3  ("*." 是必须的；且后缀必须至少有一个字符)
	*										文件夹格式：..
	*			toChecked 被检查的路径字符串
	*	@return TRUE 符合要求
	*	@note	
	*/
	static BOOL checkPathName(LPCTSTR format,LPCTSTR toChecked);

};

