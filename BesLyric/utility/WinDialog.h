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
* @file       WinDialog.h
* 
* Describe   对话框相关类的定义
*/


#pragma once
#include "stdafx.h"
#include <windows.h>
#include <Shlobj.h>

//定义歌词文件每一行的最多字符数
#define MAX_CHAR_COUNT_OF_LINE 1000
#define MAX_WCHAR_COUNT_OF_LINE MAX_CHAR_COUNT_OF_LINE/2

//改写GetOpenFileName时用到的系统使用控件的资源ID
#define  ID_COMBO_ADDR 0x47c
#define  ID_LEFT_TOOBAR 0x4A0

//使用GetOpenFileName打开文件夹时 用来替换 m_ofn.lpfnHook 的回调函数 
UINT_PTR __stdcall  MyFolderProc(  HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam );
LRESULT __stdcall  _WndProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam  );


/*
*	@brief 用于打开文件 和 文件夹(方式一)
*/
class CBrowseDlg
{
	public:
		CBrowseDlg(void);
		~CBrowseDlg(void);

	protected:
		WCHAR m_pszDirPath[MAX_PATH]; //选择的目录
		WCHAR m_pszFilePath[MAX_PATH];  //选择中的文件地址

	public:
		BOOL DoDirBrowse(HWND hwndOwner, LPCTSTR pszDisplayName, BOOL bAddNewFolder, const WCHAR *szDefaultDir = NULL);  //文件夹浏览
		BOOL DoFileBrowse(HWND hWnd, LPCTSTR pFilter, const WCHAR *pInitialDir);  //文件选择器

		LPTSTR GetDirPath();  //获取目录路径
		LPTSTR GetFilePath();  //获取文件路径

private:
		//参考 http://blog.csdn.net/shuilan0066/article/details/7302904
		static  int CALLBACK  BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData);

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
        LPCTSTR lpszFileName = NULL,
		LPCTSTR lpszFileDir = NULL,
        LPCTSTR lpszDefExt = NULL,
        LPCTSTR lpszFilter = NULL,
        DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT ,
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

	//判断文件夹是否存在
	static BOOL IsFloderExist(const wstring &strPath);
};

