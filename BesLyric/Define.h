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
* @file       Define.h
* 
* Describe    定义了 程序使用的一些结构
*/

#pragma once
#include "stdafx.h"
#include <string>
using namespace std;

/* 宏定义 */
#ifdef _DEBUG

#define UPDATE_LOOP  0			/* 是否在启动时更新程序 */

#else

#define UPDATE_LOOP  1

#endif

#define RET_SUCCEEDED	0
#define MAX_BUFFER_SIZE	 (260 * 2)

#define MSG_USER_MAKING_START_BUTTON	WM_USER+1
#define MSG_USER_PLAYING_START_BUTTON	WM_USER+2

#define MSG_USER_UPDATE_CHECK_PROGRESS	WM_USER+3
#define MSG_USER_CLOSE_CHECK_PROGRESS	WM_USER+4

#define MSG_USER_DROP_FILE				WM_USER+5

/* 结构定义 */

//在制作歌词页面 和 滚动预览 页面中，需要选择音乐路径，歌词路径，输出路径等信息
//该结构用于存储 路径名 以及 是否已被选择的状态
typedef struct _PATH_STATE
{	
	TCHAR nameOfPath[_MAX_PATH];
	bool isInited;
}PATH_STATE; 

enum ENCODING_TYPE			//文件编码类型
{
	ASCII,						
	UNICODE_LITTLE_ENDIAN,
	UNICODE_BIG_ENDIAN,
	UTF_8,
	OTHER
};


/* 全局变量定义 */
/* 版本格式说明：X.Y.Z  
X表示主版本，架构性修改时更新
Y表示次版本，较大修改时更新
Z表示修改号，小问题时更新
*/
static const wstring VERSION_NUMBER = L"2.1.0";		//版本号（注意每次更改版本号时需要更改2处，1处是这里，1处是 BesLyric.rc 中的Version）

static const wstring  LINK_VERSION_LOG= L"http://files.cnblogs.com/files/BensonLaur/versionLog.zip";			//链接，指向版本日志文件
static const wstring  LINK_LAST_VERSION_INFO= L"http://files.cnblogs.com/files/BensonLaur/lastVersion.zip";		//链接，指向最后版本信息的文件
static const wstring  LINK_LAST_EXE= L"http://files.cnblogs.com/files/BensonLaur/BesLyricExe.zip";				//链接，指向最新的 EXE文件

static const wstring LINK_SEND_LOGIN = L"http://beslyric.320.io/BesBlog/beslyric/login.action";

static const wstring FILE_NAME_LAST_VERSION_INFO = L"version";					//文件名，从 LINK_LAST_VERSION_INFO 下载下来储存的文件
static const wstring FILE_NAME_LAST_EXE_TEMP = L"BesLyric";						//文件名，从 LINK_LAST_EXE 下载下来储存的文件
static const wstring SETTING_FILE_NAME = L"setting";							//文件名，储存设置

static const wstring TEMP_WAV_FLODER_NAME = L"wav";								//文件夹名称，存放临时转换得到的wav文件

static const wstring SERVER_FILE_EXTENTION_W = L".zip";							//定义上传到 cnblog服务器的文件拓展名
static const string SERVER_FILE_EXTENTION_A = ".zip";	

static const wstring LINK_SERVER_PATH = L"http://files.cnblogs.com/files/BensonLaur/";		//链接，服务器地址

static const wstring LINK_DOWNLOAD_SERVER = L"http://ovfwclhwl.bkt.clouddn.com/";			//链接，服务器地址2