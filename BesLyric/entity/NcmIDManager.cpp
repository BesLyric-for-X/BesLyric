
#include "stdafx.h"
#include "NcmIDManager.h"
#include "..\utility\WinFile.h"
#include "..\lib\xml\tinyxml2.h"
#include "..\utility\Downloader.h"
using namespace tinyxml2;

SINGLETON_C_D_0(CNcmIDManager)

	
static const wstring XML_PAIR = L"pair";			//ncm id 对
static const wstring XML_FILE_NAME = L"fileName";	//文件名
static const wstring XML_ID = L"id";				//ID

//保存数据对
bool CNcmIDManager::SaveDataPairs()
{
	wstring xmlStr = L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	xmlStr += L"<ncmid>\n";

	for(auto iter= m_mapNcmID.begin(); iter != m_mapNcmID.end(); iter++)
		xmlStr += L"\t<"+ XML_PAIR +L" " + XML_FILE_NAME +L"=\""+	iter->first +L"\" " + XML_ID +L"=\""+	iter->second +L"\""+  L" />\n";
	
	xmlStr += L"</ncmid>\n";
	
	wstring wstrPath = FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" +NCM_ID_FILE_NAME;
	
	return FileOperator::WriteToUtf8File( wstrPath,xmlStr);
}

//加载数据对
bool CNcmIDManager::LoadDataPairs()
{
	//从文件加载数据
	wstring wstrPath = FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\" + NCM_ID_FILE_NAME;
	string strPath = S_CW2A(SStringW(wstrPath.c_str()));
	if(FileHelper::CheckFileExist(wstrPath))
	{
		//读取XML文件
		tinyxml2::XMLDocument doc;
		doc.LoadFile(strPath.c_str());
		if(doc.Error())
		{
			return false;
		}

		m_mapNcmID.clear();


		//根
		XMLElement *pRoot = doc.RootElement();
		SASSERT(pRoot);

		XMLElement* ele = pRoot->FirstChildElement();
		while(ele)
		{
			const char* szName = ele->Name();
			const char* szfileName = ele->Attribute(S_CW2A(SStringW(XML_FILE_NAME.c_str()).GetBuffer(1)));
			const char* szID = ele->Attribute(S_CW2A(SStringW(XML_ID.c_str()).GetBuffer(1)));
			
			wstring wStrName = S_CA2W(SStringA(szfileName));
			wstring wStrID = S_CA2W(SStringA(szID));
			
			WCHAR wszName[MAX_BUFFER_SIZE] = {0};
			WCHAR wszID[MAX_BUFFER_SIZE] = {0};

			//由于文件保存是以utf-8保存，需要转换
			::MultiByteToWideChar(CP_UTF8,0,szfileName, MAX_BUFFER_SIZE/2, wszName, MAX_BUFFER_SIZE);
			//由于文件保存是以utf-8保存，需要转换
			::MultiByteToWideChar(CP_UTF8,0,szID, MAX_BUFFER_SIZE/2, wszID, MAX_BUFFER_SIZE);
			
			pair<wstring, wstring> p;
			p.first = wszName;
			p.second = wszID;
			m_mapNcmID.insert(p);

			//下一兄弟结点
			ele = ele->NextSiblingElement();
		}
	}
	else
		SaveDataPairs();

	//在有网络的可以校验的时候，对数据进行简单的校验，更新储存
	map< wstring, wstring>	mapNcmIDTemp;
	for(auto iter = m_mapNcmID.begin(); iter != m_mapNcmID.end(); iter++)
	{
		bool bValid = false;
		if(!CheckIDValidity( iter->second, bValid))
			return true;//网络连接失败了，不检测了

		if(bValid)
		{
			mapNcmIDTemp.insert(*iter);
		}
	}

	if(mapNcmIDTemp.size() != m_mapNcmID.size())
	{
		//已经筛除了一些，重新存储
		m_mapNcmID = mapNcmIDTemp;
		SaveDataPairs();
	}

	return true;
}

//查找ID
bool CNcmIDManager::FindID( wstring fileName,OUT wstring& id)
{
	auto iter = m_mapNcmID.find(fileName);
	if(iter != m_mapNcmID.end())
	{
		id = iter->second;

		return true;
	}
	else
		return false;
}

//插入ID对
void CNcmIDManager::InsertNcmIDPair( wstring fileName, wstring id)
{
	wstring tempID;
	if(FindID(fileName,tempID))
	{
		if(tempID != id)
			UpdateID(fileName, id);
	}
	else
	{
		pair<wstring,wstring> p(fileName, id);
		m_mapNcmID.insert(p);
	}

}


//检测ID是否有效
//返回 false 为查询失败（网络问题等）
bool CNcmIDManager::CheckIDValidity(wstring id, OUT bool& bValid)
{
	wstring strContent = L"";
	wstring strLink = L"http://music.163.com/song/media/outer/url?id="+ id +L".mp3";
	if(!CDownloader::DownloadString( strLink, strContent, 500))
	{
		return false;
	}
	else
	{
		auto index = strContent.find(L"<!DOCTYPE html>");  //任意检测2个网页返回的html标签
		auto index2 = strContent.find(L"<html>");
		if(index != wstring::npos || index2 != wstring::npos ) //返回串中包含网页相关标签，认为下载歌曲失败，认为ID无效
			bValid = false;
		else
			bValid = true;

		return true;
	}
}


//更新ID
bool CNcmIDManager::UpdateID( wstring fileName, wstring id)
{
	auto iter = m_mapNcmID.find(fileName);
	if(iter != m_mapNcmID.end())
	{
		iter->second = id;

		return true;
	}
	else
		return false;
}
