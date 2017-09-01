/*
*	Copyright (C) 2017  BensonLaur
*	note: Looking up header file for license detail
*/

// LyricMaker.cpp :  实现  LyricMaker类 的接口	
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "LyricMaker.h"
#include "../utility/FileHelper.h"
#include "../utility/WinFile.h"
#include "Windows.h"
#include <mmsystem.h> 
#include "BSMessageBox.h"
#pragma comment (lib, "winmm.lib")

LyricMaker::LyricMaker():
		 m_bLastLineSpace(false),
		 m_nCurLine(0),
		 m_nTotalLine(0)
{
	m_szMusicPathName[0]= _T('\0');		
	m_szLyricPathName[0]= _T('\0');
	m_szOutputPath[0]= _T('\0');
	m_szOutputPathName[0] = _T('\0');
}

void LyricMaker::Init(CSettingPage *pSettingPage)
{
	SASSERT(NULL != pSettingPage);

	//保存主窗口对象
	m_pSettingPage = pSettingPage;
}

//设置各个路径
//设置音乐路径时，传入播放音乐需要的 消息宿主窗口的句柄
void LyricMaker::setMusicPath(LPCTSTR pathName, HWND hostWnd)
{
	_tcscpy(m_szMusicPathName,pathName);
	m_musicPlayer.init(m_szMusicPathName,hostWnd);
}
	
void LyricMaker::setLyricPath(LPCTSTR pathName)
{
	_tcscpy(m_szLyricPathName,pathName);
}
	
void LyricMaker::setm_szOutputPath(LPCTSTR pathName)
{
	_tcscpy(m_szOutputPath,pathName);
}

//重置 LyricMaker的 歌词数据为空, 生成输出的文件名
void LyricMaker::reloadMaker()
{
	m_vLyricOriginWithEmptyLine.clear();
	m_vLyricOrigin.clear();
	m_vLyricOutput.clear();

	//生成输出的文件名
	generateOutputFileName();
}

//制作开始,记录开始制作的时间
//成功播放返回true，需要转换格式返回false
bool LyricMaker::makingStart()
{
	//更新基本的行数记录的数据
	m_nTotalLine = m_vLyricOrigin.size();
	m_nCurLine = 0;

	return playMusic();
}

//为下一行歌词 标记上 网易云音乐要求的 时间轴格式，写入m_vLyricOutput中
void LyricMaker::markNextLine()
{
	int ms = m_musicPlayer.getPosition();

	//根据用户设置的时间偏移，修改得到实际记录的毫秒数
	ms -= m_pSettingPage->m_nTimeShift;
	if(ms < 0 )
		ms = 0;

	//得到[00:33.490] 形式的时间串
	TCHAR timeBuf[255];
	msToLyricTimeString(ms, timeBuf);

	//构建新的一行加入m_vLyricOutput中
	SStringT newLine(timeBuf);
	newLine.Append(m_vLyricOriginWithEmptyLine.at(m_nCurLine-1));
	newLine.Append(SStringT(_T("\n")));
	m_vLyricOutput.push_back(newLine);

	//MB(SStringT().Format(_T("%s"),m_vLyricOutput.at(m_nCurLine-1)));

	//setLastLineSpace(false);
}

//如果上一行不是空白行的话,添加
void LyricMaker::markSpaceLine()
{
	if(!isLastLineSpace())
	{
		m_nCurLine += 1;//空行也要也是1行
		m_nTotalLine += 1;

		int ms = m_musicPlayer.getPosition();

		//根据用户设置的时间偏移，修改得到实际记录的毫秒数
		ms -= m_pSettingPage->m_nTimeShift;
		if(ms < 0 )
			ms = 0;

		//得到[00:33.490] 形式的时间串
		TCHAR timeBuf[255];
		msToLyricTimeString(ms, timeBuf);

		//构建新的一行加入m_vLyricOutput中 (只有时间的空白行)
		SStringT newLine(timeBuf);
		newLine.Append(SStringT(_T("\n")));
		m_vLyricOutput.push_back(newLine);

		// 同时更新 m_vLyricOriginWithEmptyLine
		int nCount = 0;
		for(auto iter = m_vLyricOriginWithEmptyLine.begin(); iter != m_vLyricOriginWithEmptyLine.end(); iter++)
		{
			if( ++nCount ==  m_nCurLine)//到达空行应该所在的行数，在此迭代器前添加1行空行
			{
				m_vLyricOriginWithEmptyLine.insert(iter, SStringW(L""));
				break;
			}
		}

	}
}

//将 m_vLyricOutput 写入输出文件m_szOutputPathName 中
void LyricMaker::makingEnd()
{
	bool bRet = FileOperator::WriteToUtf8File(m_szOutputPathName,m_vLyricOutput);

	if(!bRet)
	{
		BSMessageBox m;
		m.MessageBoxW(NULL,SStringT().Format(_T("文件写入失败:\\n【%s】\\n!请确保文件有效"),m_szOutputPathName),
			_T("失败提示"),MB_OK|MB_ICONWARNING);
		return;
	}

	//停止播放音乐
	stopMusic();
}

//根据当前音乐位置，重新矫正储存的歌词数据
void LyricMaker::RecorrectLyricData()
{
	//撤销 可能已经被标记的数据
	int msCurrentPos = m_musicPlayer.getPosition();

	//重新填充 m_vLyricOriginWithEmptyLine
	m_vLyricOriginWithEmptyLine.clear();
	auto iterOriginLyric = m_vLyricOrigin.begin();

	m_nCurLine = 0; //重新计算 当前行
	for(auto iter = m_vLyricOutput.begin(); iter!= m_vLyricOutput.end(); iter++) //遍历已经存储的歌词
	{
		WCHAR* pBuffer = iter->GetBuffer(1);
		int m,s,ms;
		m = s = ms = 0;
		WCHAR szBuffer[MAX_WCHAR_COUNT_OF_LINE]={0};
		swscanf(pBuffer, L"[%d:%d.%d]%s", &m, &s, &ms, szBuffer); //获得歌词的时间信息
		int msTotal = m * 60 * 1000 + s * 1000 + ms;
		
		if(msTotal <= msCurrentPos)
		{
			m_nCurLine++;
			
			//重新填充 m_vLyricOriginWithEmptyLine
			if( 0 == wcslen(szBuffer))//加入空行
				m_vLyricOriginWithEmptyLine.push_back(SStringW(L""));
			else
				m_vLyricOriginWithEmptyLine.push_back(*iterOriginLyric++);
		}
		else
		{
			//删除接下来所有剩下的时间行
			while(iter!=m_vLyricOutput.end())
			{
				iter = m_vLyricOutput.erase(iter);
			}
			break;
		}
	}

	//继续 填充 未填完的歌词
	while(iterOriginLyric != m_vLyricOrigin.end())
			m_vLyricOriginWithEmptyLine.push_back(*iterOriginLyric++);

	//重新计算 m_nTotalLine
	m_nTotalLine = m_vLyricOriginWithEmptyLine.size();
}


//获得原始歌词(包括插入的空行) 
// nPos 从第1行开始
SStringW LyricMaker::GetOriginLyricAt(std::size_t nPos)
{
	SASSERT( nPos > 0 && nPos <= m_vLyricOriginWithEmptyLine.size() && L"nPos 不在有效范围");

	return m_vLyricOriginWithEmptyLine[nPos-1];
}


//获得当前的输出 文件名
void LyricMaker::getOutputFileName(TCHAR* name, int lenth)
{
	_tcscpy(name,outputFileName);
}

//获得当前的输出 路径文件名
void LyricMaker::getm_szOutputPathName(TCHAR* name, int lenth)
{
	_tcscpy(name,m_szOutputPathName);
}

//结束音乐播放
void LyricMaker::stopMusic()
{
	m_musicPlayer.closeStop();
}

void LyricMaker::setLastLineSpace(bool value)
{
	this->m_bLastLineSpace = value;
}

//上一行是否为空白行
bool LyricMaker::isLastLineSpace()
{
	// 分析 m_vLyricOutput 数据，看最后一行数据是否只有时间标签
	// 如若是，为是空行
	bool bIsLastLineSpace = false;

	if(m_vLyricOutput.begin() != m_vLyricOutput.end())
	{
		WCHAR* pBuffer = (m_vLyricOutput.end()-1)->GetBuffer(1);
		int m,s,ms;
		WCHAR szBuffer[MAX_WCHAR_COUNT_OF_LINE]={0};
		swscanf(pBuffer, L"[%d:%d.%d]%s", &m, &s, &ms,szBuffer); //获得歌词的时间信息
		if(wcslen(szBuffer)==0) //szBuffer 没有包含整行歌词，但是如果有数据，长度一定大于0
			bIsLastLineSpace = true;
	}

	return bIsLastLineSpace;
}

//从文件获取每行歌词的集合向量
vector<SStringT> LyricMaker::getLyricOrigin(File& encodingFile)
{
	vector<SStringW> lyrics;

	FileOperator::ReadAllLinesW(encodingFile, &lyrics);
	
	return lyrics;
}


//根据m_szMusicPathName 的文件名得到歌词文件名，并更新outputFileName 和 m_szOutputPathName的值
void LyricMaker::generateOutputFileName()
{
	int len = _tcslen(m_szMusicPathName);
	if(len==0)
		return;

	// 保存最后一个'\\'的位置
	int i,j;
	int pos; 
	for(i=0;i< len;i++)
		if(m_szMusicPathName[i]==_T('\\'))
			pos = i;

	//复制歌名到 outputFileName ，不包括后缀
	for(i=pos+1,j=0;m_szMusicPathName[i]!=_T('.');i++,j++)
	{
		outputFileName[j] = m_szMusicPathName[i];
	}
	outputFileName[j] = _T('\0');

	//补充完输出完整的文件名
	_tcscat(outputFileName,_T(".lrc"));

	//初始化 m_szOutputPathName
	_tcscpy(m_szOutputPathName,m_szOutputPath);
	_tcscat(m_szOutputPathName,_T("\\"));
	_tcscat(m_szOutputPathName,outputFileName);
}

//将毫秒差值时间 转换为歌词时间格式 “[00:33.490] Look at the stars”
//以 [00:33.490] 格式输出到 timeBuf
void LyricMaker::msToLyricTimeString(int ms, LPTSTR timeBuf)
{
	int minutes = ms/60000;
	ms = ms%60000;
	double seconds = ms*1.0/1000;
	_stprintf(timeBuf,_T("[%02d:%06.3lf]"),minutes,seconds);
}

//播放音乐
bool LyricMaker::playMusic()
{
	if(m_musicPlayer.openTest() == false)
	{
		return false;
	}

	m_musicPlayer.openStart();
	return true;
}

