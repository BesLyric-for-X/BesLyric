/*
*	Copyright (C) 2017  BensonLaur
*	note: Looking up header file for license detail
*/

#include "stdafx.h"
#include "WinDialog.h"
#include <Windows.h>
#include "Commdlg.h"
#include <fstream>
using namespace std;

/*  用于打开文件 和 文件夹(方式一) */
CBrowseDlg::CBrowseDlg(void)
{
	memset(m_pszDirPath, 0, MAX_PATH);
	memset(m_pszFilePath, 0, MAX_PATH);
}

CBrowseDlg::~CBrowseDlg(void)
{
}

//文件夹浏览
BOOL CBrowseDlg::DoDirBrowse(HWND hwndOwner, LPCTSTR pszTitle, BOOL bAddNewFolder,const WCHAR *szDefaultDir)  
{
	BROWSEINFO bi = {0};
	bi.hwndOwner = hwndOwner;  
	bi.lpszTitle = pszTitle;
	bi.ulFlags = bAddNewFolder ? BIF_NEWDIALOGSTYLE : 0;

	//设置默认路径
	bi.lpfn = BrowseCallbackProc ;        //设置CALLBACK函数  
	bi.lParam = (LPARAM)szDefaultDir;	//设置默认路径  

	PIDLIST_ABSOLUTE pItem = ::SHBrowseForFolder(&bi);  
	if (pItem != NULL)
	{
		BOOL bRet = ::SHGetPathFromIDListW(pItem, m_pszDirPath);

		LPMALLOC pMalloc;  
		if(SUCCEEDED(SHGetMalloc(&pMalloc)))//pidl指向的对象用完应该释放
		{  
			pMalloc->Free(pItem);  
			pMalloc->Release();  
		}  

		return bRet;
	}

	return FALSE;  
}

//获取目录
LPTSTR CBrowseDlg::GetDirPath()
{
	return m_pszDirPath;
}

//文件浏览
BOOL CBrowseDlg::DoFileBrowse(HWND hWnd, LPCTSTR pFilter, const WCHAR *pInitialDir)
{
	memset(m_pszFilePath, 0, MAX_PATH);
	OPENFILENAMEW ofn = {0};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = pFilter;  //_T("Exe文件(*.exe)\0*.exe\0所有文件(*.*)\0*.*\0");
	ofn.lpstrInitialDir = pInitialDir;  //默认的文件路径
	ofn.lpstrFile = m_pszFilePath;  //存放文件的缓冲区
	ofn.nMaxFile = MAX_PATH;
	ofn.nFilterIndex = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER ;//标志如果是多选要加上OFN_ALLOWMULTISELECT  
	return GetOpenFileNameW(&ofn);
}

//获取文件路径
LPTSTR CBrowseDlg::GetFilePath()
{
	return m_pszFilePath;
}

int CALLBACK CBrowseDlg::BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)  
{  
    switch(uMsg)  
    {  
    case BFFM_INITIALIZED:    //初始化消息  
        ::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,lpData);   //  m_filePath 为类的静态变量  
        break;  
    case BFFM_SELCHANGED:    //选择路径变化，  
        {  
            TCHAR curr[MAX_PATH];     
            SHGetPathFromIDList((LPCITEMIDLIST)lParam,curr);     
            ::SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)curr);     
        }  
        break;  
    default:  
        break;  
    }  
    return 0;     
}



/*  用于打开文件 和 文件夹； 以及保存文件(方式二) */

//构造函数
CFileDialogEx::CFileDialogEx(BOOL bOpenFileDialog, LPCTSTR lpszFileName,LPCTSTR lpszFileDir,LPCTSTR lpszDefExt, LPCTSTR lpszFilter,
       DWORD dwFlags , HWND hWndParent, BOOL bFloder)
{
        memset(&m_ofn, 0, sizeof(m_ofn)); // initialize structure to 0/NULL
        m_szFileName[0] = _T('\0');
        m_szFileTitle[0] = _T('\0');

        m_bOpenFileDialog = bOpenFileDialog;
        m_ofn.lStructSize = sizeof(m_ofn);
        m_ofn.lpstrFile = m_szFileName;
		m_ofn.lpstrInitialDir = lpszFileDir;
        m_ofn.nMaxFile = _MAX_PATH;
        m_ofn.lpstrDefExt = lpszDefExt;
        m_ofn.lpstrFileTitle = (LPTSTR)m_szFileTitle;
        m_ofn.nMaxFileTitle = _MAX_FNAME;
        m_ofn.Flags = dwFlags | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLESIZING| OFN_NOCHANGEDIR;
        m_ofn.lpstrFilter = lpszFilter;
        m_ofn.hwndOwner = hWndParent;

        // setup initial file name
        if(lpszFileName != NULL)
            _tcscpy_s(m_szFileName, _countof(m_szFileName), lpszFileName);

		//文件夹设置
		if(bFloder)
		{
			m_ofn.hInstance = (HMODULE)GetCurrentProcess();//不要使用NULL,可能造成无法定制的问题
			m_ofn.lpfnHook = (LPOFNHOOKPROC)MyFolderProc;
		}
}

//打开选择窗口
INT_PTR CFileDialogEx::DoModal(HWND hWndParent)
{
     if(m_ofn.hwndOwner == NULL)   // set only if not specified before
         m_ofn.hwndOwner = hWndParent;

     if(m_bOpenFileDialog)
         return ::GetOpenFileName(&m_ofn);
     else
         return ::GetSaveFileName(&m_ofn);
}

//检查文件名是否符合要求格式
BOOL CFileDialogEx::checkPathName(LPCTSTR format,LPCTSTR toChecked)
{
	int i;
	bool isFloder = false;
	//TODO：异常抛出处理
	int len = _tcslen(format);
	if(_tcscmp(format,_T(".."))==0)
	{
		isFloder = true;
	}
	else if(len < 3 || format[0]!=_T('*') || format[1]!=_T('.'))
		return FALSE;  //TODO：异常
	

	//获取并检查 被检查的路径字符串 toChecked 的信息
	TCHAR pathName[_MAX_PATH];
	TCHAR ext[_MAX_EXT];

	int lenPathName = 0, pos =-1;

	_tcscpy(pathName,toChecked);
	lenPathName = _tcslen(pathName);	//得到路径总长
	if(!lenPathName)
		return FALSE;

	//得到路径中最后一个“.”的位置置于pos中
	for( i=0; i< lenPathName; i++)
	{
		if(_T('.')==pathName[i])
			pos = i;
	}

	if(isFloder) //检查文件夹类型
	{
		if(pos == -1)//这里默认文件夹的路径不包含任何点'.'
			return TRUE;
		else
			return FALSE;
	}
	else //检查普通后缀名类型
	{
		_tcscpy(ext,&pathName[pos+1]);  //得到路径的后缀（不包含“.”）
		if(_tcscmp(&format[2],ext)==0)	//和 参数提供的后缀对比
			return TRUE;
		else
			return FALSE;
	}
}


LONG g_lOriWndProc = NULL;
BOOL g_bReplaced = FALSE;

//以下代码和2个函数来自：http://www.cnblogs.com/kocpp/p/5349467.html
/*
		//文件夹设置
		if(bFloder)
		{
			m_ofn.hInstance = (HMODULE)GetCurrentProcess();//不要使用NULL,可能造成无法定制的问题
			m_ofn.lpfnHook = (LPOFNHOOKPROC)MyFolderProc;
		}
*/

//选择文件夹时，设置HookProc: m_ofn.lpfnHook = (COMMDLGPROC)MyFolderProc; 

LRESULT  __stdcall  _WndProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam  )
{
    switch(uMsg)
    {
    case WM_COMMAND:
        {
            if( wParam == IDOK )
            {
                wchar_t wcDirPath[MAX_PATH] = {0};
                HWND hComboAddr = GetDlgItem(hwnd, ID_COMBO_ADDR);
                if (hComboAddr != NULL)
                {
                    GetWindowText(hComboAddr, wcDirPath, MAX_PATH);
                }
                if (!wcslen(wcDirPath))
                {
                    break;
                }
                DWORD dwAttr = GetFileAttributes(wcDirPath);
                if(dwAttr != -1 && (FILE_ATTRIBUTE_DIRECTORY & dwAttr))
                {
                    LPOPENFILENAMEW oFn = (LPOPENFILENAME)GetProp(hwnd, L"OPENFILENAME");
                    if (oFn)
                    {
                        int size = oFn->nMaxFile > MAX_PATH?MAX_PATH: oFn->nMaxFile;
                        memcpy(oFn->lpstrFile, wcDirPath, size * sizeof(wchar_t));
                        RemoveProp(hwnd, L"OPENFILENAME");
                        EndDialog(hwnd, 1);
                    }
                    else
                    {
                        EndDialog(hwnd, 0);
                    }
                }
                break;
            }
            //////////////////////////////////////////////////////////////////////////
            //如果是左边toolbar发出的WM_COMMOND消息（即点击左边的toolbar）, 则清空OK按钮旁的组合框。
            HWND hCtrl = (HWND)lParam;
            if (hCtrl == NULL)
            {
                break;
            }
            int ctrlId = GetDlgCtrlID(hCtrl);
            if (ctrlId == ID_LEFT_TOOBAR)
            {
                HWND hComboAddr = GetDlgItem(hwnd, ID_COMBO_ADDR);
                if (hComboAddr != NULL)
                {
                    SetWindowTextW(hComboAddr, L"");
                }
            }
        }
    break;
    }    
    int i = CallWindowProc( (WNDPROC) g_lOriWndProc , hwnd, uMsg, wParam ,lParam );
    return i;
}

UINT_PTR __stdcall  MyFolderProc(  HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    //参考reactos可知，hdlg 是一个隐藏的对话框，其父窗口为打开文件对话框， OK，CANCEL按钮等控件的消息在父窗口处理。
    if(uiMsg == WM_NOTIFY)
    {
        LPOFNOTIFY lpOfNotify = (LPOFNOTIFY)lParam;
        if (lpOfNotify->hdr.code == CDN_INITDONE )
        {
            SetPropW(GetParent(hdlg), L"OPENFILENAME", (HANDLE)(lpOfNotify->lpOFN));
            g_lOriWndProc  = ::SetWindowLongW( ::GetParent( hdlg ), GWL_WNDPROC , (LONG)_WndProc );    
        }
        if (lpOfNotify->hdr.code == CDN_SELCHANGE)
        {
            wchar_t wcDirPath[MAX_PATH] = {0};
            CommDlg_OpenSave_GetFilePathW(GetParent( hdlg ), wcDirPath, sizeof(wcDirPath));            
            HWND hComboAddr = GetDlgItem(GetParent(hdlg), ID_COMBO_ADDR);
            if (hComboAddr != NULL)
            {
                if (wcslen(wcDirPath))
                {
                    //去掉文件夹快捷方式的后缀名。
                    int pathSize = wcslen(wcDirPath);
                    if (pathSize >= 4)
                    {
                        wchar_t* wcExtension = PathFindExtensionW(wcDirPath);
                        if (wcslen(wcExtension))
                        {
                            wcExtension = CharLowerW(wcExtension);
                            if(!wcscmp(wcExtension, L".lnk"))
                            {
                                wcDirPath[pathSize - 4] = L'\0';
                            }
                        }
                    }
                    SetWindowTextW(hComboAddr, wcDirPath);
                }
                else
                {
                    SetWindowTextW(hComboAddr, L"");
                }                
            }
        }
    }
    return 1;
}
