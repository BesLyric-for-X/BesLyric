#include "stdafx.h"
#include "UpdateHelper.h"

#include "../utility/WinFile.h"

#include "..\lib\md5\md5.h"
#include "..\lib\xml\tinyxml2.h"
using namespace tinyxml2;

/*!
	检测文件是否为有效的 xml 文件

	\param filePath 被检测的文件
	\return 返回文件是否为有效的 xml 文件
*/
bool UpdateHelper::IsValidXml(const std::wstring& filePath)
{
	//读取XML文件
	tinyxml2::XMLDocument doc;
	FILE* fp = 0;
	errno_t err = _tfopen_s(&fp, filePath.c_str(), L"rb");
	if (!fp || err)
	{
		return false;
	}
	doc.LoadFile(fp);
	fclose(fp);

	if(doc.Error())
	{
		return false;
	}

	return true;
}
		
/*!
	获得更新文件内容

	\param updateItemFile 存储更新项信息的xml
	\return 返回是否获取成功
*/
bool UpdateHelper::GetUpdateItem(const wstring& updateItemFile, vector<UpdateItem>& updateItems)
{
	updateItems.clear();

	if(FileHelper::CheckFileExist(updateItemFile))
	{
		//读取XML文件
		tinyxml2::XMLDocument doc;
		FILE* fp = 0;
		errno_t err = _tfopen_s(&fp, updateItemFile.c_str(), L"rb");
		if (!fp || err)
		{
			return false;
		}
		doc.LoadFile(fp);
		fclose(fp);

		if(doc.Error())
		{
			return false;
		}

		//根
		XMLElement *pRoot = doc.RootElement();
		SASSERT(pRoot);

		XMLElement* ele = pRoot->FirstChildElement();
		while(ele)
		{
			UpdateItem item;
			const char* szName = ele->Attribute("name");
			const char* szLink = ele->Attribute("link");
			const char* szLocal = ele->Attribute("local");
			const char* szMd5 = ele->Attribute("md5");
			
			wstring wStrName = S_CA2W(SStringA(szName),CP_UTF8);
			wstring wStrLink = S_CA2W(SStringA(szLink),CP_UTF8);
			wstring wStrLocal = S_CA2W(SStringA(szLocal),CP_UTF8);
			string strMd5 = szMd5;

			item.fileName = wStrName;
			item.link = wStrLink;
			item.local = wStrLocal; 
			item.md5 = strMd5;

			updateItems.push_back(item);

			//下一兄弟结点
			ele = ele->NextSiblingElement();
		}
	}



	return true;
}

//更新内容自检，如果当前已有的内容和更新xml不同，删除更新xml文件
void UpdateHelper::UpdateItemSelfCheck()
{
	vector<UpdateItem> updateItems;
	if(!GetUpdateItem(GetEtcDir() + L"update", updateItems))			
	{
		return;
	}

	//逐个文件检测
	bool deleteUpdateXml = false;
	for(auto iter = updateItems.begin(); iter != updateItems.end(); iter++)
	{
		wstring taget = FileHelper::GetCurrentDirectoryStr() + iter->local + iter->fileName;
		
		//计算本地md5 
		string md5;
		GetFileMd5(taget, md5);

		if(md5 != iter->md5)
		{
			deleteUpdateXml = true;
			break;
		}
	}

	if(deleteUpdateXml)
		DeleteFile((GetEtcDir() + L"update").c_str());
}

//获得运行目录下的 etc/ 目录
wstring UpdateHelper::GetEtcDir()
{
	return FileHelper::GetCurrentDirectoryStr() + FLODER_NAME_ETC + L"\\";
}

//确保相对路径目录存在
bool UpdateHelper::MakeSureRelativeLocalExist(const wstring& basePath, const wstring& relativePath)
{
	wstring dir = basePath + relativePath;

	return FileHelper::MakeSureTargetDirExist(dir);
}

//获取文件的 md5 码
bool UpdateHelper::GetFileMd5(const wstring& filePath, string& strMd5)
{
	//不存在文件则返回
	if(!FileHelper::CheckFileExist(filePath))
		return false;

	MD5_CTX md5 = MD5_CTX();
    char md5Str[33];
    md5.GetFileMd5(md5Str, filePath.c_str());

	strMd5 = md5Str;

	return true;
}


//判断文件md5是否是有效的 ffmpeg 的 md5
bool UpdateHelper::IsValidFFmpegMd5(const string& strMd5)
{
	// 2.3.0 后的新版本 ffmpeg 为兼容 xp 的 ffmpeg,为了旧用户不受影响（重新下载）
	//先判断是否是旧的 ffmpeg 的 MD5, 再判断新的 ffmpeg 的 MD5

	const string MD5_FFMPEG_old = "949ed6af96c53ba9e1477ded35281db5";
	const string MD5_FFMPEG_new = "979fc6e7ff2c74108353c114bf86e2bb";

	if( MD5_FFMPEG_old == strMd5)
		return true;

	return MD5_FFMPEG_new == strMd5;
}
