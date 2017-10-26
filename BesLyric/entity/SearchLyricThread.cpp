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

//开始线程
bool CSearchLyricThread::Start(HWND hMainWnd, SStringW& strMusicName, SStringW& strMusicArtist)
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
DWORD WINAPI CSearchLyricThread::ProcSearch(LPVOID pParam)
{
	CSearchLyricThread* pThread = static_cast<CSearchLyricThread*>(pParam);

	pThread->m_bIsSearching = true; //进入搜索状态

	vector<LyricInfo> vecLyricInfo;			//暂时储存每一次获得的
	CAutoRefPtr<ISearcher> pSearcher;

	bool bShowUnexpectedResultTip = true; //显示意外结果提示
	SStringW strResultTip = L"";

	int nTotalCount = 0;
	for( int i= 0; i < (int)SEARCH_FROM::UNDEFINED; i++) //遍历每一种定义的歌词获取方式
	{
		pSearcher = nullptr;//在 CreateSearcher 中对 SOUI智能指针取 地址& 前，先置为空（释放上一次分配的堆对象）
		CLyricSearcherFactory::CreateSearcher((SEARCH_FROM)i, pSearcher);

		vecLyricInfo.clear();
		if(!pSearcher->SearchLyric(pThread->m_strMusicName, pThread->m_strMusicArtist, vecLyricInfo))
		{
			pSearcher->GetLastResult(strResultTip);  //保留最后一次的异常信息
		}
		else
		{
			bShowUnexpectedResultTip = false;  //只要出现过一次搜索正常的结果，就不显示异常信息
		}
		
		if(!vecLyricInfo.empty())
		{
			LyricSearchResult* pSearchResult = new LyricSearchResult;
			pSearchResult->vecLyricInfoTotal = vecLyricInfo;
			//pSearchResult->strUnexpectedResultTip = strResultTip;
			pSearchResult->bShowUnexpectedResultTip = false;			//遍历每一个搜索器的过程都不显示异常信息
			pSearchResult->bCurrentSearchDone = false;					//搜索还没结束
			pSearchResult->bAppendToList = (nTotalCount==0 ? false: true); //之前还没发过数据则需要清空列表将bAppendToList 设为false
			nTotalCount += vecLyricInfo.size();

			::SendMessage( pThread->m_hMainWnd, MSG_USER_SHOW_LYRIC_RESULT, (WPARAM)pSearchResult,0);
		}
	}

	//发送最后一次信号，结束搜索
	LyricSearchResult* pSearchResult = new LyricSearchResult;
	pSearchResult->bCurrentSearchDone = true;							//标记搜索结束
	pSearchResult->bAppendToList = nTotalCount == 0 ? false:true;//这里显示异常信息说明没有获取到任何数据，清空列表；否则，不清空列表
	pSearchResult->vecLyricInfoTotal = vector<LyricInfo>();				//发送空的数据回去
	pSearchResult->bShowUnexpectedResultTip = bShowUnexpectedResultTip;	//是否显示异常信息
	pSearchResult->strUnexpectedResultTip = strResultTip;				//异常信息
	::SendMessage( pThread->m_hMainWnd, MSG_USER_SHOW_LYRIC_RESULT, (WPARAM)pSearchResult,0);
	
	pThread->m_bIsSearching = false;

	return 0;
}
