#include "stdafx.h"
#include "PageSearchLyric.h"
#include "controlEx\LyricListView.h"
#include "utility\WinDialog.h"
#include "utility\WinFile.h"
#include "utility\StringHelper.h"
#include "entity\SearchLyricThread.h"


CPageSearchLyric::CPageSearchLyric()
{
	m_editSearchLyricName = NULL;
	m_editSearchLyricArtist = NULL;

	m_btnSearchHere = NULL;
	m_btnSearchBaidu = NULL;

	m_wndSearchLyricTip = NULL;
	m_txtSearchNameTip = NULL;
	m_txtSearchArtistTip = NULL;
	m_wndSearchArtistTip = NULL;
	m_txtSearchResultTip = NULL;
	m_txtIsSearchingLyricTip = NULL;

	m_editOriginLyricPath = NULL;
	m_editLrcLyricPath = NULL;
	
	m_btnSelectOriginLyricPath = NULL;
	m_btnSelectLrcLyricPath = NULL;
	
	m_editOriginLyricName = NULL;
	m_editOriginLyricArtist = NULL;
	m_editLrcLyricName = NULL;
	m_editLrcLyricArtist = NULL;
	
	m_editOriginLyricContent = NULL;
	m_editLrcLyricContent = NULL;
	
	m_pMainWnd = NULL;		/* 主窗口指针 */
}

//初始化设置页面
void CPageSearchLyric::Init(SHostWnd *pMainWnd)
{
	SASSERT(NULL != pMainWnd);

	//保存主窗口对象
	m_pMainWnd = pMainWnd;

	//获得代码中常用的控件的指针
	m_editSearchLyricName = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_search_lyric_name);
	m_editSearchLyricArtist = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_search_lyric_artist);

	m_btnSearchHere = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_search_lyric_using_program);
	m_btnSearchBaidu = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_search_lyric_using_baidu);

	m_wndSearchLyricTip = m_pMainWnd->FindChildByID2<SWindow>(R.id.window_search_lyric_tip);
	m_txtSearchNameTip = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_search_lyric_name_tip);
	m_txtSearchArtistTip = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_search_lyric_artist_tip);
	m_wndSearchArtistTip = m_pMainWnd->FindChildByID2<SWindow>(R.id.window_search_lyric_artist_tip);
	m_txtSearchResultTip = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_search_lyric_result_tip);
	m_txtIsSearchingLyricTip = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_is_searching_lyric_tip);

	m_editOriginLyricPath = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_origin_lyric_path);
	m_editLrcLyricPath = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lrc_lyric_path);
	
	m_btnSelectOriginLyricPath = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_select_origin_lyric_path);;
	m_btnSelectLrcLyricPath = m_pMainWnd->FindChildByID2<SButton>(R.id.btn_select_lrc_lyric_path);;
	
	m_editOriginLyricName = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_origin_lyric_name);;
	m_editOriginLyricArtist = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_origin_lyric_artist);;
	m_editLrcLyricName = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lrc_lyric_name);;
	m_editLrcLyricArtist = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lrc_lyric_artist);;
	
	m_editOriginLyricContent = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_origin_lyric_content);;
	m_editLrcLyricContent = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_lrc_lyric_content);;
	
	SASSERT(m_editSearchLyricName != NULL);
	SASSERT(m_editSearchLyricArtist != NULL);
	SASSERT(m_btnSearchHere != NULL);
	SASSERT(m_btnSearchBaidu != NULL);
	SASSERT(m_wndSearchLyricTip != NULL);
	SASSERT(m_txtSearchNameTip != NULL);
	SASSERT(m_txtSearchArtistTip != NULL);
	SASSERT(m_wndSearchArtistTip != NULL);
	SASSERT(m_txtSearchResultTip != NULL);
	SASSERT(m_txtIsSearchingLyricTip != NULL);
	SASSERT(m_editOriginLyricPath != NULL);
	SASSERT(m_editLrcLyricPath != NULL);
	SASSERT(m_btnSelectOriginLyricPath != NULL);
	SASSERT(m_btnSelectLrcLyricPath != NULL);
	SASSERT(m_editOriginLyricName != NULL);
	SASSERT(m_editOriginLyricArtist != NULL);
	SASSERT(m_editLrcLyricName != NULL);
	SASSERT(m_editLrcLyricArtist != NULL);
	SASSERT(m_editOriginLyricContent != NULL);
	SASSERT(m_editLrcLyricContent != NULL);

	m_wndSearchLyricTip->SetVisible(FALSE, TRUE);	//隐藏搜索提示

	//初始化歌词保存路径
	m_editOriginLyricPath->SetWindowTextW(M()->m_settingPage.m_default_lyric_path.c_str());
	m_editLrcLyricPath->SetWindowTextW(M()->m_settingPage.m_default_output_path.c_str());

	//多列listview
    CLyricListView * pMcListView = m_pMainWnd->FindChildByName2<CLyricListView>("mclv_lyric");
    if(pMcListView)
    {
        IMcAdapter *pAdapter = m_lrcListAdapter =new CLyricMcAdapterFix;
		m_lrcListAdapter->AttachMainDlgPointer(M());
        pMcListView->SetAdapter(pAdapter);
        pAdapter->Release();
    }
}

//获得主窗口对象
CMainDlg* CPageSearchLyric::M()
{
	return (CMainDlg*)m_pMainWnd;
}


/*
*	滚动预览页面的响应函数
*/

//在百度搜索歌词
void CPageSearchLyric::OnBtnSearchBaidu()
{
	SStringW strMusicName = L"";
	SStringW strMusicArtist = L"";

	if(!GetMusicAndArtist(strMusicName, strMusicArtist))
		return;
	
	//将链接中的字符进行转换，以使得链接能够支持
	/*
	"\"单反斜杠  %5C 
	"|"      %7C 
	回车  %0D%0A 
	空格  %20 
	双引号 %22 
	"&"		%26 
	*/
	wstring sMusicName = L"";
	wstring sMusicArtist = L"";
	for(auto i = 0;  i < strMusicName.GetLength(); i++)
	{
		switch(strMusicName.GetAt(i))
		{
		case L'\\':
			sMusicName += L"%5C";
			break;
		case L'|':
			sMusicName += L"%7C";
			break;
		case L'\n':
			sMusicName += L"%0D%0A";
			break;
		case L' ':
			sMusicName += L"%20";
			break;
		case L'"':
			sMusicName += L"%22";
			break;
		case L'&':
			sMusicName += L"%26";
			break;
		default:
			sMusicName += strMusicName.GetAt(i);
		}
	}

	if(strMusicArtist.GetLength() != 0)
	for(auto i = 0;  i < strMusicArtist.GetLength(); i++)
	{
		switch(strMusicArtist.GetAt(i))
		{
		case L'\\':
			sMusicArtist += L"%5C";
			break;
		case L'|':
			sMusicArtist += L"%7C";
			break;
		case L'\n':
			sMusicArtist += L"%0D%0A";
			break;
		case L' ':
			sMusicArtist += L"%20";
			break;
		case L'"':
			sMusicArtist += L"%22";
			break;
		case L'&':
			sMusicArtist += L"%26";
			break;
		default:
			sMusicArtist += strMusicArtist.GetAt(i);
		}
	}

	wstring sParam = L"start https://www.baidu.com/s?wd=%22";
	sParam += sMusicName.c_str();

	if(strMusicArtist.GetLength() != 0)
	{
		sParam += L"%22%20%22";
		sParam += sMusicArtist.c_str();
	}

	sParam += L"%22%20%22歌词%22";

	//使用控制台打开默认浏览器百度搜索歌词
	_wsystem(sParam.c_str());
	
}


//通过本软件内获取歌词
void CPageSearchLyric::OnBtnSearchInProgram()
{
	if(CSearchLyricThread::getSingleton().IsSearching())
	{
		_MessageBox(NULL,L"搜索模块正被占用，请等待.....",L"等待提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	SStringW strMusicName = L"";
	SStringW strMusicArtist = L"";

	if(!GetMusicAndArtist(strMusicName, strMusicArtist))
		return;

	//切换回列表页面
	M()->FindChildByID2<STabCtrl>(R.id.tab_lyric_list)->SetCurSel(0);

	//显示正在搜索
	m_txtSearchNameTip->SetWindowTextW(L"“"+ strMusicName  + L"”");
	m_txtSearchArtistTip->SetWindowTextW(L"“"+ strMusicArtist  + L"”");

	m_wndSearchArtistTip->SetVisible( strMusicArtist.GetLength() == 0 ? FALSE:TRUE, TRUE);
	m_txtSearchResultTip->SetVisible(FALSE,TRUE);
	m_txtIsSearchingLyricTip->SetVisible(TRUE,TRUE);
	
	m_wndSearchLyricTip->SetVisible(TRUE, TRUE); //显示提示

	//隐藏按钮，不让继续搜索
	m_pMainWnd->FindChildByID2<SButton>(R.id.btn_search_lyric_using_program)->EnableWindow(FALSE, TRUE);
	

	//开启搜索歌词线程
	CSearchLyricThread::getSingleton().Start(M()->m_hWnd, strMusicName,strMusicArtist);
}

void CPageSearchLyric::ShowLyricResult(LyricSearchResult* pResult)
{
	//删除列表中所有的已有数据
	static int nLyricCount = 0;
	if(!pResult->bAppendToList)  
	{
		nLyricCount = 0;
		m_lrcListAdapter->DeleteAllItem();
	}

	SStringW strResultTip = L"";
	if(pResult->bShowUnexpectedResultTip)
		strResultTip = pResult->strUnexpectedResultTip;

	//显示搜索结果
	nLyricCount += pResult->vecLyricInfoTotal.size();
	m_txtSearchResultTip->SetWindowTextW(SStringW().Format(L"，找到%d个歌词文件。 %s",nLyricCount , strResultTip.GetBuffer(1) ));
	m_txtSearchResultTip->SetVisible(TRUE,TRUE);

	for(auto iter = pResult->vecLyricInfoTotal.begin(); iter != pResult->vecLyricInfoTotal.end(); iter++)
		m_lrcListAdapter->AddItem( iter->strSong, iter->strArtist, iter->strLyricFrom,iter->strPlaneText, iter->strLabelText);
	m_lrcListAdapter->notifyDataSetChanged();
	
	if(pResult->bCurrentSearchDone) //搜索结束时才能恢复按钮
	{
		//恢复按钮，可以下一次继续搜索
		m_pMainWnd->FindChildByID2<SButton>(R.id.btn_search_lyric_using_program)->EnableWindow(TRUE, TRUE);
		
		m_txtIsSearchingLyricTip->SetVisible(FALSE,TRUE); //隐藏正在搜索提示
	}
}

//选择原歌词保存路径
void CPageSearchLyric::OnBtnSelectOriginLyricPath(LPCWSTR pFilePath)
{
	BOOL bRet = TRUE;
	LPCWSTR pPath = NULL;

	if(pFilePath == NULL)
	{
		SStringW pathBefore = m_editOriginLyricPath->GetWindowTextW();
		CBrowseDlg Browser;
		BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("选择 【原歌词】 保存路径"), TRUE, pathBefore);

		if(bRet == TRUE)
			pPath = Browser.GetDirPath();
	}
	else
		pPath = pFilePath;

	if(bRet == TRUE)
	{
		m_editOriginLyricPath->SetWindowTextW(pPath);
	}
}

//选择Lrc歌词保存路径
void CPageSearchLyric::OnBtnSelectLrcLyricPath(LPCWSTR pFilePath)
{
	BOOL bRet = TRUE;
	LPCWSTR pPath = NULL;

	if(pFilePath == NULL)
	{
		SStringW pathBefore = m_editLrcLyricPath->GetWindowTextW();
		CBrowseDlg Browser;
		BOOL bRet = Browser.DoDirBrowse(::GetActiveWindow(),_T("选择 【LRC歌词】 保存路径"), TRUE, pathBefore);
		
		if(bRet == TRUE)
			pPath = Browser.GetDirPath();
	}
	else
		pPath = pFilePath;

	if(bRet == TRUE)
	{
		m_editLrcLyricPath->SetWindowTextW(pPath);
	}
}	

//保存原歌词
void CPageSearchLyric::OnBtnSaveOriginLyricPath()
{
	SStringW strContent = CStringHelper::Trim(m_editOriginLyricContent->GetWindowTextW(),L" \t\r\n");

	if(strContent.GetLength() == 0)
	{
		_MessageBox(M()->m_hWnd, L"歌词内容为空，无法保存！", L"提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	SStringW strPath = m_editOriginLyricPath->GetWindowTextW();
	SStringW strSong = CStringHelper::Trim(m_editOriginLyricName->GetWindowTextW());
	SStringW strArtist = CStringHelper::Trim(m_editOriginLyricArtist->GetWindowTextW());
	SStringW strConnector = (strSong.GetLength()!=0 && strArtist.GetLength() != 0)? L"-":L"";

	CFileDialogEx saveDlg(FALSE, strArtist+ strConnector+ strSong ,strPath.GetBuffer(1),L"txt",L"原歌词文件(txt)\0*.txt;\0\0",OFN_OVERWRITEPROMPT, M()->m_hWnd);
	int ret = saveDlg.DoModal();
	if(ret == TRUE)
	{
		//将文本内容保存到文件中
		SStringW savePath = saveDlg.m_szFileName;
		if(FileOperator::WriteToUtf8File(wstring(savePath.GetBuffer(1)), wstring(strContent.GetBuffer(1))))
		{
			int nRet = _MessageBox(M()->m_hWnd, (L"原歌词已保存！\\n\\n保存路径：\\n"+savePath + L"\\n\\n是否直接选择该歌词到【歌词制作】页面？").GetBuffer(1), 
				L"提示", MB_YESNO|MB_ICONINFORMATION);
			if(nRet == IDYES)
			{
				//切换到[歌词制作]页面
				STabCtrl* tab = M()->FindChildByID2<STabCtrl>(R.id.tab_main);
				if(tab)
					tab->SetCurSel(0);

				M()->m_pageMaking->OnBtnSelectLyric1(savePath);//选择到[歌词制作]页面
			}
		}
		else
			_MessageBox(M()->m_hWnd, (L"保存操作失败！\\n\\n路径：\\n"+savePath).GetBuffer(1), L"提示", MB_OK|MB_ICONWARNING);
	}
}

//保存Lrc歌词
void CPageSearchLyric::OnBtnSaveLrcLyricPath()
{
	SStringW strContent = CStringHelper::Trim(m_editLrcLyricContent->GetWindowTextW(),L" \t\r\n");

	if(strContent.GetLength() == 0)
	{
		_MessageBox(M()->m_hWnd, L"歌词内容为空，无法保存！", L"提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	SStringW strPath = m_editLrcLyricPath->GetWindowTextW();
	SStringW strSong = CStringHelper::Trim(m_editLrcLyricName->GetWindowTextW());
	SStringW strArtist = CStringHelper::Trim(m_editLrcLyricArtist->GetWindowTextW());
	SStringW strConnector = (strSong.GetLength()!=0 && strArtist.GetLength() != 0)? L"-":L"";

	CFileDialogEx saveDlg(FALSE, strSong+ strConnector+ strArtist ,strPath.GetBuffer(1),L"lrc",L"LRC歌词文件(lrc)\0*.lrc;\0\0",OFN_OVERWRITEPROMPT, M()->m_hWnd);
	int ret = saveDlg.DoModal();
	if(ret == TRUE)
	{
		//将文本内容保存到文件中
		SStringW savePath = saveDlg.m_szFileName;
		if(FileOperator::WriteToUtf8File(wstring(savePath.GetBuffer(1)), wstring(strContent.GetBuffer(1))))
		{
			int nRet = _MessageBox(M()->m_hWnd, (L"LRC歌词已保存！\\n\\n保存路径：\\n"+savePath+ L"\\n\\n是否直接选择该歌词到【滚动预览】页面？").GetBuffer(1), 
			L"提示", MB_YESNO|MB_ICONINFORMATION);
			if(nRet == IDYES)
			{
				//切换到[滚动预览]页面
				STabCtrl* tab = M()->FindChildByID2<STabCtrl>(R.id.tab_main);
				if(tab)
					tab->SetCurSel(1);

				M()->m_pageResult->OnBtnSelectLyric2(savePath);//选择到[歌词制作]页面
			}
		}
		else
			_MessageBox(M()->m_hWnd, (L"保存操作失败！\\n\\n路径：\\n"+savePath).GetBuffer(1), L"提示", MB_OK|MB_ICONWARNING);
	}
}	


//根据猜测结果自动填充搜索关键词并开始搜索
void CPageSearchLyric::OnSearchWithGuess(SongInfoGuessResult* pGuessRes)
{
	SASSERT( pGuessRes->nResultType == 1 || pGuessRes->nResultType == 2);

	//到这里猜测结果一定包含歌词名
	m_editSearchLyricName->SetWindowTextW(pGuessRes->strSongName);

	if(pGuessRes->nResultType == 1)//还包含
	{
		m_editSearchLyricArtist->SetWindowTextW(pGuessRes->strArtist);
	}
	else
	{
		m_editSearchLyricArtist->SetWindowTextW(SStringW());
	}

	OnBtnSearchInProgram();
}


//获得界面上填写的歌曲名和歌手名
bool CPageSearchLyric::GetMusicAndArtist(SStringW &strMusicName, SStringW &strMusicArtist)
{
	strMusicName = m_editSearchLyricName->GetWindowTextW();
	strMusicArtist = m_editSearchLyricArtist->GetWindowTextW();
	
	strMusicName = CStringHelper::Trim(strMusicName);
	strMusicArtist = CStringHelper::Trim(strMusicArtist);

	if(strMusicName.GetLength() == 0)
	{
		_MessageBox(m_pMainWnd->m_hWnd, L"歌曲名不能为空", L"提示", MB_OK|MB_ICONINFORMATION);
		return false;
	}

	return true;
}


