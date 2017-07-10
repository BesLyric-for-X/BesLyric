#include "stdafx.h"
#include "BSMessageBox.h"

//发送自定义图标的消息
int _MessageBox(HWND hwnd,LPCTSTR content, LPCTSTR tiltle, UINT uType)
{
	BSMessageBox m;
	return m.MessageBoxW(hwnd,content,tiltle,uType);
}