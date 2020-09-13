#include "stdafx.h"
#include "PageResult.h"
#include "utility/WinDialog.h"
#include "utility/WinFile.h"
#include "entity\CheckIntegrityThread.h"

CPageResult::CPageResult()
{
	wcscpy_s(PATH_STATE_2[0].nameOfPath, L"音乐文件");
	wcscpy_s(PATH_STATE_2[1].nameOfPath, L"歌词文件");
	PATH_STATE_2[0].isInited = false;
	PATH_STATE_2[1].isInited = false;

	P2_Line1 = NULL;
	P2_Line2 = NULL;
	P2_Line3 = NULL;
	m_wndHighlight = NULL;
	
	emptyTip2= NULL;
	
	m_EditMusic   = NULL;
	m_EditLyric   = NULL;
	
	m_txtMusic = NULL;
	m_txtLyric = NULL;

	m_txtTime = NULL;
	m_ProgressTime = NULL;
	
	m_btnStart = NULL;
	m_btnAdjust =NULL;
	m_btnEndPreview = NULL;
	m_btnLoad = NULL;

	m_bSingleCycle = false;	//单曲循环
}

//初始化设置页面
void CPageResult::Init(SHostWnd *pMainWnd)
{
	SASSERT(NULL != pMainWnd);

	//保存主窗口对象
	m_pMainWnd = pMainWnd;

	//获得代码中常用的控件的指针
	P2_Line1 = M()->FindChildByID2<SStatic>(R.id.name_2_line_1);
	SASSERT(P2_Line1 != NULL);
	P2_Line2 = M()->FindChildByID2<SStatic>(65001);
	SASSERT(P2_Line2 != NULL);
	P2_Line3 = M()->FindChildByID2<SStatic>(R.id.name_2_line_3);
	SASSERT(P2_Line3 != NULL);
	m_wndHighlight = m_pMainWnd->FindChildByID(R.id.highlight_bkgnd_2);
	SASSERT(m_wndHighlight != NULL);

	emptyTip2 = M()->FindChildByID2<SStatic>(R.id.empty_tip_block_2);
	SASSERT(emptyTip2 != NULL);


	m_EditMusic = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_music_2);
	SASSERT(m_EditMusic != NULL);
	m_EditLyric = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lyric_2);
	SASSERT(m_EditLyric != NULL);
	
	m_txtMusic = m_pMainWnd->FindChildByID2<SStatic>(R.id.line_music_2);
	SASSERT(m_txtMusic != NULL);
	m_txtLyric = m_pMainWnd->FindChildByID2<SStatic>(R.id.line_lyric_2);
	SASSERT(m_txtLyric != NULL);
	
	m_txtTime = m_pMainWnd->FindChildByID2<SStatic>(R.id.text_time_2);
	SASSERT(m_txtTime != NULL);
	m_ProgressTime = m_pMainWnd->FindChildByID2<SProgress>(R.id.progress_music_2);
	SASSERT(m_ProgressTime != NULL);
	
	m_btnStart = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_start_playing);
	SASSERT(m_btnStart != NULL);
	m_btnAdjust = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_manual_adjust);
	SASSERT(m_btnAdjust != NULL);
	m_btnEndPreview = M()->FindChildByID2<SButton>(R.id.btn_end_preview);
	SASSERT(m_btnEndPreview != NULL);
	m_btnLoad = M()->FindChildByID2<SButton>(R.id.btn_load_2);
	SASSERT(m_btnLoad != NULL);
	
	m_checkDesktopLyric = M()->FindChildByID2<SCheckBox>(R.id.check_desktop_lyric);
	SASSERT(m_checkDesktopLyric != NULL);
	m_checkSingleCycle = M()->FindChildByID2<SCheckBox>(R.id.check_single_cycle);
	SASSERT(m_checkSingleCycle != NULL);
}

//获得主窗口对象
CMainDlg* CPageResult::M()
{
	return (CMainDlg*)m_pMainWnd;
}


/*
*	滚动预览页面的响应函数
*/

//两个路径的选择
void CPageResult::OnBtnSelectMusic2(LPCWSTR pFilePath)
{	
	BOOL bRet = TRUE;
	LPCWSTR pPath = NULL;

	if(pFilePath == NULL)
	{
		CBrowseDlg Browser;
		bRet = Browser.DoFileBrowse(
			::GetActiveWindow(),
			L"音频文件(mp3,mp2,wma,wav,wv,ape,flac)\0*.mp3;*.mp2;*.wma;*.wav;*.wv;*.ape;*.flac;\0手机录音(amr)/手机铃声(mmf)\0*amr;*.mmf\0\0",
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
		 || CFileDialogEx::checkPathName(_T("*.mmf"),pPath))
		{	
			; //使用“或”条件判断( || )而 不用“且”条件判断（&&），以减少 checkPathName 调用的次数
		}
		else 
		{
			_MessageBox(M()->m_hWnd,_T("格式不支持\\n请确定文件格式为【选择对话框指定的文件类型】"),_T("提示"),MB_OK|MB_ICONINFORMATION);
			return;
		}

		//显示新选择的文件
		m_EditMusic->SetWindowTextW(pPath);

		PATH_STATE_2[0].isInited = true;

		//歌词播放器
		M()->player.setMusicPath(pPath, M()->m_hWnd);
	}
    
}

void CPageResult::OnBtnSelectLyric2(LPCWSTR pFilePath)
{	
	BOOL bRet = TRUE;
	LPCWSTR pPath = NULL;

	if(pFilePath == NULL)
	{
		CBrowseDlg Browser;
		bRet = Browser.DoFileBrowse(
			::GetActiveWindow(),
			L"LRC文本文件(*.lrc)\0*.lrc\0\0",
			FileHelper::CheckFolderExist(M()->m_settingPage.m_default_output_path)? M()->m_settingPage.m_default_output_path.c_str():nullptr
			);
		
		if(bRet == TRUE)
			pPath = Browser.GetFilePath();
	}
	else
		pPath = pFilePath;

	if(bRet == TRUE)
	{
		if(!CFileDialogEx::checkPathName(_T("*.lrc"),pPath))
		{
			_MessageBox(M()->m_hWnd,_T("格式不支持\\n请确定文件格式为【*.lrc】"),_T("提示"),MB_OK|MB_ICONINFORMATION);
			return;
		}
		
		//显示新选择的文件
		m_EditLyric->SetWindowTextW(pPath);
		
		PATH_STATE_2[1].isInited = true;
		
		//加入歌词播放器
		M()->player.setLyricPath(pPath);
	}
}

void CPageResult::OnBtnLoad2()
{
	//确保两个个路径的数据都已经初始化
	if(!isPathReady_2())
	{
		//CMainDlg::getPathNotReady_2() 此时返回的正是 未初始化的路径 在 PATH_STATE 中对应的下标
		_MessageBox(M()->m_hWnd,SStringT().Format(_T("您还没选择：\\n【%s】"),PATH_STATE_2[getPathNotReady_2()].nameOfPath),
			_T("提示"),MB_OK|MB_ICONINFORMATION);
		return;
	}

	//确保 歌词文件 和 音乐文件 路径有效
	File lyricFile(M()->player.m_szLyricPathName,_T("r"));

	if(!lyricFile.isValidFile()) 
	{
		_MessageBox(M()->m_hWnd,SStringT().Format(_T("文件打开失败:\\n【%s】\\n请确保文件有效!"),M()->player.m_szLyricPathName),
			_T("失败提示"),MB_OK|MB_ICONWARNING);
		return;
	}

	//重置 LyricPlayer的 歌词数据为空
	M()->player.reloadPlayer();

	//从文件获取带时间信息的每行歌词的集合向量
	M()->player.m_vLineInfo = M()->player.getLyricWithLineInfo(lyricFile);

	//没有歌词，不能播放
	if(M()->player.m_vLineInfo.empty())
	{
		//清空 （可能存在的）页面的 当前音乐 和 当前歌词的信息
		m_txtMusic->SetWindowTextW(_T(""));
		m_txtLyric->SetWindowTextW(_T(""));

		//清空 （可能存在的）第三行歌词
		P2_Line3->SetWindowTextW(_T(""));

		//禁用 （可能启用的）开始按钮
		m_btnStart->EnableWindow(FALSE,TRUE); //第一个参数bEnable为 是否启用，第二个参数bUpdate为是否在改变状态后更新显示

		//禁用 （可能启用的）手动微调时间轴
		m_btnAdjust->EnableWindow(FALSE,TRUE); 

		_MessageBox(M()->m_hWnd,SStringT().Format(_T("当前歌词文件没有可播放内容！\\n文件：\\n【%s】"),M()->player.m_szLyricPathName),
			_T("失败提示"),MB_OK|MB_ICONWARNING);
		return;
	}

	//更新 页面的 当前音乐 和 当前歌词的信息
	m_txtMusic->SetWindowTextW(M()->player.m_szMusicPathName);
	m_txtLyric->SetWindowTextW(M()->player.m_szLyricPathName);

	//更新化页面的显示
	//读取完毕，清空显示面板，显示第一条非空歌词
	P2_Line1->SetWindowTextW(_T(""));
	P2_Line2->SetWindowTextW(_T(""));
	P2_Line3->SetWindowTextW(M()->player.m_vLineInfo[0].m_strLine);

	//改变按钮的状态
	m_btnStart->EnableWindow(TRUE,TRUE); //第一个参数bEnable为 是否启用，第二个参数bUpdate为是否在改变状态后更新显示

	//启用用 “手动微调时间轴”按钮
	m_btnAdjust->EnableWindow(TRUE,TRUE); 

}

//第二个页面(滚动预览)：回到“加载按钮”按下后的状态
void CPageResult::backToInit_2()
{
	//结束播放和歌词滚动
	M()->player.playingEnd(m_pMainWnd);

	//重置显示面板，显示第一条非空歌词
	P2_Line1->SetWindowTextW(_T(""));
	P2_Line2->SetWindowTextW(_T(""));
	P2_Line3->SetWindowTextW(M()->player.m_vLineInfo[0].m_strLine);

	//隐藏空行提示
	emptyTip2->SetVisible(FALSE,TRUE);

	//改变按钮的状态
	m_btnLoad->EnableWindow(TRUE,TRUE);
	m_btnStart->EnableWindow(TRUE,TRUE);
	m_btnEndPreview->EnableWindow(FALSE,TRUE);
	
	//取消歌词显示面板中的“当前行”的高亮背景
	m_wndHighlight->SetVisible(FALSE,TRUE);

	//重置时间标签和进度条
	m_txtTime->SetWindowTextW(_T("00:00.000"));
	m_ProgressTime->SetValue( 0 );//设置千分数值

	//结束“播放预览”状态
	M()->m_bIsLyricPlaying = FALSE;
}

//结束预览播放，重置状态
void CPageResult::OnBtnEndPreview()
{	
	//重置状态
	backToInit_2();
}

//开始播放歌词
void CPageResult::OnBtnStartPlaying()
{
	//如果正在：制作歌词 的状态，则不允许播放预览歌词
	if(M()->m_bIsLyricMaking == TRUE)
	{
		_MessageBox(M()->m_hWnd,_T("请先结束【歌词制作】！"),_T("提示"),MB_OK|MB_ICONINFORMATION);
		return;
	}

	//开始播放和歌词滚动
	bool bSuccess = M()->player.playingStart(m_pMainWnd);
	if(!bSuccess)
	{
		//播放不成功，需要转换文件
		
		//打开失败，有可能是MCI初始化出错
		//开启线程进行转换
		::CreateThread(NULL, 0, ThreadConvertProc, this, 0 ,NULL);

		return;
	}

	//改变按钮的状态
	m_btnLoad->EnableWindow(FALSE,TRUE);
	m_btnStart->EnableWindow(FALSE,TRUE);
	m_btnEndPreview->EnableWindow(TRUE,TRUE);
	
	//显示歌词显示面板中的“当前行”的高亮背景
	SWindow *highLight = M()->FindChildByID(R.id.highlight_bkgnd_2);
	m_wndHighlight->SetVisible(TRUE,TRUE);

	//进入“播放预览”状态
	M()->m_bIsLyricPlaying = TRUE;

	//记录页面播放足迹，详看变量的说明
	M()->FootPrintPage = 1;
}

void CPageResult::OnBtnManualAdjust()
{	
	if(_tcslen(M()->player.m_szLyricPathName)!=0)
	{
		//用window默认编辑器打开歌词文件
		ShellExecute(NULL,L"open",L"notepad", M()->player.m_szLyricPathName,NULL,SW_SHOWNORMAL);
	}
}



//判断第二个页面(滚动预览) 的 两个路径是否都选择完毕 */
bool CPageResult::isPathReady_2()				
{
	bool ret = true;
	for(int i=0; i< 2; i++)
		if(!PATH_STATE_2[i].isInited)
		{
			ret = PATH_STATE_2[i].isInited;
			break;
		}
	return ret;
}

/* 获得当前未初始化的第一个路径 在PATH_STATE_2 中对应的下标;都初始化了则返回-1 */
int CPageResult::getPathNotReady_2()			
{
	int index = -1;
	for(int i=0; i< sizeof(PATH_STATE_2)/sizeof(PATH_STATE_2[0]); i++)
		if(!PATH_STATE_2[i].isInited)
		{
			index = i;
			break;
		}
	return index;
}


//在滚动预览的页面，执行歌词显示 m_nCurLine
void CPageResult::scrollToLyricCurLine()
{
	//player.m_nCurLine 的范围是1 ~ player.m_nTotalLine

	//这里每个条件里的第二行本来可以拿出来先执行，但是由于还我不了解更新是依次刷新还是 一次全刷新，
	//为保证显示顺序从上到下能依次刷新，这里选择累赘一点的分别写在各个条件中
	if(M()->player.m_nCurLine == 1)
	{
		P2_Line1->SetWindowTextW(L"");

		//更新第二行和第三行
		P2_Line2->SetWindowTextW(M()->player.m_vLineInfo[M()->player.m_nCurLine -1].m_strLine);
		P2_Line3->SetWindowTextW(M()->player.m_vLineInfo[M()->player.m_nCurLine+1 -1].m_strLine);
	}
	else if(M()->player.m_nCurLine < M()->player.m_nTotalLine)
	{
		//更新三行
		P2_Line1->SetWindowTextW(M()->player.m_vLineInfo[M()->player.m_nCurLine-1 -1].m_strLine);
		P2_Line2->SetWindowTextW(M()->player.m_vLineInfo[M()->player.m_nCurLine -1].m_strLine);
		P2_Line3->SetWindowTextW(M()->player.m_vLineInfo[M()->player.m_nCurLine+1 -1].m_strLine);
	}
	else // player.m_nCurLine == player.m_nTotalLine
	{
		//更新第一行和第二行
		P2_Line1->SetWindowTextW(M()->player.m_vLineInfo[M()->player.m_nCurLine-1 -1].m_strLine);
		P2_Line2->SetWindowTextW(M()->player.m_vLineInfo[M()->player.m_nCurLine -1].m_strLine);

		//将最后一行清空
		P2_Line3->SetWindowTextW(L"");
	}

	//根据歌词是否为空，显示或隐藏空行提示
	emptyTip2->SetVisible(M()->player.m_vLineInfo[M()->player.m_nCurLine -1].m_bIsEmptyLine,TRUE);
}

//桌面歌词
void CPageResult::OnCheckDesktopLyricChanged()
{
	BOOL bDesktopChecked = m_checkDesktopLyric->IsChecked();
	//if(bDesktopChecked)
	//	M()->m_wndDesktopLyric->ShowDesktopLyric();
	//else
	//	M()->m_wndDesktopLyric->HideDesktopLyric();
	if(bDesktopChecked)
		M()->m_wndDesktopLyric->ShowWindow(SW_SHOW);
	else
		M()->m_wndDesktopLyric->ShowWindow(SW_HIDE);
}

//单曲循环
void CPageResult::OnCheckSingleCycleChanged()
{
	BOOL bSingleCycleChecked = m_checkSingleCycle->IsChecked();
	if(bSingleCycleChecked)
		 m_bSingleCycle = true;
	else
		 m_bSingleCycle = false;
}

void CPageResult::OnBtnSoundOpen2()
{
	M()->OnBtnSoundOpen();
}

void CPageResult::OnBtnSoundClose2()
{
	M()->OnBtnSoundClose();
}

void  CPageResult::OnSliderPos2(EventArgs *pEvt)
{
	M()->OnSliderPos(false);
}


//线程执行地址(格式转换线程)
DWORD WINAPI CPageResult::ThreadConvertProc(LPVOID pParam)
{
	CPageResult* pPageResult  = (CPageResult*)pParam;

	WCHAR szMusicPathName[_MAX_PATH];
	pPageResult->M()->player.m_musicPlayer.GetMusicPathName(szMusicPathName, _MAX_PATH);

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
		_MessageBox(NULL, (L"文件夹不存在："+strTargetDir + L"\\n试图转格式失败，请重新下载完整程序").c_str(), L"提示", MB_OK| MB_ICONINFORMATION);
		return false;
	}

	bool bFileExist =  FileHelper::CheckFileExist(strFfmpegPath);
	string strMd5;
	bool bRet = UpdateHelper::GetFileMd5(strFfmpegPath,strMd5);
	if(!bFileExist || !bRet || (bFileExist && strMd5 != "949ed6af96c53ba9e1477ded35281db5")) //检测
	{
		_MessageBox(NULL, (L"文件不存在或不完整：\\n"+strFfmpegPath + L"\\n\\n试图转格式失败\\n请尝试：\
			\\n1、在【设置】页面的【升级与检测】下，点击按钮【完整性检测】"
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

	pPageResult->M()->player.setMusicPath(strTargetFilePath.c_str(),pPageResult->M()->m_hWnd);

	//注意事项，参考 PageMaking 中ThreadConvertProc 回调函数

	::PostMessage(pPageResult->M()->m_hWnd,MSG_USER_PLAYING_START_BUTTON,0,0);

	return true;
}


