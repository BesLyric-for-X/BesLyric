/*
*	Copyright (C) 2017  BensonLaur
*	note: Looking up header file for license detail
*/

// MainDlg.cpp :  主窗口类的实现	
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Windows.h>
#include "MainDlg.h"
#include "utility/WinDialog.h"
#include <helper/SDibHelper.h>
#include "BSMessageBox.h"
#include "entity/AutoUpdateThread.h"
#include "entity\CheckIntegrityThread.h"
#include "entity\FileDroper.h"
#include "utility\Downloader.h"
#include "entity\OsInfoHelper.hpp"

//经过个人测试，音乐播放设备，在毫秒级访问时，（比如获取歌曲当前位置）会有一定的延迟，导致声音已经被设备播放，
//但是在获取实时信息来显示时存在延迟，在此定义设备可能的延迟（单位：毫秒）
#define MUSIC_DIVICE_DELAY 300
	
#ifdef DWMBLUR	//win7毛玻璃开关
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")
#endif

#include "utility\SplitFile.h"
#include <fstream>
using namespace std;


//just for test
void CMainDlg::test()
{
	//just for test
	
	//wstring systemArch = S_CA2W(OsInfoHelper().GetOsBits().c_str()).GetBuffer(64);
	//systemArch = systemArch;

	//CSplitFile::SplitFile(L"C:\\Users\\Administrator\\Desktop\\3.4.1\\ffmpeg.exe", L"C:\\Users\\Administrator\\Desktop\\3.4.1",L"ff", 4 * 1024 * 1024);

	//CSplitFile::MergeFile(L"C:\\Users\\Administrator\\Desktop\\3.4.1",L"ff", L"C:\\Users\\Administrator\\Desktop\\3.4.1");
	
	//CDownloader::DownloadFile(L"https://github.com/BensonLaur/image-storage/raw/master/General Resource/download/ffmpeg.exe", 
	//	L"C:\\Users\\BensonLaur\\Desktop\\ff.exe");
	
	//wstring strSaveBuffer;
	//CDownloader::DownloadString( L"http://gecimi.com/api/lyric/我们的歌", strSaveBuffer);
	//
	//locale &loc=locale::global(locale(locale(),"",LC_CTYPE)); 
	//wofstream ofstream("C:\\Users\\BensonLaur\\Desktop\\json.json");
	//ofstream << strSaveBuffer << endl;
	//ofstream << L"中文1 test1"  << endl;
	//ofstream << L"中文2 test2"  << endl;
	//ofstream.close();
	//_tfopen(L"C:\\Users\\BensonLaur\\Desktop\\json.test", L"w");

	//CDownloader::DownloadString( L"http://s.gecimi.com/lrc/388/38847/3884774.lrc", strSaveBuffer);
	
	//CDownloader::DownloadFile( L"http://music.163.com/song/media/outer/url?id=531051690.mp3", 
	//	L"C:\\Users\\BensonLaur\\Desktop\\NetEase\\The Middle(自动下载).mp3");
	//CDownloader::DownloadFile( L"http://music.163.com/song/media/outer/url?id=1111.mp3", 
	//	L"C:\\Users\\BensonLaur\\Desktop\\NetEase\\test.mp3");
	
	//string md5;
	//CCheckIntegrityThread::GetFileMd5(L"E:\\git\\BesLyric\\Debug\\imgdecoder-gdip.dll", md5);
	//CCheckIntegrityThread::GetFileMd5(L"E:\\git\\BesLyric\\Debug\\render-gdi.dll", md5);
	//CCheckIntegrityThread::GetFileMd5(L"E:\\git\\BesLyric\\Debug\\soui.dll", md5);
	//CCheckIntegrityThread::GetFileMd5(L"E:\\git\\BesLyric\\Debug\\soui-sys-resource.dll", md5);
	//CCheckIntegrityThread::GetFileMd5(L"E:\\git\\BesLyric\\Debug\\utilities.dll", md5);

}

CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
	//test();
	m_bLayoutInited = FALSE;
	
	m_pageMaking = NULL;
	m_pageResult = NULL;
	m_pageSearchLyric = NULL;
	m_pageSearchNcmID = NULL;
}

CMainDlg::~CMainDlg()
{
	//结束自动更新线程
	AutoUpdateThread::getSingleton().Stop();
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	#ifdef DWMBLUR	//win7毛玻璃开关
	MARGINS mar = {5,5,30,5};
	DwmExtendFrameIntoClientArea ( m_hWnd, &mar );
	#endif

	SetMsgHandled(FALSE);
	return 0;
}
BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
    //设置为磁吸主窗口
    SetMainWnd(m_hWnd);

	initDesktopLyric();	//初始化桌面歌词

	//设置背景图片
	setBackSkin();

	initPage();//初始化各个页面内容

	m_bLayoutInited = TRUE;
	
	//初始化 m_bIsLyricMaking 状态为 “未在制作中”，该状态可以锁定（忽略）键盘消息
	m_bIsLyricMaking =FALSE;
	//未在“播放预览”中
	m_bIsLyricPlaying = FALSE;
	
	//音量的初值为最大值1000
	this->m_nVolumn = 1000;

	//初始化记录页面播放足迹，详看变量的说明
	FootPrintPage = -1;
	
	//test();
	//初始化页面之后再执行耗时的操作

	//初始化etc文件并清理相关文件
	initFloderAndFile();

	//启动自动更新线程
	AutoUpdateThread::getSingleton().Start();

	//检测程序的完整性
	CCheckIntegrityThread::getSingleton().Start(false);
	return 0;
}

//初始化各个页面内容
void CMainDlg::initPage()
{
	m_pageMaking = new CPageMaking;
	m_pageResult = new CPageResult;
	m_pageSearchLyric = new CPageSearchLyric;
	m_pageSearchNcmID = new CPageSearchNcmID;

	//初始化“歌词制作页面”
	//默认输出路径有效时设置输出路径
	if(FileHelper::CheckFolderExist(m_settingPage.m_default_output_path))
	{
		SWindow *m_szOutputPath = FindChildByID(R.id.edit_output);
		m_szOutputPath->SetWindowTextW(m_settingPage.m_default_output_path.c_str());

		m_pageMaking->PATH_STATE_1[2].isInited = true;
		//加入歌词制作器
		maker.setm_szOutputPath(m_settingPage.m_default_output_path.c_str());
	}


	//初始化“设置页面”
	m_settingPage.Init(this);
	m_pageMaking->Init(this);
	m_pageResult->Init(this);
	m_pageSearchLyric->Init(this);
	m_pageSearchNcmID->Init(this);
		
	maker.Init( &m_settingPage );//歌词制作需要用到设置页面的数据

	//注册文件拖放
	HRESULT hr=::RegisterDragDrop(m_hWnd,GetDropTarget());
	RegisterDragDrop(m_pageMaking->m_EditMusic->GetSwnd(),new CFileDroper( this->m_hWnd ,m_pageMaking->m_EditMusic));
	RegisterDragDrop(m_pageMaking->m_EditLyric->GetSwnd(),new CFileDroper( this->m_hWnd ,m_pageMaking->m_EditLyric));
	RegisterDragDrop(m_pageMaking->m_EditOutPath->GetSwnd(),new CFileDroper( this->m_hWnd ,m_pageMaking->m_EditOutPath));
	RegisterDragDrop(m_pageResult->m_EditMusic->GetSwnd(),new CFileDroper( this->m_hWnd ,m_pageResult->m_EditMusic));
	RegisterDragDrop(m_pageResult->m_EditLyric->GetSwnd(),new CFileDroper( this->m_hWnd ,m_pageResult->m_EditLyric));
	RegisterDragDrop(m_pageSearchLyric->m_editLrcLyricPath->GetSwnd(),new CFileDroper( this->m_hWnd ,m_pageSearchLyric->m_editLrcLyricPath));
	RegisterDragDrop(m_pageSearchLyric->m_editOriginLyricPath->GetSwnd(),new CFileDroper( this->m_hWnd ,m_pageSearchLyric->m_editOriginLyricPath));
	

	//初始化“本软件”页面

	//版本号设置
	SWindow* txt_version = FindChildByName2<SWindow>("txt_version_number");
	SASSERT(txt_version != NULL);
	if(txt_version != NULL)
		txt_version->SetWindowTextW(SStringW(VERSION_NUMBER.c_str()));

}


//初始化etc文件并清理相关文件
void CMainDlg::initFloderAndFile()
{
	//在后来新的版本中，由于创建文件的地方越来越多，因此决定将在运行目录下创建 etc文件夹，用于储存各种配置文件 或 临时文件

	wstring wstrEtcFloder = FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC;
	wstring wstrOldSettingPath = FileHelper::GetCurrentDirectoryStr() +SETTING_FILE_NAME;

	if(!FileHelper::CheckFolderExist(wstrEtcFloder)) //etc 文件不存在， 认为是较旧版本 或 第一次运行等，需要创建转移配置文件
	{
		if(RET_SUCCEEDED != _wmkdir(wstrEtcFloder.c_str()))
		{
			wstring strTip = L"程序无法创建目录：\\n";
			strTip += wstrEtcFloder +L"\\n";
			strTip += L"这将导致设置、自动升级、ncm 歌曲ID匹配等功能异常，请尝试更换软件存放目录试图解决本问题\\n";
			
			_MessageBox(NULL, strTip.c_str(), L"提示", MB_OK|MB_ICONWARNING);
			return;
		}

		//将设置文件复制 到 etc 文件夹下面
		wstring wstrNewSettingPath = FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" +SETTING_FILE_NAME;
		if(FileHelper::CheckFileExist(wstrOldSettingPath))
		{
			CopyFile(wstrOldSettingPath.c_str(), wstrNewSettingPath.c_str(), TRUE);//复制旧的配置到新的配置位置
			m_settingPage.LoadSetting(); //再次载入一次配置
		}
	}

	//清除之前升级残留的文件
	vector<wstring> vecFiles;
	FileHelper::FindAllFiles( FileHelper::GetCurrentDirectoryStr().c_str(), vecFiles, false);
	for(auto iter = vecFiles.begin(); iter != vecFiles.end(); iter++)
	{
		auto index = iter->find_last_of(L'.');
		if(index == wstring::npos)
			continue;

		wstring subStr = iter->substr(index+1);

		if(subStr.size()==0)
			continue;

		//如果. 后面的子串都是数字，则认为是升级残留的文件，删除之
		bool bDelete = true;
		for(auto iterc = subStr.begin(); iterc != subStr.end(); iterc++)
		{
			if(!(*iterc >= L'0' && *iterc <= L'9'))
			{
				bDelete = false;
				break;
			}
		}

		if(bDelete)
			DeleteFile(iter->c_str()); 
	}

	//删除旧的配置
	if(FileHelper::CheckFileExist(wstrOldSettingPath))
		DeleteFile(wstrOldSettingPath.c_str()); 

	//删除旧的 temp ,version 文件
	wstring strVersionFile = FileHelper::GetCurrentDirectoryStr()+ FILE_NAME_LAST_VERSION_INFO;
	wstring strTempFile =  FileHelper::GetCurrentDirectoryStr()+ FILE_NAME_TEMP;
	
	if(FileHelper::CheckFileExist(strVersionFile))
		DeleteFile(strVersionFile.c_str()); 
	if(FileHelper::CheckFileExist(strTempFile))
		DeleteFile(strTempFile.c_str()); 

	//在新的更新系统中，第N次启动更新的文件被替换的旧版本文件，需要在N+1启动时清除
	wstring strFileToDelete = wstrEtcFloder+L"\\fileToDelete";
	if(FileHelper::CheckFileExist(strFileToDelete))
	{
		vector<SStringW> vecLines;
		FileOperator::ReadAllLinesW(strFileToDelete, &vecLines);
		wstring tempPath;
		for(auto iter = vecLines.begin(); iter != vecLines.end(); iter++){
			tempPath = FileHelper::GetCurrentDirectoryStr() + iter->GetBuffer(1);
			DeleteFile(tempPath.c_str()); 
		}

		DeleteFile(strFileToDelete.c_str()); 
	}
}

//初始化桌面歌词
void CMainDlg::initDesktopLyric()
{
	m_wndDesktopLyric = new DlgDesktopLyric(_T("layout:xml_dlg_lyric"));
	SASSERT(m_wndDesktopLyric && "分配桌面歌词对象内存失败");

	m_wndDesktopLyric->CreateAndInitWindow();

	//以指定 吸附模式 和 对齐方式 添加为 吸附于主窗口的子窗口
    CMagnetFrame::ATTACHMODE am  = AM_BOTTOM;
    CMagnetFrame::ATTACHALIGN aa = AA_LEFT;
    AddSubWnd(m_wndDesktopLyric->m_hWnd, am,aa);
}

//主页面切换时
void CMainDlg::OnPageChanged(EventArgs *pEvt)
{
	STabCtrl* pageTab = (STabCtrl*)pEvt->sender;
	int cur_sel = pageTab->GetCurSel();
	if(cur_sel == 0)//“歌词制作页面”
	{
		//填充默认输出路径
		if(FileHelper::CheckFolderExist(m_settingPage.m_default_output_path))
		{
			SWindow *m_szOutputPath = FindChildByID(R.id.edit_output);
			m_szOutputPath->SetWindowTextW(m_settingPage.m_default_output_path.c_str());

			m_pageMaking->PATH_STATE_1[2].isInited = true;
			//加入歌词制作器
			maker.setm_szOutputPath(m_settingPage.m_default_output_path.c_str());
		}
	}
}


int CMainDlg::MsgDropFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	OnDropFile((SEdit*)wParam,(WCHAR*)lParam);
	return 0;
}

//在拖动文件到 Edit控件时
void CMainDlg::OnDropFile(SEdit* pEdit, wstring strFilePath)
{
	if(pEdit == this->m_pageMaking->m_EditMusic)
	{
		m_pageMaking->OnBtnSelectMusic1(strFilePath.c_str());
	}
	else if(pEdit == this->m_pageMaking->m_EditLyric)
	{
		m_pageMaking->OnBtnSelectLyric1(strFilePath.c_str());
	}
	else if(pEdit == this->m_pageMaking->m_EditOutPath)
	{
		m_pageMaking->OnBtnSelectOutput(strFilePath.c_str());
	}
	else if(pEdit == this->m_pageResult->m_EditMusic)
	{
		m_pageResult->OnBtnSelectMusic2(strFilePath.c_str());
	}
	else if(pEdit == this->m_pageResult->m_EditLyric)
	{
		m_pageResult->OnBtnSelectLyric2(strFilePath.c_str());
	}
	else if(pEdit == this->m_pageSearchLyric->m_editOriginLyricPath)
	{
		m_pageSearchLyric->OnBtnSelectOriginLyricPath(strFilePath.c_str());
	}
	else if(pEdit == this->m_pageSearchLyric->m_editLrcLyricPath)
	{
		m_pageSearchLyric->OnBtnSelectLrcLyricPath(strFilePath.c_str());
	}
}

//TODO:消息映射
void CMainDlg::OnClose()
{
	CSimpleWnd::DestroyWindow();
}
void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}
void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	if (!m_bLayoutInited) return;
	
	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if(!pBtnMax || !pBtnRestore) return;
	
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}
//设置程序窗口的背景图片
void CMainDlg::setBackSkin()
{
    //SSkinImgList * pSkin = sobj_cast<SSkinImgList>(GETSKIN(_T("skin_bkimg")));
    //SASSERT(pSkin);
    //FindChildByID(R.id.img_skin_layer)->SetAttribute(L"skin",_T("skin_bkimg"));
	
    //COLORREF crAvg = SDIBHelper::CalcAvarageColor(pSkin->GetImage());
    //DoColorize(crAvg);
}


BOOL CMainDlg::PageMakingChainEvent(CPageMaking* pPageMaking, EventArgs* pEvt)
{
	if(m_pageMaking == NULL)
		return FALSE;

	return pPageMaking->_HandleEvent(pEvt);
}
BOOL CMainDlg::PageResultChainEvent(CPageResult* pPageResult,EventArgs* pEvt)
{
	if(pPageResult == NULL)
		return FALSE;

	return pPageResult->_HandleEvent(pEvt);
}
BOOL CMainDlg::PageSearchLyricChainEvent(CPageSearchLyric* pPageSearchLyric,EventArgs* pEvt)
{
	if(pPageSearchLyric == NULL)
		return FALSE;

	return pPageSearchLyric->_HandleEvent(pEvt);
}
BOOL CMainDlg::PageSearchNcmIDChainEvent(CPageSearchNcmID* pPageSearchNcmID,EventArgs* pEvt)
{
	if(pPageSearchNcmID == NULL)
		return FALSE;

	return pPageSearchNcmID->_HandleEvent(pEvt);
}

//键盘消息
void CMainDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//后来除了主要的页面，又拓展了一些额外附加功能页面，所以如果是在附加页面，不处理拦截按键
	STabCtrl *ContainerTab  = FindChildByID2<STabCtrl>(R.id.tab_content_container); 
	if(ContainerTab->GetCurSel() != 0)
	{
		SetMsgHandled(FALSE);
		return;
	}

	//接下来为主要页面的按键处理逻辑
	int curPage;

	//获取当前所处页面
	STabCtrl *mainTab = FindChildByID2<STabCtrl>(R.id.tab_main);
	if(mainTab)
		curPage = mainTab->GetCurSel();

	if(curPage == 0)//来自第一页
	{
		//不在制作歌词的状态中，则不响应按键的状态
		if(!this->m_bIsLyricMaking) return;
		switch(nChar)
		{
			case VK_UP:
				maker.m_nCurLine += 1;
			
				if(maker.m_nCurLine <= maker.m_nTotalLine)
				{
					//更新显示的歌词
					//第一次第一行没有数据
					if(maker.m_nCurLine != 1)
					{
						m_pageMaking->P1_Line1->SetWindowTextW(maker.GetOriginLyricAt(maker.m_nCurLine-1));
					}

					m_pageMaking->P1_Line2->SetWindowTextW(maker.GetOriginLyricAt(maker.m_nCurLine));
				
					if(maker.m_nCurLine != maker.m_nTotalLine)
					{
						m_pageMaking->P1_Line3->SetWindowTextW(maker.GetOriginLyricAt(maker.m_nCurLine+1));
					}
					else//最后行后面数据为空
					{
						m_pageMaking->P1_Line3->SetWindowTextW(_T(""));
					}

					//隐藏空行提示
					m_pageMaking->emptyTip1->SetVisible(FALSE,TRUE);

					//标记生成下一行歌词
					maker.markNextLine();

					break ;
				}
				//else 执行“left”空出一行的操作

			case VK_RIGHT:

				//更新页面显示
				if(maker.m_nCurLine == 0)
				{
					m_pageMaking->P1_Line1->SetWindowTextW(_T(""));
					m_pageMaking->P1_Line2->SetWindowTextW(_T(""));
				
					//显示空行提示
					m_pageMaking->emptyTip1->SetVisible(TRUE,TRUE);
				}
				else if(maker.m_nCurLine <= maker.m_nTotalLine )  
				{
					//更新显示面板的第一行和第二行

					m_pageMaking->P1_Line1->SetWindowTextW(maker.GetOriginLyricAt(maker.m_nCurLine));
					m_pageMaking->P1_Line2->SetWindowTextW(_T(""));

					//显示空行提示
					m_pageMaking->emptyTip1->SetVisible(TRUE,TRUE);

					//else 保持原来页面的显示
				}else if(maker.m_nCurLine == maker.m_nTotalLine+1)//此条件需要考虑到：在最后一行时按下VK_UP，则m_nCurLine m_nTotalLine +1
				{
					//更新显示面板的第一行和第二行
					m_pageMaking->P1_Line1->SetWindowTextW(maker.GetOriginLyricAt(maker.m_nCurLine-1));
					m_pageMaking->P1_Line2->SetWindowTextW(_T(""));
				
					//显示空行提示
					m_pageMaking->emptyTip1->SetVisible(TRUE,TRUE);
				}
			
				//更新插入数据
				maker.markSpaceLine();

					break;
			case VK_RETURN:
				{
				this->m_bIsLyricMaking = FALSE;

				//取得输出的文件名
				TCHAR outFileName[_MAX_FNAME];
				maker.getOutputFileName(outFileName);

				//提示用户是否保存
				int ret = _MessageBox(this->m_hWnd,SStringT().Format(_T("结束了制作!\\n\
																		是否保存到文件:\\n%s？ \
							"),outFileName),_T("保存提示"),MB_OKCANCEL|MB_ICONQUESTION);

				if(ret == IDOK)
				//如果确定保存，则保存入目标文件，并给出提示
				{
					maker.makingEnd();
				
					//取得输出的 路径文件名
					TCHAR outPathName[_MAX_FNAME];
					maker.getm_szOutputPathName(outPathName);

					//给出提示预览提示
					_MessageBox(this->m_hWnd,SStringT().Format(_T("歌词已保存到：\\n%s  \\n\
							效果预览方法：\\n点击本页面左下角的“效果预览”\\n\
							或 在左边切换页面“滚动预览” 自己选择文件。"),outPathName),_T("预览提示"),MB_OK|MB_ICONINFORMATION);
				}

					//重置状态
					m_pageMaking->backToInit_1();
			
					//改变预览按钮的状态
					SButton *btn_preview = FindChildByID2<SButton>(R.id.btn_preview);
					btn_preview->EnableWindow(TRUE,TRUE);
					SButton *btn_open_output = FindChildByID2<SButton>(R.id.btn_open_output);
					btn_open_output->EnableWindow(TRUE,TRUE);
				}
				break;
								
			case 'B'://回退5秒

				//后退十秒
				maker.m_musicPlayer.shift(-5*1000);
				maker.m_musicPlayer.playAfterSeek();

				//撤销 可能已经被标记的数据
				//重新计算得到当前行数 m_nCurLine
				maker.RecorrectLyricData();

				//更新界面的显示为撤销后的数据
				m_pageMaking->UpdataMakerLyricShowing();

				break;
			case VK_SPACE://暂停
				
				DWORD status = maker.m_musicPlayer.getModeStatus();
				if(status == MCI_MODE_PLAY)
				{
					maker.m_musicPlayer.pause();
				}
				else if(status == MCI_MODE_PAUSE)
				{
					maker.m_musicPlayer.resume();
				}

				break;
		}
	}
	else if(curPage == 1)//来自第二页
	{
		//不在预览歌词的状态中，则不响应按键的状态
		if(!this->m_bIsLyricPlaying) return;
		switch(nChar)
		{
			case VK_LEFT:
				//后退十秒
				player.m_musicPlayer.shift(-10*1000);
				player.m_musicPlayer.playAfterSeek();

				//由于本程序的歌词显示的原理是 在每次定时器触发时，遍历每一行歌词，只有在找到第一个 时间比当前播放位置的时间值大
				//的一行时，才更新当前行 player.m_nCurLine 的值，所以回退到最开始的位置时，Timer循环中歌词的显示不会复位为初始状态，
				//故在此做此处理
				if(player.isCurrentZeroLine())
				{
					//清空显示面板，显示第一条非空歌词
					m_pageResult->P2_Line1->SetWindowTextW(_T(""));
					m_pageResult->P2_Line2->SetWindowTextW(_T(""));
					m_pageResult->P2_Line3->SetWindowTextW(player.m_vLineInfo[0].m_strLine);

					//隐藏空行提示
					m_pageResult->emptyTip2->SetVisible(FALSE,TRUE);
				}
				
				break;
			case VK_RIGHT:
				//前进十秒

				player.m_musicPlayer.shift(10*1000);
				player.m_musicPlayer.playAfterSeek();

				break;
			case VK_SPACE:
				
				DWORD status = player.m_musicPlayer.getModeStatus();
				if(status == MCI_MODE_PLAY)
				{
					player.m_musicPlayer.pause();
				}
				else if(status == MCI_MODE_PAUSE)
				{
					player.m_musicPlayer.resume();
				}

				break;
		}
	}
	else
	{
		SetMsgHandled(FALSE);
	}

}



//接收音乐消息
//参数说明：https://msdn.microsoft.com/en-us/library/dd757358(v=vs.85).aspx
//wParam = (WPARAM) wFlags 
//lParam = (LONG) lDevID
void CMainDlg::OnMusicCommand(UINT lParam, UINT wParam)
{
	switch(wParam)
	{
	case MCI_NOTIFY_ABORTED:	//The device received a command that prevented the current conditions for initiating the callback function from being met. If a new command interrupts the current command and it also requests notification, the device sends this message only and not MCI_NOTIFY_SUPERSEDED
		//MB(_T("MCI_NOTIFY_ABORTED"));
		//播放中的音乐被stop时会发送该消息

		//不做处理
		break;
	case MCI_NOTIFY_FAILURE	://A device error occurred while the device was executing the command.
		MB(_T("MCI_NOTIFY_FAILURE"));
		break;
	case MCI_NOTIFY_SUCCESSFUL:	//The conditions initiating the callback function have been met.
		//MB(_T("MCI_NOTIFY_SUCCESSFUL"));
		//成功执行完动作后会发送该消息

		//FootPrintPage 里保留着最后播放时所在的页面；由此决定当前 音乐结束是来自哪个页面

		if(FootPrintPage == 0) //第1页（来自制作歌词页面）
		{
			//发送回车按键消息
			SendMessage(WM_KEYDOWN, VK_RETURN );  //本程序只处理 wParam 参数，wParam置为 VK_RETURN
		}
		else if(FootPrintPage == 1) //第1页（来自制作歌词页面）
		{
			//发送 “结束预览”时所触发的事件
			SButton* btn = FindChildByID2<SButton>(R.id.btn_end_preview);//这一句不是必要的，且当结束时刻页面不在第二页则会获取失败
			SOUI::EventCmd Evt(btn);				//初始化EventCmd需要一个参数
			Evt.idFrom = R.id.btn_end_preview;		//在这里只要事件对象是EventCmd，idFrom是按钮id，即可模拟调用按钮响应函数

			//自己调用 soui 消息处理函数
			this->_HandleEvent(&Evt);

			//如果单曲循环，则重新播放
			if(this->m_pageResult->m_bSingleCycle)
			{
				//发送 “播放按钮”触发的事件
				SButton* btn = FindChildByID2<SButton>(R.id.btn_start_playing);//这一句不是必要的，且当结束时刻页面不在第二页则会获取失败
				SOUI::EventCmd Evt(btn);				//初始化EventCmd需要一个参数
				Evt.idFrom = R.id.btn_start_playing;	//在这里只要事件对象是EventCmd，idFrom是按钮id，即可模拟调用按钮响应函数
							
				//自己调用 soui 消息处理函数
				this->_HandleEvent(&Evt);
			}
		}

		break;
	case MCI_NOTIFY_SUPERSEDED: //The device received another command with the "notify" flag set and the current conditions for initiating the callback function have been superseded.
		MB(_T("MCI_NOTIFY_SUPERSEDED"));
		break;
	}
}


//接收定时器消息，用于滚动歌词动画的实现
void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	static int curMSecond, totalLength;
	static int minute,ms; 
	static double second;
	static TCHAR timeBuf[20];
	SStatic* timeLabel_1 = FindChildByID2<SStatic>(R.id.text_time_1);
	SProgress* timeProgress_1 = FindChildByID2<SProgress>(R.id.progress_music_1);
	SStatic* timeLabel_2 = FindChildByID2<SStatic>(R.id.text_time_2);
	SProgress* timeProgress_2 = FindChildByID2<SProgress>(R.id.progress_music_2);

	STabCtrl* tab = FindChildByID2<STabCtrl>(R.id.tab_main);

	switch(nIDEvent)
	{
	case 101:
		if(tab)
			if(tab->GetCurSel() == 0) //在歌词制作页面
			{
				if(this->m_bIsLyricMaking)
				{
					curMSecond = maker.m_musicPlayer.getPosition();  //毫秒时间
				
					//更新页面时间显示标签
					minute = (curMSecond)/60000;  
					ms  = (curMSecond)%60000;//由于歌词的内容的显示 提前了 MUSIC_DIVICE_DELAY 毫秒，虽然看上去滚动不会有延迟了，但是暂停时却暴
														 //露了这种 MUSIC_DIVICE_DELAY 毫秒 的差距（已显示歌词却时间没跟上），故在此加  MUSIC_DIVICE_DELAY 毫秒 显示
					second = ms *1.0/1000;
					_stprintf(timeBuf,_T("[%02d:%06.3lf]"),minute,second);

					if(timeLabel_1)
						timeLabel_1->SetWindowTextW(timeBuf);
	
					//更新进度条的显示
					if(timeProgress_1)
						timeProgress_1->SetValue( int(curMSecond * 1.0 / maker.m_musicPlayer.getLength() * 1000) ); //设置千分数值
				}

			}
	case 102:
		//if(tab)
		//	if(tab->GetCurSel() == 1) //在歌词预览的页面
		//	{
				if(this->m_bIsLyricPlaying)
				{
					curMSecond = player.m_musicPlayer.getPosition();  //毫秒时间

					player.updateCurLine(); //如果快进或者后退都会导致，当前行发生变化，故需要先更新再判断

					if(player.m_nCurLine==0)//下面循环需要m_nCurLine从1开始
						player.m_nCurLine=1;

					while(player.m_nCurLine <= player.m_nTotalLine ) //准备循环读取所有未被 显示处理的行
					{
						if(player.m_vLineInfo[player.m_nCurLine -1 ].m_nmSesonds -MUSIC_DIVICE_DELAY > curMSecond  ) //设备访问会有延迟 故提前 MUSIC_DIVICE_DELAY 毫秒显示歌词
							break;	//歌词时间 大于 实际经过时间，还不用处理，跳出处理循环
					
						//执行面板滚动到 m_nCurLine
						m_pageResult->scrollToLyricCurLine();

						//更新桌面歌词里的歌词内容
						m_wndDesktopLyric->SetCurrentLyric(player.m_vLineInfo[player.m_nCurLine -1 ].m_strLine);

						//滚动完毕，自增一行
						player.m_nCurLine ++;
					}

					//更新页面时间显示标签
					minute = (curMSecond+ MUSIC_DIVICE_DELAY )/60000;  
					ms  = (curMSecond+ MUSIC_DIVICE_DELAY )%60000;//由于歌词的内容的显示 提前了 MUSIC_DIVICE_DELAY 毫秒，虽然看上去滚动不会有延迟了，但是暂停时却暴
														 //露了这种 MUSIC_DIVICE_DELAY 毫秒 的差距（已显示歌词却时间没跟上），故在此加  MUSIC_DIVICE_DELAY 毫秒 显示
					second = ms *1.0/1000;
					_stprintf(timeBuf,_T("[%02d:%06.3lf]"),minute,second);

					if(timeLabel_2)
						timeLabel_2->SetWindowTextW(timeBuf);
	
					//更新进度条的显示
					if(timeProgress_2)
						timeProgress_2->SetValue( int(curMSecond * 1.0 / player.m_musicPlayer.getLength() * 1000) );//设置千分数值
				}

			//}
		break;
	default:
		SetMsgHandled(FALSE);
		//SHostWnd::OnTimer(nIDEvent);
	}
}


//实际相应操作
void CMainDlg::OnBtnSoundOpen()
{
	this->m_nVolumnBeforeMute = this->m_nVolumn;

	//设置音乐播放器的音量
	maker.m_musicPlayer.setVolumn(0);
	player.m_musicPlayer.setVolumn(0);

	// 更新两个页面的静音按钮的显示状态
	SButton * btnOpen_1 = FindChildByID2<SButton>(R.id.btn_sound_open_1);
	SButton * btnOpen_2 = FindChildByID2<SButton>(R.id.btn_sound_open_2);
	SButton * btnClose_1 = FindChildByID2<SButton>(R.id.btn_sound_close_1);
	SButton * btnClose_2 = FindChildByID2<SButton>(R.id.btn_sound_close_2);

	if(btnOpen_1)
		btnOpen_1->SetVisible(FALSE,TRUE);
	if(btnOpen_2)
		btnOpen_2->SetVisible(FALSE,TRUE);
	if(btnClose_1)
		btnClose_1->SetVisible(TRUE,TRUE);
	if(btnClose_2)
		btnClose_2->SetVisible(TRUE,TRUE);
	
	//更新音量sound bar的位置显示状态
	SSliderBar* bar1 = FindChildByID2<SSliderBar>(R.id.slider_sound_1);
	if(bar1)
		bar1->SetValue(0);
	SSliderBar* bar2 = FindChildByID2<SSliderBar>(R.id.slider_sound_2);
	if(bar2)
		bar2->SetValue(0);
}

void CMainDlg::OnBtnSoundClose()
{
	//设置音乐播放器的音量
	maker.m_musicPlayer.setVolumn(this->m_nVolumn);
	player.m_musicPlayer.setVolumn(this->m_nVolumn);

	// 更新两个页面的静音按钮的显示状态
	SButton * btnOpen_1 = FindChildByID2<SButton>(R.id.btn_sound_open_1);
	SButton * btnOpen_2 = FindChildByID2<SButton>(R.id.btn_sound_open_2);
	SButton * btnClose_1 = FindChildByID2<SButton>(R.id.btn_sound_close_1);
	SButton * btnClose_2 = FindChildByID2<SButton>(R.id.btn_sound_close_2);

	if(btnOpen_1)
		btnOpen_1->SetVisible(TRUE,TRUE);
	if(btnOpen_2)
		btnOpen_2->SetVisible(TRUE,TRUE);
	if(btnClose_1)
		btnClose_1->SetVisible(FALSE,TRUE);
	if(btnClose_2)
		btnClose_2->SetVisible(FALSE,TRUE);

	//更新音量sound bar的位置显示状态
	SSliderBar* bar1 = FindChildByID2<SSliderBar>(R.id.slider_sound_1);
	if(bar1)
		bar1->SetValue(this->m_nVolumnBeforeMute * 100 /1000);
	SSliderBar* bar2 = FindChildByID2<SSliderBar>(R.id.slider_sound_2);
	if(bar2)
		bar2->SetValue(this->m_nVolumnBeforeMute  * 100 /1000);

	//恢复声音的值
	maker.m_musicPlayer.setVolumn(this->m_nVolumnBeforeMute);
	this->m_nVolumn = player.m_musicPlayer.setVolumn(this->m_nVolumnBeforeMute);
}




void  CMainDlg::OnSliderPos(bool isPos1)
{
	int volumn = this->m_nVolumn;
	int value = this->m_nVolumn * 100 / 1000;

	//同步两个音量条的显示
	if(isPos1)
	{
		//获得  slider_sound_1 的值，决定设置的音量
		SSliderBar* bar1 = FindChildByID2<SSliderBar>(R.id.slider_sound_1);
		if(bar1)
			 value = bar1->GetValue();

		//更新 slider_sound_2 的位置
		SSliderBar* bar2 = FindChildByID2<SSliderBar>(R.id.slider_sound_2);
		if(bar2)
			  bar2->SetValue(value);
	}
	else
	{
		//获得  slider_sound_2 的值，决定设置的音量
		SSliderBar* bar2 = FindChildByID2<SSliderBar>(R.id.slider_sound_2);
		if(bar2)
			 value = bar2->GetValue();

		//更新 slider_sound_1 的位置
		SSliderBar* bar1 = FindChildByID2<SSliderBar>(R.id.slider_sound_1);
		if(bar1)
			  bar1->SetValue(value);
	}
	
	//两个音乐播放器都更新音量
	volumn = value  * 1000 / 100;
	maker.m_musicPlayer.setVolumn(volumn);
	this->m_nVolumn = player.m_musicPlayer.setVolumn(volumn);
	
	//根据当前音量是否为0，改变静音按钮的显示
	if(value==0)
	{
		// 更新两个页面的静音按钮的显示状态
		SButton * btnOpen_1 = FindChildByID2<SButton>(R.id.btn_sound_open_1);
		SButton * btnOpen_2 = FindChildByID2<SButton>(R.id.btn_sound_open_2);
		SButton * btnClose_1 = FindChildByID2<SButton>(R.id.btn_sound_close_1);
		SButton * btnClose_2 = FindChildByID2<SButton>(R.id.btn_sound_close_2);

		if(btnOpen_1)
			btnOpen_1->SetVisible(FALSE,TRUE);
		if(btnOpen_2)
			btnOpen_2->SetVisible(FALSE,TRUE);
		if(btnClose_1)
			btnClose_1->SetVisible(TRUE,TRUE);
		if(btnClose_2)
			btnClose_2->SetVisible(TRUE,TRUE);
	}
	else
	{
		// 更新两个页面的静音按钮的显示状态
		SButton * btnOpen_1 = FindChildByID2<SButton>(R.id.btn_sound_open_1);
		SButton * btnOpen_2 = FindChildByID2<SButton>(R.id.btn_sound_open_2);
		SButton * btnClose_1 = FindChildByID2<SButton>(R.id.btn_sound_close_1);
		SButton * btnClose_2 = FindChildByID2<SButton>(R.id.btn_sound_close_2);

		if(btnOpen_1)
			btnOpen_1->SetVisible(TRUE,TRUE);
		if(btnOpen_2)
			btnOpen_2->SetVisible(TRUE,TRUE);
		if(btnClose_1)
			btnClose_1->SetVisible(FALSE,TRUE);
		if(btnClose_2)
			btnClose_2->SetVisible(FALSE,TRUE);
	}
}

//用于处理格式转换线程结束后，通知主线程播放
int CMainDlg::MessageButtonCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if(uMsg == MSG_USER_MAKING_START_BUTTON)
	{
		m_pageMaking->OnBtnStartMaking();
	}
	else if(uMsg == MSG_USER_PLAYING_START_BUTTON)
	{
		m_pageResult->OnBtnStartPlaying();
	}

	return TRUE;
}


//处理消息，显示搜索到的歌词
int CMainDlg::MessageShowLyricResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	LyricSearchResult* pResult = (LyricSearchResult*)wParam;

	m_pageSearchLyric->ShowLyricResult(pResult);

	delete pResult;
	return TRUE;
}


//处理消息，显示搜索到的ID
int CMainDlg::MessageShowIDResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	IDSearchResult* pResult = (IDSearchResult*)wParam;

	m_pageSearchNcmID->ShowIDResult(pResult);

	delete pResult;
	return TRUE;
}


int CMainDlg::MessageSearchWithGuessResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	SongInfoGuessResult* pGuessRes  = (SongInfoGuessResult*)wParam;

	if(pGuessRes->nResultType == 3)
	{
		_MessageBox(this->m_hWnd,L"无法猜测出该文件的具体信息，请自行搜索歌词",L"提示", MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		//填充收拾信息，并开始搜索
		m_pageSearchLyric->OnSearchWithGuess(pGuessRes);

		//自动切换到搜索歌词页面
		STabCtrl* tab = FindChildByID2<STabCtrl>(R.id.tab_main);
		if(tab)
			tab->SetCurSel(2);
	}

	delete pGuessRes;
	return TRUE;
}

int CMainDlg::MessageSearchIDWithGuessResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	SongInfoGuessResult* pGuessRes  = (SongInfoGuessResult*)wParam;

	if(pGuessRes->nResultType == 3)
	{
		_MessageBox(this->m_hWnd,L"无法猜测出该文件的具体信息，请自行搜索ID",L"提示", MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		//填充收拾信息，并开始搜索
		m_pageSearchNcmID->OnSearchWithGuess(pGuessRes);

		//自动切换到搜索ID页面
		STabCtrl* tab = FindChildByID2<STabCtrl>(R.id.tab_content_container);
		if(tab)
			tab->SetCurSel(1);
	}

	delete pGuessRes;
	return TRUE;
}