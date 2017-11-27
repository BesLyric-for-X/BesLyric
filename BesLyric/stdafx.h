// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

#include <WinSock2.h>

#ifndef _CRT_SECURE_NO_WARNINGS
#define  _CRT_SECURE_NO_WARNINGS
#endif

#define	 DLL_SOUI
#include <souistd.h>
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <res.mgr/sobjdefattr.h>
#include <com-cfg.h>
#include "resource.h"
#define R_IN_CPP	//定义这个开关来
#include "res\resource.h"
using namespace SOUI;

#include <string>
using namespace std;

#include "Define.h"


extern int _MessageBox(HWND hwnd,LPCTSTR content, LPCTSTR tiltle, UINT uType);


//仅用于测试时，快速写出 弹框提示的代码 : MB("content");
void MB(LPCTSTR content);

//禁用第三方库的警告
//#pragma warning(disable:4127)
//#pragma warning(disable:4244)
//#pragma warning(disable:4311)
//#pragma warning(disable:4312)
//#pragma warning(disable:4512)
//#pragma warning(disable:4571)
//#pragma warning(disable:4640)
//#pragma warning(disable:4706)
//#pragma warning(disable:4710)
//#pragma warning(disable:4800)
//#pragma warning(disable:4804)
//#pragma warning(disable:4820)
#pragma warning(disable:4996)			//在这里消除jsoncpp 库中的警告
