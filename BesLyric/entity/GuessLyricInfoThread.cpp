#include "stdafx.h"
#include "GuessLyricInfoThread.h"
#include "Define.h"
#include <wininet.h>					//链接网络
#pragma comment( lib, "wininet.lib" ) 

#include "..\utility\Downloader.h"
#include "..\utility\SplitFile.h"
#include "..\utility\StringHelper.h"
#include "..\DlgCheckIntegrity.h"

#include "..\utility\HttpRequest.h"
#include "..\utility\UrlEncoding.h"
#include "..\lib\json\json.h"

#include "entity\LyricSearcherFactory.h"


using namespace SOUI;

//开始线程
bool CGuessLyricInfoThread::Start(HWND hMainWnd, SStringW& strMusicPath, bool isForNcmID)
{
	if(m_bIsGuessing)
	{
		_MessageBox(NULL,L"正在分析歌曲信息当中.....",L"等待提示", MB_OK|MB_ICONINFORMATION);
		return true;
	}

	m_hMainWnd = hMainWnd;
	m_strMusicPath = strMusicPath;
	m_isForNcmID = isForNcmID;

	//启动线程
	m_handleThreadGuess = ::CreateThread(NULL, 0, ProcGuessLyricInfo, this, 0 ,NULL);

	return (m_handleThreadGuess != NULL);
}


//从mp3文件中获取猜测结果
bool CGuessLyricInfoThread::GetGuessResultFromMp3(SongInfoGuessResult* pGuessRes )
{
	pGuessRes->strArtist = SStringW();
	pGuessRes->strSongName = SStringW();

	//读取文件内容 到 m_pFileData 指向的内存中
	FILE *pf;
	errno_t err = _tfopen_s(&pf, m_strMusicPath, L"rb");
	if (err != 0)
		return false;

	fseek(pf, 0, SEEK_END);
	int nFileSize = ftell(pf);										//获取文件大小
	rewind(pf);

	PBYTE pFileData = (PBYTE)malloc(sizeof(BYTE)* nFileSize);			//分配整个文件的大小，获取文件数据块的指针
	if (pFileData == NULL)
		return false;

	if (fread(pFileData, 1, nFileSize, pf) != nFileSize)		//文件拷贝到m_pFileData中
		return false;

	fclose(pf);

	//分析标题和歌手
	int nGetCount = 0;//获得数据数
	for(int i=0;i <nFileSize; i++)
	{
		if(nGetCount >=2 )
			break;
		//标签结构
		//char FrameID[4];   
		//char Size[4];   
		//char Flags[2];  
		
		union _USize{byte bit[4]; int nSize;} USize;
			
		if( pFileData[i] == 'T' && pFileData[i+1] == 'I' && pFileData[i+2] == 'T' && pFileData[i+3] == '2') //标题
		{
			USize.bit[0] = pFileData[i+4+3];
			USize.bit[1] = pFileData[i+4+2];
			USize.bit[2] = pFileData[i+4+1];
			USize.bit[3] = pFileData[i+4+0];
			
			int nSize = USize.nSize;  

			if(nSize <= 3 || (nSize-3)%2!=0) //这里标签大小大于3时才有内容，且只处理去除bom头后还剩下偶数个
				continue;

			BYTE szBom[3];
			char* pszTitle = new char[nSize-3];
			
			memcpy(szBom, pFileData+i+4+6, 3);
			memcpy(pszTitle, pFileData+i+4+9, nSize-3);

			//if(szBom[0] == 0x01 && szBom[1] == 0xff && szBom[2] == 0xfe){}
			
			pGuessRes->strSongName = S_CW2W( SStringW( (wchar_t*)pszTitle, (nSize-3)/2));

			delete pszTitle;
			nGetCount++;
		}
		else if(pFileData[i] == 'T' && pFileData[i+1] == 'P' && pFileData[i+2] == 'E' && pFileData[i+3] == '1')//作者
		{
			USize.bit[0] = pFileData[i+4+3];
			USize.bit[1] = pFileData[i+4+2];
			USize.bit[2] = pFileData[i+4+1];
			USize.bit[3] = pFileData[i+4+0];
			
			int nSize = USize.nSize;

			if(nSize <= 3 || (nSize-3)%2!=0) //这里标签大小大于3时才有内容，且只处理去除bom头后还剩下偶数个
				continue;

			BYTE szBom[3];
			char* pszArtist = new char[nSize-3];
			
			memcpy(szBom, pFileData+i+4+6, 3);
			memcpy(pszArtist, pFileData+i+4+9, nSize-3);

			//if(szBom[0] == 0x01 && szBom[1] == 0xff && szBom[2] == 0xfe){}
			
			pGuessRes->strArtist = S_CW2W( SStringW( (wchar_t*)pszArtist, (nSize-3)/2));

			delete pszArtist;
			nGetCount++;
		}
	}

	if(pGuessRes->strSongName.GetLength() == 0)
	{
		pGuessRes->nResultType = 3; //没有获取到数据
		return false;
	}
	else
	{
		if(pGuessRes->strArtist.GetLength() == 0)
			pGuessRes->nResultType = 2; //只获取到歌曲名
		else
			pGuessRes->nResultType = 1;//获取到的歌曲名和作家名
	}

	return true;
}


//从网易云接口分析前个字符串是否为歌手
bool CGuessLyricInfoThread::GuessWhetherFirstArtistFromNetease(SStringW& strFirst, SStringW& strSecond)
{
	CHttpRequest httpRequest;
	wstring wstrFirst = S_CW2W(strFirst).GetBuffer(1);
	wstring wstrSecond = S_CW2W(strSecond).GetBuffer(1);

	string strFirstUrl = CUrlEncodinig().UrlUTF8(S_CW2A(strFirst).GetBuffer(1));
	string strSecondUrl = CUrlEncodinig().UrlUTF8(S_CW2A(strSecond).GetBuffer(1));

	wstring strRes;
	if(!httpRequest.Post( "music.163.com/api/search/get/web", 
								"csrf_token=&s="+strFirstUrl+"+"+ strSecondUrl +"&type=1&offset=0&total=True&limit=8",
								strRes))
	{
		//L"网络连接失败，无法获取歌词索引数据",默认返回true，猜测第一个字符串歌手名
		return true;
	}
	
	strRes = strRes.substr( strRes.find_first_of('{'), strRes.find_last_of('}') - strRes.find_first_of('{')+1);

	//获取id列表
	vector< SONGINFO > vecSongList;
	if(!SearcherNetEaseCloud::GetSongListFromJson(strRes, vecSongList))
	{
		//L"网易云歌词数据格式异常，无法解析数据";,默认返回true，猜测第一个字符串歌手名
		return true;
	}
	
	int nFirstArtistCount;
	int nFirstSongCount;
	int nSecondArtistCount;
	int nSecondSongCount;

	nFirstArtistCount = nFirstSongCount = nSecondArtistCount = nSecondSongCount = 0;

	for(auto iter = vecSongList.begin(); iter != vecSongList.end(); iter++)
	{	
		if( wstring::npos != iter->strArtists.find(wstrFirst))//第一个串包含于结果歌词名
			nFirstArtistCount++;
		if( wstring::npos != iter->strSong.find(wstrFirst))//第一个串包含于结果歌手名
			nFirstSongCount++;
		if( wstring::npos != iter->strArtists.find(wstrSecond))//第二个串包含于结果歌词名
			nSecondArtistCount++;
		if( wstring::npos != iter->strSong.find(wstrSecond))//第二个串包含于结果歌手名
			nSecondSongCount++;
	}
	
	//评估2个字符串的得分
	int nPointFirstGot = 0;
	int nPointSecondGot = 0;

	if( nFirstArtistCount > nFirstSongCount)
		nPointFirstGot++;
	if( nSecondArtistCount > nSecondSongCount)
		nPointSecondGot++;

	if(nPointFirstGot >= nPointSecondGot)
		return true;
	else
		return false;
}


// 线程执行地址
DWORD WINAPI CGuessLyricInfoThread::ProcGuessLyricInfo(LPVOID pParam)
{
	CGuessLyricInfoThread* pThread = static_cast<CGuessLyricInfoThread*>(pParam);

	pThread->m_bIsGuessing = true;//标记猜测状态

	//获得文件名的名称 和 后缀
	SStringW strName;
	SStringW strExt;

	int nLastDot = -1;
	int nLastSlash = -1;
	for(int i = pThread->m_strMusicPath.GetLength()-1; i >=0 ; i--)
	{
		if(pThread->m_strMusicPath.GetAt(i) == L'.')
		{
			nLastDot = i;
		}
		else if(pThread->m_strMusicPath.GetAt(i) == L'\\')
		{
			nLastSlash = i;
			break;
		}
	}

	SASSERT( nLastDot != -1 && nLastSlash != -1 && "音乐路径出错");
	strExt = pThread->m_strMusicPath.Right( pThread->m_strMusicPath.GetLength() - nLastDot-1);
	strName = pThread->m_strMusicPath.Right( pThread->m_strMusicPath.GetLength() - nLastSlash-1).Left(nLastDot - nLastSlash-1);
	
	SongInfoGuessResult* pGuessRes = new SongInfoGuessResult(); //储存猜测的结果

	//如果是mp3，读取读取并分析文件
	bool bHasGuessed = false;
	if(strExt.Compare(SStringW(L"mp3")) == 0)
	{
		if(pThread->GetGuessResultFromMp3( pGuessRes ))
			bHasGuessed = true;
	}

	//如果没有猜测出结果，根据文件名猜测结果
	if(!bHasGuessed)
	{
		//从文件得到"-" 2个字段
		int nIndex = strName.Find( L"-");
		if(nIndex == -1)
		{
			//如果只有1个字段，直接作为歌曲名返回
			pGuessRes->nResultType = 2;
			pGuessRes->strSongName = strName;
		}
		else
		{
			SStringW s1 = CStringHelper::Trim( strName.Left( nIndex ));
			SStringW s2 = CStringHelper::Trim( strName.Right( strName.GetLength() - nIndex -1 ));

			//如果得到2个字段，则通过网易云接口搜索得到的结果，确定哪个是文件名，哪个是歌手名
			if(pThread->GuessWhetherFirstArtistFromNetease( s1, s2))
			{
				pGuessRes->strArtist = s1;
				pGuessRes->strSongName = s2;		
			}
			else{
				pGuessRes->strArtist = s2;
				pGuessRes->strSongName = s1;
			}

			pGuessRes->nResultType = 1;
		}
	}

	if(!pThread->m_isForNcmID) //原先，发送给搜索歌词用
		::SendMessage( pThread->m_hMainWnd, MSG_USER_SEARCH_WITH_GUESS_RESULT, (WPARAM)pGuessRes,0); 
	else //拓展，发送给搜索ID用
		::SendMessage( pThread->m_hMainWnd, MSG_USER_SEARCH_NCM_WITH_GUESS_RESULT, (WPARAM)pGuessRes,0); 
	//MSG_USER_SEARCH_WITH_GUESS_RESULT

	pThread->m_bIsGuessing = false;

	return 0;
}
