#include "stdafx.h"
#include "PageSearchNcmID.h"
#include "controlEx\LyricListView.h"
#include "utility\WinDialog.h"
#include "utility\WinFile.h"
#include "utility\StringHelper.h"
#include "entity\SearchIDThread.h"
#include "entity\NcmIDManager.h"
#include "utility\Downloader.h"

#include "entity\LoadAndCheckNcmIDThread.h"

CPageSearchNcmID::CPageSearchNcmID()
{
	m_pMainWnd = NULL;		/* 主窗口指针 */

	m_window_search_ncm_id_tip = NULL;
	m_editSongFileName = NULL;
	m_editNcmID = NULL;
	m_editSearchName= NULL;
	m_editSearchArtist= NULL;

	
	m_txtSearchNameTip =NULL;
	m_txtSearchArtistTip  =NULL;
	m_wndSearchArtistTip =NULL;
	m_txtSearchResultTip =NULL;
	m_txtIsSearchingIDTip =NULL;
}

//初始化设置页面
void CPageSearchNcmID::Init(SHostWnd *pMainWnd)
{
	SASSERT(NULL != pMainWnd);

	//保存主窗口对象
	m_pMainWnd = pMainWnd;
	
	//获得代码中常用的控件的指针
	m_window_search_ncm_id_tip = m_pMainWnd->FindChildByID2<SWindow>(R.id.window_search_ncm_id_tip);
	m_editSongFileName = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_song_file_name);
	m_editNcmID = m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_ncm_id);
	
	m_editSearchName= m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_search_ncm_id_name);
	m_editSearchArtist= m_pMainWnd->FindChildByID2<SEdit>(R.id.edit_search_ncm_id_artist);
	
	
	m_txtSearchNameTip = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_search_ncm_id_name_tip);
	m_txtSearchArtistTip  = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_search_ncm_id_artist_tip);
	m_wndSearchArtistTip = m_pMainWnd->FindChildByID2<SWindow>(R.id.window_search_ncm_id_artist_tip);
	m_txtSearchResultTip = m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_search_ncm_id_result_tip);
	m_txtIsSearchingIDTip =m_pMainWnd->FindChildByID2<SStatic>(R.id.txt_is_searching_ncm_id_tip);


	SASSERT(m_window_search_ncm_id_tip != NULL);
	SASSERT(m_editSongFileName != NULL);
	SASSERT(m_editNcmID != NULL);
	SASSERT(m_editSearchName != NULL);
	SASSERT(m_editSearchArtist != NULL);
	

	m_window_search_ncm_id_tip->SetVisible(FALSE, TRUE);	//隐藏搜索提示

	//多列listview
    CLyricListView * pMcListView = m_pMainWnd->FindChildByName2<CLyricListView>("mclv_ncm_id");
    if(pMcListView)
    {
        IMcAdapter *pAdapter = m_IDListAdapter =new CSongIDMcAdapterFix;
		m_IDListAdapter->AttachMainDlgPointer(M());
        pMcListView->SetAdapter(pAdapter);
        pAdapter->Release();
    }

	//用线程加载和检测NCMID ,之所以使用线程，是因为该操作联网耗时
	CLoadAndCheckNcmIDThread::getSingleton().Start();
}

//获得主窗口对象
CMainDlg* CPageSearchNcmID::M()
{
	return (CMainDlg*)m_pMainWnd;
}


/*
*	响应函数
*/

//返回主要窗口页面
void CPageSearchNcmID::OnBtnReturnMain()
{
	M()->FindChildByID2<STabCtrl>(R.id.tab_content_container)->SetCurSel(0); //返回
}


//使用文件路径的文件名填充歌曲文件名编辑控件
void CPageSearchNcmID::SetSongFileNameEditWithPath(wstring strFilePath)
{
	wstring strFileName = L"";

	auto indexSlash = strFilePath.find_last_of(L'\\');
	if(indexSlash != wstring::npos)
	{
		wstring strFile = strFilePath.substr(indexSlash+1);
		if(strFile.size() != 0)
		{
			auto indexDot = strFile.find_last_of(L'.');
			strFileName = strFile.substr(0,indexDot);
		}
	}

	//使用 strFileName 填充界面编辑框
	m_editSongFileName->SetWindowTextW(strFileName.c_str());

	//如果该文件名已经匹配了ID，则显示ID到对应的编辑框
	wstring strID = L"";
	CNcmIDManager::GetInstance()->FindID(strFileName, strID); //如果查找到结果，则更新到strID
		
	m_editNcmID->SetWindowTextW(strID.c_str());
}


//选择ID作为当前歌名的ncm ID
void CPageSearchNcmID::OnBtnSelectID()
{
	wstring strName = m_editSongFileName->GetWindowTextW().Trim(' ').GetBuffer(1);
	wstring strID = m_editNcmID->GetWindowTextW().Trim(' ').GetBuffer(1);

	if(strID.size() == 0)
	{
		_MessageBox(M()->m_hWnd, L"ID不能为空", L"提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	//搜索该ID看是否该ID有效（根据查其歌词的结果，判断是否有效）
	bool bValid = false;
	if(CNcmIDManager::GetInstance()->CheckIDValidity(strID,bValid))
	{
		if(!bValid)
		{
			_MessageBox(M()->m_hWnd, L"当前填写ID为无效ID 或 其对应歌曲网易没有版权，匹配ID失败", L"提示", MB_OK|MB_ICONINFORMATION);
			return;
		}
	}
	else
	{
		_MessageBox(M()->m_hWnd, L"当前无法查询，匹配ID失败\\n可能是网络连接问题", L"提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	//如果有旧的ID，看看是否和之前相同，不同的话，需要删除mp3目录下的 mp3文件，以后面重新下载，不删除的话，还是会播放原来的mp3
	wstring strOldID = L"";
	bool bHasOld = CNcmIDManager::GetInstance()->FindID(strName,strOldID);
	if(bHasOld)
	{
		if(strOldID != strID)
		{
			wstring strMp3FloderPath = FileHelper::GetCurrentDirectoryStr() + TEMP_MP3_FLODER_NAME ;
			wstring strMp3FilePath = strMp3FloderPath + L"\\" + strName + L".mp3" ;
			if(FileHelper::CheckFileExist(strMp3FilePath))
				_wremove(strMp3FilePath.c_str());
		}
	}

	//添加键值对
	CNcmIDManager::GetInstance()->InsertNcmIDPair(strName, strID);

	if(!CNcmIDManager::GetInstance()->SaveDataPairs())
	{
		_MessageBox(M()->m_hWnd, L"无法成功保存数据，添加键值对失败", L"提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	OnBtnReturnMain();
}



//搜索ID
void CPageSearchNcmID::OnBtnSearch()
{
	if(CSearchIDThread::getSingleton().IsSearching())
	{
		_MessageBox(NULL,L"搜索模块正被占用，请等待.....",L"等待提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	SStringW strMusicName = L"";
	SStringW strMusicArtist = L"";

	if(!GetMusicAndArtist(strMusicName, strMusicArtist))
		return;

	//显示正在搜索
	m_txtSearchNameTip->SetWindowTextW(L"“"+ strMusicName  + L"”");
	m_txtSearchArtistTip->SetWindowTextW(L"“"+ strMusicArtist  + L"”");

	m_wndSearchArtistTip->SetVisible( strMusicArtist.GetLength() == 0 ? FALSE:TRUE, TRUE);
	m_txtSearchResultTip->SetVisible(FALSE,TRUE);
	m_txtIsSearchingIDTip->SetVisible(TRUE,TRUE);
	
	m_window_search_ncm_id_tip->SetVisible(TRUE, TRUE); //显示提示

	//隐藏按钮，不让继续搜索
	m_pMainWnd->FindChildByID2<SButton>(R.id.btn_search_ncm_id)->EnableWindow(FALSE, TRUE);
	

	//开启搜索ID线程
	CSearchIDThread::getSingleton().Start(M()->m_hWnd, strMusicName,strMusicArtist);
}

void CPageSearchNcmID::ShowIDResult(IDSearchResult* pResult)
{
	//删除列表中所有的已有数据
	m_IDListAdapter->DeleteAllItem();

	SStringW strResultTip = L"";
	if(pResult->bShowUnexpectedResultTip)
		strResultTip = pResult->strUnexpectedResultTip;

	//显示搜索结果
	m_txtSearchResultTip->SetWindowTextW(SStringW().Format(L"，找到%d个ID。 %s",pResult->vecIDInfoTotal.size() , strResultTip.GetBuffer(1) ));
	m_txtSearchResultTip->SetVisible(TRUE,TRUE);

	for(auto iter = pResult->vecIDInfoTotal.begin(); iter != pResult->vecIDInfoTotal.end(); iter++)
		m_IDListAdapter->AddItem( iter->strSong, iter->strArtist, iter->strID);
	m_IDListAdapter->notifyDataSetChanged();
	

	//恢复按钮，可以下一次继续搜索
	m_pMainWnd->FindChildByID2<SButton>(R.id.btn_search_ncm_id)->EnableWindow(TRUE, TRUE);
	
	m_txtIsSearchingIDTip->SetVisible(FALSE,TRUE); //隐藏正在搜索提示
	
}

//根据猜测结果自动填充搜索关键词并开始搜索
void CPageSearchNcmID::OnSearchWithGuess(SongInfoGuessResult* pGuessRes)
{
	SASSERT( pGuessRes->nResultType == 1 || pGuessRes->nResultType == 2);

	//到这里猜测结果一定包含歌词名
	m_editSearchName->SetWindowTextW(pGuessRes->strSongName);

	if(pGuessRes->nResultType == 1)//还包含
	{
		m_editSearchArtist->SetWindowTextW(pGuessRes->strArtist);
	}
	else
	{
		m_editSearchArtist->SetWindowTextW(SStringW());
	}

	OnBtnSearch();
}


//获得界面上填写的歌曲名和歌手名
bool CPageSearchNcmID::GetMusicAndArtist(SStringW &strMusicName, SStringW &strMusicArtist)
{
	strMusicName = m_editSearchName->GetWindowTextW();
	strMusicArtist = m_editSearchArtist->GetWindowTextW();
	
	strMusicName = CStringHelper::Trim(strMusicName);
	strMusicArtist = CStringHelper::Trim(strMusicArtist);

	if(strMusicName.GetLength() == 0)
	{
		_MessageBox(m_pMainWnd->m_hWnd, L"歌曲名不能为空", L"提示", MB_OK|MB_ICONINFORMATION);
		return false;
	}

	return true;
}


