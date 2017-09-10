// LyricHandler.cpp
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LrcHandler.h"
#include "../utility/WinDialog.h"
#include "../utility/WinFile.h"
#include "../utility/StringHelper.h"
#include <algorithm>
#include <ctype.h>


TimeLineInfo::TimeLineInfo(SStringT timeLine)
{
	//初始化类的基本成员的信息
	m_strTimeLine = timeLine;
	int pos = m_strTimeLine.Find(_T(']'));
	SStringT timeStr = m_strTimeLine.Left(pos+1);

	m_strLine = m_strTimeLine.Right(m_strTimeLine.GetLength()-pos-1);
	m_nmSesonds = TimeStringToMSecond(timeStr,timeStr.GetLength());

	if(m_strLine.GetLength()==0)
		m_bIsEmptyLine = true;
	else
		m_bIsEmptyLine = false;
}


//从时间标签字符串得到对应的毫秒时间
int TimeLineInfo::TimeStringToMSecond(LPCTSTR timeStr, int length)
{
	//TODO：异常抛出处理

	TCHAR szMinute[5];	//分钟
	TCHAR szSecond[10];	//秒

	int i,j;
	//得到: 的位置
	int pos1 = -1,pos3 = length-1;
	for(i=0; i<length; i++)
	{
		if(_T(':') == timeStr[i])
			pos1 = i;
	}

	//得到三个时间段的字符串
	for(j=0,i=1; i < pos1; i++,j++)
		szMinute[j] = timeStr[i];
	szMinute[j] = _T('\0');

	for(j=0, i = pos1+1; i<pos3; i++, j++)
		szSecond[j] = timeStr[i];
	szSecond[j] = _T('\0');

	int millisecond = _wtoi(szMinute)  * 60000 + (int)(_wtof(szSecond) * 1000 );
	return millisecond;
}

//返回无符号十进制串对应的数字（十进制串可以是 023、12、04 等形式，数值为0到999）
int TimeLineInfo::DecStrToDecimal(LPCTSTR timeStr)
{
	int bit = _tcslen(timeStr);
	int result = 0;
	for(int i=0; i< bit; i++)
	{
		result *= 10;
		result += timeStr[i]-_T('0');
	}
	return result;
}


//使用读取的的文件的所有行初始化Lrc处理器
LrcProcessor::LrcProcessor(vector<SStringW> vecLines, bool bDealOffset)
{
	ProcessData(vecLines, bDealOffset);
}


//使用整个LRC文本数据初始化Lrc处理器
LrcProcessor::LrcProcessor(SStringW AllContent, bool bDealOffset)
{
	//将AllContent 分割成行
	vector<SStringW> vecLines;
	CStringHelper::SplitStringByToLines( AllContent, L'\n', vecLines);
	
	ProcessData(vecLines, bDealOffset);
}

//当前歌词文件是否有效
bool LrcProcessor::IsLrcFileValid()
{
	return m_bIsLrcFileValid;
}

//获得网易云音乐支持的格式
vector<TimeLineInfo> LrcProcessor::GetNeteaseLrc()
{
	return m_vecNeteaseLrc;
}

//生成Lrc文件
bool LrcProcessor::GenerateNeteaseLrcFile(SStringW strFilePath)
{
	vector<SStringW> vecLines;

	WCHAR szTimeBuf[MAX_BUFFER_SIZE/2];
	for(auto iter = m_vecNeteaseLrc.begin(); iter != m_vecNeteaseLrc.end(); iter++)
	{
		int ms = iter->m_nmSesonds;

		szTimeBuf[0] = L'\0';
		int minutes = ms/60000;
		ms = ms%60000;
		double seconds = ms*1.0/1000;
		_stprintf(szTimeBuf,_T("[%02d:%06.3lf]"),minutes,seconds);

		SStringW newLine = szTimeBuf;
		newLine.Append( iter->m_strLine );
		newLine.Append(L"\n");

		vecLines.push_back(newLine);
	}

	return FileOperator::WriteToUtf8File( strFilePath.GetBuffer(1), vecLines);
}


//处理 vector 行数据
void LrcProcessor::ProcessData(const vector<SStringW>& vecLines, bool bDealOffset)
{
	m_bDealOffset = bDealOffset;
	m_strTitle = L"";
	m_strArtist = L"";
	m_strAlbum = L"";
	m_strEditor = L"";
	m_nOffset = 0;

	m_bIsLrcFileValid = true;

	//遍历处理所有行
	for(auto iter = vecLines.begin(); iter != vecLines.end(); iter++)
	{
		if( false == DealOneLine( *iter ) )
		{
			m_bIsLrcFileValid = false;	
			return;
		}
	}

	//对所有歌词进行排序
	sort(m_vecNeteaseLrc.begin(), m_vecNeteaseLrc.end());

	//去除重复的空行，连续出现多个空行时，保留第一个空行的时间
	bool isLastLineEmpty = false;
	for(auto iter = m_vecNeteaseLrc.begin(); iter != m_vecNeteaseLrc.end(); )
	{
		if(iter->m_strLine.GetLength() == 0)
		{
			if(isLastLineEmpty)
			{
				iter = m_vecNeteaseLrc.erase(iter);
			}
			else
			{
				iter++;
				isLastLineEmpty = true;
			}
		}
		else
		{
			iter++;
			isLastLineEmpty = false;
		}
	}

	//对整体的时间进行偏移
	if(m_bDealOffset)
	{
		for(auto iter = m_vecNeteaseLrc.begin(); iter != m_vecNeteaseLrc.end(); iter++)
		{
			iter->m_nmSesonds = iter->m_nmSesonds - m_nOffset < 0 ? 0 : iter->m_nmSesonds - m_nOffset;
		}
	}
}

//处理1行
bool LrcProcessor::DealOneLine(const SStringW& _strLine)
{
	SStringW strLine = _strLine;
	int nPos1 = -1;
	int nPos2 = -1;
	if( -1 != strLine.Find(L"[ti:"))//标题
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strTitle = strLine.Left(nPos2);
		m_strTitle = m_strTitle.Right(m_strTitle.GetLength()-1 - nPos1);
	}
	else if( -1 != strLine.Find(L"[ar:"))//艺术家
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strArtist = strLine.Left(nPos2);
		m_strArtist = m_strArtist.Right(m_strArtist.GetLength()-1 - nPos1);
	}
	else if( -1 != strLine.Find(L"[al:"))//专辑
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strAlbum = strLine.Left(nPos2);
		m_strAlbum = m_strAlbum.Right(m_strAlbum.GetLength()-1 - nPos1 );
	}
	else if( -1 != strLine.Find(L"[by:"))//歌词制作者
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		m_strEditor = strLine.Left(nPos2);
		m_strEditor = m_strEditor.Right(m_strEditor.GetLength()-1 - nPos1);
	}
	else if( -1 != strLine.Find(L"[offset:"))
	{
		nPos1 = strLine.Find(L":");
		nPos2 = strLine.Find(L"]");
		SStringW m_strOffset = strLine.Left(nPos2);
		m_strOffset = m_strOffset.Right(m_strOffset.GetLength()-1 - nPos1);
		m_nOffset = _wtoi( m_strOffset.GetBuffer(1));
	}
	else //其他情况认为是标记时间的行
	{
		wstring sLine = strLine.GetBuffer(1);
		//分析是否为有效的时间标签行（排除其他自定义标签，对没有时间标签的歌词，添加时间[0:0.000]兼容处理）
		if(!MakeSureTimeLineValid(sLine))
			return true;

		wstring sTimeLabel = L"";
		wstring sLyric = L"";
		int nPosColon = -1;
		sLyric = sLine.substr( sLine.find_last_of(L']') + 1);
		do{
			nPos1 = sLine.find_first_of(L'[');
			nPosColon =  sLine.find_first_of(L':');
			nPos2 =  sLine.find_first_of(L']');
			if(nPos1 ==0 && nPos1 < nPosColon && nPosColon < nPos2) // strLine头还有时间标签
			{
				sTimeLabel = sLine.substr(nPos1, nPos2+1);

				TimeLineInfo timeInfoLine(SStringW( (sTimeLabel + sLyric).c_str()));

				m_vecNeteaseLrc.push_back(timeInfoLine); //添加一行到网易云歌词

				sLine = sLine.substr(nPos2+1); //去除前面标签
			}
			else
				break;
		}
		while( true);
	}

	return true;
}

//按歌词时间升序比较
bool LrcProcessor::CompareWithIncreaceTime(const TimeLineInfo & L1, const TimeLineInfo & L2)
{
	return L1.m_nmSesonds < L2.m_nmSesonds;
}

//保证时间行有效（无法保证则返回 false）
bool LrcProcessor::MakeSureTimeLineValid(wstring& line)
{
	wstring strLine = line;
	//查看是否有标签 [:]
	size_t pos1 = strLine.find_first_of(L'[');
	size_t pos2 = strLine.find_first_of(L':');
	size_t pos3 = strLine.find_first_of(L']');
	
	//对于没有标签的情况，当成歌词，添加 [0:0.000] 时间
	if(pos1 == wstring::npos && pos2 == wstring::npos && pos2 == wstring::npos )
	{
		if(CStringHelper::GetTrimLength(line, L" \t\r\n") == 0)//为空串
		{
			return false;
		}

		//在最后的时候 strLine 可能没有 \n,添加一个回车符号
		strLine = CStringHelper::Trim(strLine, L"\r\n") + L"\n";

		line = L"[0:0.000]" + strLine;
		return true;
	}

	if( pos1 == wstring::npos || pos2 == wstring::npos || pos2 == wstring::npos ||
		pos1 > pos2 || pos1 > pos3 || pos2 > pos3 ||
		pos1 +1 == pos2 || pos2+1 == pos3
		)	//非法行，无法处理
		return false;

	//存在标签，自定义和无效时间标签都 无法继续处理；只有有效的时间标签，才可以
	wstring strMinute = strLine.substr( pos1+1,pos2 - pos1 -1);
	wstring strSecond = strLine.substr( pos2+1,pos3 - pos2 -1);
	bool bValid = true;
	for(auto i = strMinute.begin(); i != strMinute.end(); i++)
	{
		if(!iswdigit(*i))
		{
			bValid = false;
			return false;	//无效
		}
	}

	// 处理 秒 可能是 dd.ddd 或 dd 的形式
	bool HasMeetDot = false;
	for(auto i = strSecond.begin(); i != strSecond.end(); i++)
	{
		if(!HasMeetDot)
		{
			if(iswdigit(*i))
				continue;
			else if( *i == L'.')
			{
				HasMeetDot = true;
			}
		}
		else
		{
			if(!iswdigit(*i))
			{
				bValid = false;
				return false;	//无效
			}
		}
	}
		
	//在最后的时候 strLine 可能没有 \n,添加一个回车符号
	line = CStringHelper::Trim(strLine, L"\r\n") + L"\n";

	return true;
}
