// stdafx.cpp : source file that includes just the standard includes
//	SkinTest.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


//仅用于测试时，快速写出 弹框提示的代码 : MB("content");
void MB(LPCTSTR content)
{
	MessageBox(NULL,content,_T("tip"),MB_OK);
}