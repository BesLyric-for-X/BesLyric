#include "stdafx.h"
#include "PageMaking.h"
#include "utility/WinDialog.h"
#include "utility/WinFile.h"
#include "entity\GuessLyricInfoThread.h"
#include "entity\NcmIDManager.h"
#include "DlgDownloadNcmMp3.h"

CPageMaking::CPageMaking()
{
	wcscpy_s(PATH_STATE_1[0].nameOfPath, L"音乐文件");
	wcscpy_s(PATH_STATE_1[1].nameOfPath, L"歌词文件");
	wcscpy_s(PATH_STATE_1[2].nameOfPath, L"输出路径");
	PATH_STATE_1[0].isInited = false;
	PATH_STATE_1[1].isInited = false;
	PATH_STATE_1[2].isInited = false;
	
	P1_Line1 = NULL;
	P1_Line2 = NULL;
	P1_Line3 = NULL;
	m_wndHighlight = NULL;
	
	emptyTip1= NULL;

	m_EditMusic   = NULL;
	m_EditLyric   = NULL;
	m_EditOutPath = NULL;

	m_txtMusic = NULL;
	m_txtLyric = NULL;
	
	m_txtTime = NULL;
	m_ProgressTime = NULL;

	m_btnStart = NULL;
	m_btnPreview = NULL;
	m_btnOpenOutput = NULL;
	m_btnLoad = NULL;
	m_btnRestart= NULL;

	m_btnMatchNcmID = NULL;
}

//初始化设置页面
void CPageMaking::Init(SHostWnd *pMainWnd)
{
	SASSERT(NULL != pMainWnd);

	//保存主窗口对象
	m_pMainWnd = pMainWnd;

	//获得代码中常用的控件的指针
	P1_Line1 = M()->FindChildByID2<SStatic>(R.id.name_1_line_1);
	SASSERT(P1_Line1 != NULL);
	P1_Line2 = M()->FindChildByID2<SStatic>(65000);
	SASSERT(P1_Line2 != NULL);
	P1_Line3 = M()->FindChildByID2<SStatic>(R.id.name_1_line_3);
	SASSERT(P1_Line3 != NULL);
	m_wndHighlight = m_pMainWnd->FindChildByID(R.id.highlight_bkgnd);
	SASSERT(m_wndHighlight != NULL);

	emptyTip1 = M()->FindChildByID2<SStatic>(R.id.empty_tip_block);
	SASSERT(emptyTip1 != NULL);

	
	m_EditMusic = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_music_1);
	SASSERT(m_EditMusic != NULL);
	m_EditLyric = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lyric_1);
	SASSERT(m_EditLyric != NULL);
	m_EditOutPath = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_output);
	SASSERT(m_EditOutPath != NULL);

	m_txtMusic = m_pMainWnd->FindChildByID2<SStatic>(R.id.line_music_1);
	SASSERT(m_txtMusic != NULL);
	m_txtLyric = m_pMainWnd->FindChildByID2<SStatic>(R.id.line_lyric_1);
	SASSERT(m_txtLyric != NULL);

	m_txtTime = m_pMainWnd->FindChildByID2<SStatic>(R.id.text_time_1);
	SASSERT(m_txtTime != NULL);
	m_ProgressTime = m_pMainWnd->FindChildByID2<SProgress>(R.id.progress_music_1);
	SASSERT(m_ProgressTime != NULL);

	m_btnStart = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_start_making);
	SASSERT(m_btnStart != NULL);
	m_btnPreview = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_preview);
	SASSERT(m_btnPreview != NULL);
	m_btnOpenOutput = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_open_output);
	SASSERT(m_btnOpenOutput != NULL);
	m_btnLoad = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_load_1);
	SASSERT(m_btnLoad != NULL);
	m_btnRestart = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_restart);
	SASSERT(m_btnRestart != NULL);

	m_btnMatchNcmID = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_match_ncm_id);
	SASSERT(m_btnMatchNcmID != NULL);
}

//获得主窗口对象
CMainDlg* CPageMaking::M()
{
	return (CMainDlg*)m_pMainWnd;
}

/*
*	歌词制作页面的响应函数
*/

//三个路径的选择
void CPageMaking::OnBtnSelectMusic1(LPCWSTR pFilePath)
{
	BOOL bRet = TRUE;
	LPCWSTR pPath = NULL;

	if(pFilePath == NULL)
	{
		CBrowseDlg Browser;
		bRet = Browser.DoFileBrowse(
			::GetActiveWindow(),
			L"音频文件(mp3,mp2,wma,wav,wv,ape,flac),网易云音乐ncm文件\0*.mp3;*.mp2;*.wma;*.wav;*.wv;*.ape;*.flac;*.ncm\0手机录音(amr)/手机铃声(mmf)\0*amr;*.mmf\0\0",
			FileHelper::CheckFolderExist(M()->m_settingPage.m_default_music_path)? M()->m_settingPage.m_default_music_path.c_str():nullptr
			);

		if(bRet == TRUE)
			pPath = Browser.GetFilePath();
	}
	else
		pPath = pFilePath;

	if(bRet == TRUE)
	{
		if( CFileDialogEx::checkPathName(_T("*.mp3"),pPath) 
		 || CFileDialogEx::checkPathName(_T("*.mp2"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.wma"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.wav"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.wv"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.ape"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.flac"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.amr"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.mmf"),pPath)
		 || CFileDialogEx::checkPathName(_T("*.ncm"),pPath))
		{	
			; //使用“或”条件判断( || )而 不用“且”条件判断（&&），以减少 checkPathName 调用的次数

			if(CFileDialogEx::checkPathName(_T("*.ncm"),pPath))
				m_btnMatchNcmID->SetVisible(TRUE,TRUE);
			else
				m_btnMatchNcmID->SetVisible(FALSE,TRUE);
		}
		else 
		{
			_MessageBox(M()->m_hWnd,_T("格式不支持\\n请确定文件格式为【选择对话框指定的文件类型】"),_T("提示"),MB_OK|MB_ICONINFORMATION);
			return;
		}

		//显示新选择的文件
		m_EditMusic->SetWindowTextW(pPath);

		PATH_STATE_1[0].isInited = true;

		//加入歌词制作器
		M()->maker.setMusicPath(pPath,M()->m_hWnd);
	}
}

void CPageMaking::OnBtnSelectLyric1(LPCWSTR pFilePath)
{
	BOOL bRet = TRUE;
	LPCWSTR pPath = NULL;

	if(pFilePath == NULL)
	{
		if(bRet == TRUE)
			pPath = pPath;
		CBrowseDlg Browser;
		bRet = Browser.DoFileBrowse(
			::GetActiveWindow(),
			L"文本文件(*.txt)\0*.txt\0\0",
			FileHelper::CheckFolderExist(M()->m_settingPage.m_default_lyric_path)? M()->m_settingPage.m_default_lyric_path.c_str():nullptr
			);
		
		if(bRet == TRUE)
			pPath = Browser.GetFilePath();
	}
	else
		pPath = pFilePath;

	if(bRet == TRUE)
	{
		if(!CFileDialogEx::checkPathName(_T("*.txt"),pPath))
		{
			_MessageBox(M()->m_hWnd,_T("格式不支持\\n请确定文件格式为【*.txt】"),_T("提示"),MB_OK|MB_ICONINFORMATION);
			return;
		}
		
		//显示新选择的文件
		m_EditLyric->SetWindowTextW(pPath);
		
		PATH_STATE_1[1].isInited = true;
		
		//加入歌词制作器
		M()->maker.setLyricPath(pPath);
	}
}

void CPageMaking::OnBtnSelectOutput(LPCWSTR pDirPath)
{
	BOOL bRet = TRUE;
	LPCWSTR pPath = NULL;

	if(pDirPath == NULL)
	{
		CBrowseDlg Browser;
		bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("选择 【LRC歌词文件】 默认输出路径"), TRUE,M()->m_settingPage.m_default_output_path.c_str());
			
		if(bRet == TRUE)
			pPath = Browser.GetDirPath();
	}
	else
		pPath = pDirPath;

	if(bRet == TRUE)
	{
		if(!CFileDialogEx::checkPathName(_T(".."),pPath))
		{
			_MessageBox(M()->m_hWnd,_T("格式不支持\\n请确定您选择【文件夹】"),_T("提示"),MB_OK|MB_ICONINFORMATION);
			return;
		}

		//显示新选择的文件
		m_EditOutPath->SetWindowTextW(pPath);
		
		PATH_STATE_1[2].isInited = true;
		
		//加入歌词制作器
		M()->maker.setm_szOutputPath(pPath);
	}
    
}

//制作歌词：加载 (初始化 LyricMaker)
void CPageMaking::OnBtnLoad1()
{
	//确保三个路径的数据都已经初始化
	if(!isPathReady_1())
	{
		//CMainDlg::getPathNotReady_1() 此时返回的正是 未初始化的路径 在 PATH_STATE 中对应的下标
		_MessageBox(M()->m_hWnd,SStringT().Format(_T("您还没选择【%s】"),PATH_STATE_1[getPathNotReady_1()].nameOfPath),
			_T("提示"),MB_OK|MB_ICONASTERISK);
		return;
	}
	
	//显示的音乐路径
	wstring FilePathToShow = M()->maker.m_szMusicPathName;

	//载入前处理ncm 文件的情况
	bool bIsCurrentNcm = false;
	wstring strID = L"";
	wstring strName =L"";
	wstring strMp3FullPath = L"";
	int nRet = DealWithNcmFile(bIsCurrentNcm, strID, strName, strMp3FullPath);
	if(nRet > 0) //为ncm 文件的情况
	{
		if(nRet == 1) //未匹配ncm 的ID
		{
			_MessageBox(M()->m_hWnd,SStringT().Format(_T("还没有为文件名【%s】匹配网易云音乐ID\\n请先点击“匹配ID”按钮匹配"),strName.c_str()),
			_T("提示"),MB_OK|MB_ICONINFORMATION);
			return;
		}
		else if(nRet == 2) //网络连接下载mp3失败
		{
			_MessageBox(M()->m_hWnd,SStringT().Format(_T("网络连接失败，无法为您下载ncm文件对应的mp3"),M()->maker.m_szLyricPathName),
			_T("提示"),MB_OK|MB_ICONINFORMATION);
			return;
		}
		else //==3
		{
			FilePathToShow = M()->maker.m_szMusicPathName + wstring(L" ( ID: ")+ strID + L" )";
			M()->maker.setMusicPath(strMp3FullPath.c_str(), M()->m_hWnd);
		}
	}
	else if(nRet < 0)
	{
		//创建文件夹失败
		return;
	}

	//确保 歌词文件 和 音乐文件有效
	File lyricFile(M()->maker.m_szLyricPathName,_T("r"));

	if(!lyricFile.isValidFile()) 
	{
		_MessageBox(M()->m_hWnd,SStringT().Format(_T("文件打开失败:\\n【%s】\\n!请确保文件有效"),M()->maker.m_szLyricPathName),
			_T("失败提示"),MB_OK|MB_ICONWARNING);
		return;
	}

	//更新 页面的 当前音乐 和 当前歌词的信息
	m_txtMusic->SetWindowTextW(FilePathToShow.c_str());
	m_txtLyric->SetWindowTextW(M()->maker.m_szLyricPathName);
	
	//重置 LyricMaker的 歌词数据为空
	M()->maker.reloadMaker();

	//从文件读取每一行的歌词
	M()->maker.m_vLyricOrigin = M()->maker.getLyricOrigin(lyricFile);
	M()->maker.m_vLyricOriginWithEmptyLine = M()->maker.m_vLyricOrigin; //初始化 m_vLyricOriginWithEmptyLine
	 
	//读取完毕，清空显示面板，显示第一条非空歌词
	P1_Line1->SetWindowTextW(_T(""));
	P1_Line2->SetWindowTextW(_T(""));
	P1_Line3->SetWindowTextW(M()->maker.m_vLyricOriginWithEmptyLine[0]);

	//改变按钮的状态
	m_btnStart->EnableWindow(TRUE,TRUE); //第一个参数bEnable为 是否启用，第二个参数bUpdate为是否在改变状态后更新显示
	//m_btnStart->Invalidate();			//如果没有指定第二个参数bUpdate，其默认值为 FALSE，要刷新则需要自己Invalidate()
	
	//改变预览按钮的状态
	m_btnPreview->EnableWindow(FALSE,TRUE);
	m_btnOpenOutput->EnableWindow(FALSE,TRUE);
}

//播放预览刚刚制作完成的带时间时间轴歌词
void CPageMaking::OnBtnPreview()
{
	//切换到预览页面
	STabCtrl* tab = M()->FindChildByID2<STabCtrl>(R.id.tab_main);
	if(tab)
		tab->SetCurSel(1);

	Sleep(500);

	//设置player的状态
	M()->player.setMusicPath(M()->maker.m_szMusicPathName,M()->m_hWnd);
	M()->player.setLyricPath(M()->maker.m_szOutputPathName);
	
	M()->m_pageResult->PATH_STATE_2[0].isInited = true;
	M()->m_pageResult->PATH_STATE_2[1].isInited = true;

	//设置页面 路径名，和当前选择的音乐和歌词（“载入准备”按下之后的状态）
	//显示新选择的文件
	M()->m_pageResult->m_EditMusic->SetWindowTextW(M()->player.m_szMusicPathName);
	//显示新选择的文件
	M()->m_pageResult->m_EditLyric->SetWindowTextW(M()->player.m_szLyricPathName);


	M()->m_pageResult->OnBtnLoad2();
}

//打开生成的文件
void CPageMaking::OnBtnOpenOutput()
{
	if(_tcslen(M()->maker.m_szOutputPath)!=0)
	{
		//打开文件夹
		ShellExecute(NULL,L"explore", M()->maker.m_szOutputPath,NULL, NULL,SW_SHOWNORMAL);
			
		//用window默认编辑器打开歌词文件
		ShellExecute(NULL,L"open",L"notepad", M()->maker.m_szOutputPathName,NULL,SW_SHOWNORMAL);
	}
}

//试图搜索歌词
void CPageMaking::OnBtnTrySearchLyric()
{
	if(_tcslen(M()->maker.m_szMusicPathName)!=0)
	{	
		CGuessLyricInfoThread::getSingleton().Start( M()->m_hWnd, SStringW(M()->maker.m_szMusicPathName));
	}
	else
	{
		_MessageBox(M()->m_hWnd, L"请先选择音乐文件 :)", L"提示", MB_OK|MB_ICONINFORMATION);
	}
}

//打开编辑选中的歌词文件
void CPageMaking::OnBtnEditLyric()
{
	if(_tcslen(M()->maker.m_szLyricPathName)!=0)
	{	
		//用window默认编辑器打开歌词文件
		ShellExecute(NULL,L"open",L"notepad", M()->maker.m_szLyricPathName,NULL,SW_SHOWNORMAL);
	}
	else
	{
		_MessageBox(M()->m_hWnd, L"请先选择歌词文件 :)", L"提示", MB_OK|MB_ICONINFORMATION);
	}
}

//进入匹配ID页面，自动填充数据搜索ID，如果已匹配过ID，初始化到页面显示
void CPageMaking::OnBtnMatchID()
{
	M()->FindChildByID2<STabCtrl>(R.id.tab_content_container)->SetCurSel(1); //进入页面

	//使用文件路径的文件名填充歌曲文件名编辑控件
	M()->m_pageSearchNcmID->SetSongFileNameEditWithPath(M()->maker.m_szMusicPathName);  

	//复用猜测搜索歌词信息线程，来猜测搜索ID需要的信息
	CGuessLyricInfoThread::getSingleton().Start( M()->m_hWnd, SStringW(M()->maker.m_szMusicPathName), true);
}


//第一个页面(歌词制作)：回到“加载按钮”按下后的状态
void CPageMaking::backToInit_1()
{
	M()->maker.m_vLyricOriginWithEmptyLine = M()->maker.m_vLyricOrigin;
	M()->maker.m_vLyricOutput.clear();
	M()->maker.stopMusic();

	//重置显示面板，显示第一条非空歌词
	P1_Line1->SetWindowTextW(_T(""));
	P1_Line2->SetWindowTextW(_T(""));
	P1_Line3->SetWindowTextW(M()->maker.m_vLyricOrigin[0]); //第一条歌词数据

	//隐藏空行提示
	emptyTip1->SetVisible(FALSE,TRUE);
	
	//改变按钮的状态
	m_btnLoad->EnableWindow(TRUE,TRUE);
	m_btnStart->EnableWindow(TRUE,TRUE);
	m_btnRestart->EnableWindow(FALSE,TRUE);
	
	//取消歌词显示面板中的“当前行”的高亮背景
	m_wndHighlight->SetVisible(FALSE,TRUE);

	//重置时间标签和进度条
	m_txtTime->SetWindowTextW(_T("00:00.000"));
	m_ProgressTime->SetValue( 0 );//设置千分数值

	//改变状态  锁定按键信息
	M()->m_bIsLyricMaking = FALSE;

	M()->KillTimer(101);
}

//我要重制
void CPageMaking::OnBtnRestart()
{
	//重置状态
	backToInit_1();
}

//开始制作
void CPageMaking::OnBtnStartMaking()
{
	//如果正在：播放预览歌词 的状态，则不允许制作歌词
	if(M()->m_bIsLyricPlaying == TRUE)
	{
		_MessageBox(M()->m_hWnd,_T("请先结束【歌词滚动预览】！"),_T("提示"),MB_OK|MB_ICONINFORMATION);
		return;
	}

	bool bSuccess = M()->maker.makingStart();
	if(!bSuccess)
	{
		//播放不成功，需要转换文件
		
		//打开失败，有可能是MCI初始化出错
		//开启线程进行转换
		::CreateThread(NULL, 0, ThreadConvertProc, this, 0 ,NULL);

		return;
	}
	
	//改变相应的数据状态
	M()->m_bIsLyricMaking = TRUE;
	
	//改变按钮的状态
	m_btnLoad->EnableWindow(FALSE,TRUE);
	m_btnStart->EnableWindow(FALSE,TRUE);
	m_btnRestart->EnableWindow(TRUE,TRUE);
	
	//改变预览按钮的状态
	m_btnPreview->EnableWindow(FALSE,TRUE);
	m_btnOpenOutput->EnableWindow(FALSE,TRUE);

	//显示歌词显示面板中的“当前行”的高亮背景
	m_wndHighlight->SetVisible(TRUE,TRUE);

	M()->SetTimer(101,1);

	//记录页面播放足迹，详看变量的说明
	M()->FootPrintPage = 0;
}

//根据maker制作歌词的当前行数，更新界面数据
void CPageMaking::UpdataMakerLyricShowing()
{
	if(M()->maker.m_nCurLine==0) //最开始
	{
		P1_Line1->SetWindowTextW(_T(""));
		P1_Line2->SetWindowTextW(_T(""));
		P1_Line3->SetWindowTextW( M()->maker.m_vLyricOrigin[0]); //第一条歌词数据
		emptyTip1->SetVisible(FALSE,TRUE);
	}
	else
	{
		//第一次第一行没有数据
		if(M()->maker.m_nCurLine == 1)
		{
			P1_Line1->SetWindowTextW( _T(""));
		}else
		{
			P1_Line1->SetWindowTextW( M()->maker.GetOriginLyricAt(M()->maker.m_nCurLine -1));
		}

		P1_Line2->SetWindowTextW(  M()->maker.GetOriginLyricAt(M()->maker.m_nCurLine));
	
		if(M()->maker.m_nCurLine != M()->maker.m_nTotalLine)
		{
			P1_Line3->SetWindowTextW(  M()->maker.GetOriginLyricAt(M()->maker.m_nCurLine +1));
		}
		else//最后行后面数据为空
		{
			P1_Line3->SetWindowTextW(_T(""));
		}

		//根据上一行是否为空行，显示空行提示
		if(M()->maker.isLastLineSpace())
			emptyTip1->SetVisible(TRUE,TRUE);
		else
			emptyTip1->SetVisible(FALSE,TRUE);
	}
}

void CPageMaking::OnBtnSoundOpen1()
{
	M()->OnBtnSoundOpen();
}

void CPageMaking::OnBtnSoundClose1()
{
	M()->OnBtnSoundClose();
}

//处理声音slider 位置的变化
void  CPageMaking::OnSliderPos1(EventArgs *pEvt)
{
	M()->OnSliderPos(true);
}


//判断第一个页面(歌词制作) 的 三个路径是否都选择完毕 */
bool CPageMaking::isPathReady_1()				
{
	bool ret = true;
	for(int i=0; i< 3; i++)
		if(!PATH_STATE_1[i].isInited)
		{
			ret = PATH_STATE_1[i].isInited;
			break;
		}
	return ret;
}

/* 获得当前未初始化的第一个路径 在PATH_STATE_1 中对应的下标;都初始化了则返回-1 */
int CPageMaking::getPathNotReady_1()			
{
	int index = -1;
	for(int i=0; i< sizeof(PATH_STATE_1)/sizeof(PATH_STATE_1[0]); i++)
		if(!PATH_STATE_1[i].isInited)
		{
			index = i;
			break;
		}
	return index;
}


//线程执行地址(格式转换线程)
DWORD WINAPI CPageMaking::ThreadConvertProc(LPVOID pParam)
{
	CPageMaking* pPageMaking  = (CPageMaking*)pParam;

	WCHAR szMusicPathName[_MAX_PATH];
	pPageMaking->M()->maker.m_musicPlayer.GetMusicPathName(szMusicPathName, _MAX_PATH);

	wstring strMusicPath(szMusicPathName);
	wstring strDir = FileHelper::GetCurrentDirectoryStr();
	wstring strName;
	wstring strExt;
	FileHelper::SplitPath(strMusicPath, NULL,NULL,&strName,&strExt);

	if(strExt == L".wav")
	{
		_MessageBox(NULL,   (L"无法播放文件："+strName + L"\\n请尝试使用别的音乐文件，或转换格式").c_str(), L"提示", MB_OK| MB_ICONINFORMATION);
		return false;
	}

	wstring strTargetDir = strDir + TEMP_WAV_FLODER_NAME ; //得到目标文件夹
	wstring strTargetFilePath = strDir + TEMP_WAV_FLODER_NAME +L"\\"+ strName + L".wav"; //得到目标文件夹
	wstring strFfmpegPath = strDir + TEMP_WAV_FLODER_NAME +L"\\ffmpeg.exe"; //得到目标文件夹

	if(!FileHelper::CheckFolderExist(strTargetDir))
	{
		_MessageBox(NULL, (L"文件夹不存在："+strTargetDir + L"\\n\\n试图转格式失败\\n请尝试：\
			\\n1、确保 文件夹【wav】存在，并与【BesLyric.exe】在同一级目录下\
			\\n2、重新下载完整程序"
			).c_str(), L"提示", MB_OK| MB_ICONINFORMATION);
		return false;
	}

	if(!FileHelper::CheckFileExist(strFfmpegPath))
	{
		_MessageBox(NULL, (L"文件不存在："+strFfmpegPath + L"\\n\\n试图转格式失败\\n请尝试：\
			\\n1、在【设置】页面的【升级与检测】下，点击按钮【完整性检测】\
			\\n2、重新下载完整程序"
			).c_str(), L"提示", MB_OK| MB_ICONINFORMATION);
		return false;
	}

	//开始转码
	WCHAR command[_MAX_PATH * 3];
	_swprintf(command, L"-y -i \"%s\" \"%s\"",szMusicPathName, strTargetFilePath.c_str());

	//ShellExecute(NULL,L"open",strFfmpegPath.c_str(), command ,NULL,SW_SHOWNORMAL);

	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = strFfmpegPath.c_str();	
	ShExecInfo.lpParameters = command;	
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOWNORMAL;
	ShExecInfo.hInstApp = NULL;	
	
	if(!FileHelper::CheckFileExist(strTargetFilePath))//不存在，则执行转换，已存在则不转换
	{
		ShellExecuteEx(&ShExecInfo);
		WaitForSingleObject(ShExecInfo.hProcess,INFINITE);  //等待直到执行完毕，得到目标文件
	}

	if(!FileHelper::CheckFileExist(strTargetFilePath))
	{
		_MessageBox(NULL, (L"文件转格式失败："+ wstring(szMusicPathName)).c_str(), L"提示", MB_OK| MB_ICONINFORMATION);
		return false;
	}

	//pPageMaking->M()->maker.m_musicPlayer.m_szMusicPathName;
	pPageMaking->M()->maker.setMusicPath(strTargetFilePath.c_str(),pPageMaking->M()->m_hWnd);

	//注意：这里不能使用如下2种方式调用

	//错误方式 一、
	//发送消息给主窗口，触发 OnBtnStartMaking 函数
	//EventCmd evt(pPageMaking->M());
	//evt.idFrom = R.id.btn_start_making;
	//pPageMaking->M()->FireEvent(evt);

	//错误方式 二、
	//pPageMaking->OnBtnStartMaking();

	//因为这两种方式都是在本线程执行，对主线程不会产生影响，而是在当前线程直接播放

	::PostMessage(pPageMaking->M()->m_hWnd,MSG_USER_MAKING_START_BUTTON,0,0);

	return true;
}


//在载入时处理Ncm文件
// 返回 0 表示不是ncm文件
//		1 本次载入的文件为ncm文件,该文件没有匹配ID
//		2 本次载入的文件为ncm文件，下载mp3失败
//		3 本次载入的文件为ncm文件，下载mp3成功，准备就绪
//		-1; //创建文件夹错误
int CPageMaking::DealWithNcmFile(OUT bool& isCurrentNcm, OUT wstring& strID, OUT wstring& strName, OUT wstring& strMp3FilePath)
{
	if(CFileDialogEx::checkPathName(_T("*.ncm"),M()->maker.m_szMusicPathName))
	{
		isCurrentNcm = true;

		//得到文件名，查询当前歌曲的id
		wstring strMusicPath = M()->maker.m_szMusicPathName;
		auto indexBeg = strMusicPath.find_last_of(L'\\')+1;
		auto indexEnd = strMusicPath.find_last_of(L'.');
		strName = strMusicPath.substr( indexBeg,indexEnd - indexBeg);

		if(!CNcmIDManager::GetInstance()->FindID(strName, strID))
			return 1;// 1表示ncm 还没匹配ID

		//查询mp3 是否已经下载
		wstring strMp3FloderPath = FileHelper::GetCurrentDirectoryStr() + TEMP_MP3_FLODER_NAME ;
		strMp3FilePath = strMp3FloderPath + L"\\" + strName + L".mp3" ;
		if(FileHelper::CheckFileExist(strMp3FilePath))
			return 3; //已经存在，准备就绪

		//确保mp3文件夹存在
		if(!FileHelper::CheckFolderExist(strMp3FloderPath))
		{
			if(RET_SUCCEEDED != _wmkdir(strMp3FloderPath.c_str()))
			{
				wstring strTip = L"程序无法创建目录：\\n";
				strTip += strMp3FloderPath +L"\\n";
				strTip += L"这将导致ncm无法下载mp3,从而无法正常播放\\n";
				
				_MessageBox(NULL, strTip.c_str(), L"提示", MB_OK|MB_ICONWARNING);
				return -1; //创建文件夹错误
			}
		}

		//接下来弹框，下载mp3, 过程结束后关闭mp3
		
		DlgDownloadNcmMp3 dlg(L"xml_processing_tip");

		dlg.SetNcmNameAndID(strName,strID);
		
		int ret = dlg.DoModal(NULL);
		
		_MessageBox(NULL,L"",L"",MB_OK); //加上此句抵消窗口关闭整个程序的现象，不知是不是使用SOUI上的错误，弹出的窗口关闭后得加上此句“抵消”程序关闭消息

		if(ret == IDCANCEL)
			return 2;//这里表示失败
		else
			return 3;//这里已经转换完了
	}
	else
		isCurrentNcm = false;

	return 0;
}









