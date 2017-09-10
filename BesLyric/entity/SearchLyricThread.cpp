#include "stdafx.h"
#include "SearchLyricThread.h"
#include "Define.h"
#include <wininet.h>					//链接网络
#pragma comment( lib, "wininet.lib" ) 

#include "..\utility\Downloader.h"
#include "..\utility\SplitFile.h"
#include "..\DlgCheckIntegrity.h"

#include "entity\LyricSearcherFactory.h"

using namespace SOUI;

CSearchLyricThread* CSearchLyricThread::ms_Singleton = NULL;


//开始线程
bool CSearchLyricThread::Start(HWND hMainWnd, SStringW& strMusicName, SStringW& strMusicArtist)
{
	m_hMainWnd = hMainWnd;
	m_strMusicName = strMusicName;			
	m_strMusicArtist = strMusicArtist;

	//启动检测线程
	m_handleThreadSearch = ::CreateThread(NULL, 0, ProcSearch, this, 0 ,NULL);

	return (m_handleThreadSearch != NULL);
}


// 线程执行地址
DWORD WINAPI CSearchLyricThread::ProcSearch(LPVOID pParam)
{
	CSearchLyricThread* pThread = static_cast<CSearchLyricThread*>(pParam);


	vector<LyricInfo> vecLyricInfoTotal;	//储存所有获得的歌词
	vector<LyricInfo> vecLyricInfo;			//暂时储存每一次获得的
	CAutoRefPtr<ISearcher> pSearcher;

	bool bShowUnexpectedResultTip = false; //显示意外结果提示

	SStringW strResultTip = L"";

	for( int i= 0; i < (int)SEARCH_FROM::UNDEFINED; i++) //遍历每一种定义的歌词获取方式
	{
		pSearcher = nullptr;//在 CreateSearcher 中对 SOUI智能指针取 地址& 前，先置为空（释放上一次分配的堆对象）
		CLyricSearcherFactory::CreateSearcher((SEARCH_FROM)i, pSearcher);

		vecLyricInfo.clear();
		if(!pSearcher->SearchLyric(pThread->m_strMusicName, pThread->m_strMusicArtist, vecLyricInfo))
		{
			bShowUnexpectedResultTip = true;
			pSearcher->GetLastResult(strResultTip);
		}

		if(!vecLyricInfo.empty())
			vecLyricInfoTotal.insert(vecLyricInfoTotal.end(), vecLyricInfo.begin(), vecLyricInfo.end());
	}

	LyricSearchResult* pSearchResult = new LyricSearchResult;
	pSearchResult->vecLyricInfoTotal = vecLyricInfoTotal;
	pSearchResult->strUnexpectedResultTip = strResultTip;
	pSearchResult->bShowUnexpectedResultTip = bShowUnexpectedResultTip;

	::SendMessage( pThread->m_hMainWnd, MSG_USER_SHOW_LYRIC_RESULT, (WPARAM)pSearchResult,0);

	return 0;
}
