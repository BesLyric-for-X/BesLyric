#include "stdafx.h"
#include "SearchLyricThread.h"
#include "Define.h"
#include <wininet.h>					//链接网络
#pragma comment( lib, "wininet.lib" ) 

#include "..\utility\Downloader.h"
#include "..\utility\SplitFile.h"
#include "..\DlgCheckIntegrity.h"
#include "SearchIDThread.h"
#include "SearcherNetEaseCloud.h"

using namespace SOUI;

//开始线程
bool CSearchIDThread::Start(HWND hMainWnd, SStringW& strMusicName, SStringW& strMusicArtist)
{
	if(m_bIsSearching)
	{
		_MessageBox(NULL,L"搜索模块正被占用，请等待.....",L"等待提示", MB_OK|MB_ICONINFORMATION);
		return true;
	}

	m_hMainWnd = hMainWnd;
	m_strMusicName = strMusicName;			
	m_strMusicArtist = strMusicArtist;

	//启动检测线程
	m_handleThreadSearch = ::CreateThread(NULL, 0, ProcSearch, this, 0 ,NULL);

	return (m_handleThreadSearch != NULL);
}


// 线程执行地址
DWORD WINAPI CSearchIDThread::ProcSearch(LPVOID pParam)
{
	CSearchIDThread* pThread = static_cast<CSearchIDThread*>(pParam);

	pThread->m_bIsSearching = true; //进入搜索状态

	vector<IDInfo> vecLyricInfo;			//暂时储存每一次获得的


	bool bShowUnexpectedResultTip = true; //显示意外结果提示
	SStringW strResultTip = L"";

	
	vector< SONGINFO > vecSongList;
	SearcherNetEaseCloud::GetSongListWithNameAndArtist(pThread->m_strMusicName, pThread->m_strMusicArtist, vecSongList, strResultTip);


	for(auto iter = vecSongList.begin(); iter != vecSongList.end(); iter++)
	{
		IDInfo info;
		info.strSong = iter->strSong;
		info.strArtist = iter->strArtists;
		info.strID = SStringW().Format(L"%d",iter->nID);
		vecLyricInfo.push_back(info);
	}

	IDSearchResult* pSearchResult = new IDSearchResult;
	pSearchResult->vecIDInfoTotal = vecLyricInfo;				
	pSearchResult->bShowUnexpectedResultTip = bShowUnexpectedResultTip;	//是否显示异常信息
	pSearchResult->strUnexpectedResultTip = strResultTip;				//异常信息

	::SendMessage( pThread->m_hMainWnd, MSG_USER_SHOW_ID_RESULT, (WPARAM)pSearchResult,0);
	
	pThread->m_bIsSearching = false;

	return 0;
}
