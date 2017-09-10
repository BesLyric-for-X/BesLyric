#include "stdafx.h"
#include "StringHelper.h"
#include <vector>
using namespace std;

//去除前后字符
SStringW CStringHelper::Trim(SStringW strToTrim, SStringW strCharactorRemoved)
{
	int first_fit;
	int last_fit;
	bool bHasFindFirst;

	first_fit = last_fit = 0;
	bHasFindFirst = false;

	for(int i = 0; i != strToTrim.GetLength(); i++)
	{
		if(!bHasFindFirst)
		{
			if(!IsCharInString(strToTrim.GetAt(i),strCharactorRemoved) )
			{
				first_fit = i;
				bHasFindFirst = true;
			}
		}
		else 
		{
			if(!IsCharInString(strToTrim.GetAt(i),strCharactorRemoved))
			{
				last_fit = i;
			}
		}
	}

	if(!bHasFindFirst && last_fit == 0)  //没有任何匹配字符的情况
		return L"";

	if(bHasFindFirst && last_fit == 0)	//只有最后1个字符是符合的情况，last_fit 为0 ，需要纠正
		last_fit = strToTrim.GetLength() - 1;

	strToTrim = strToTrim.Left( last_fit + 1).Right( last_fit + 1 - first_fit);

	return strToTrim;
}


wstring CStringHelper::Trim(wstring strToTrim, wstring strCharactorRemoved)
{
	SStringW strToTrim2 = SStringW(strToTrim.c_str());
	SStringW strCharactorRemoved2 = SStringW(strCharactorRemoved.c_str());
	SStringW strReturn = Trim(strToTrim2, strCharactorRemoved2);
	return wstring(strReturn.GetBuffer(1));
}

//获得Trim之后的长度
int CStringHelper::GetTrimLength(SStringW strToTrim, SStringW strCharactorRemoved)
{
	return Trim(strToTrim,strCharactorRemoved).GetLength();
}

int CStringHelper::GetTrimLength(wstring strToTrim, wstring strCharactorRemoved)
{
	return Trim(strToTrim,strCharactorRemoved).size();
}


//将字符串分割成行
void CStringHelper::SplitStringByToLines(SStringW strToSplted,WCHAR splitChar, vector<SStringW>& vecLines)
{
	int pos = 0;
	
	do{
		pos = strToSplted.Find( splitChar);
		if(pos != -1)
		{
			vecLines.push_back(strToSplted.Left(pos+1));
			strToSplted = strToSplted.Right( strToSplted.GetLength() - pos -1);
		}
		else
		{
			vecLines.push_back(strToSplted);
			break;
		}

	}while(true);
}


//字符是否在字符串内出现
bool CStringHelper::IsCharInString(WCHAR ch, SStringW str)
{
	for(int i=0; i<str.GetLength(); i++)
	{
		if(ch == str[i])
			return true;
	}

	return false;
}