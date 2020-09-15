#include "stdafx.h"

#include<algorithm>
#include<ctime>

#include "LinkHelper.h"
#include "UpdateHelper.h"

#include "../utility/Downloader.h"
#include "../utility/WinFile.h"

#include "..\lib\md5\md5.h"
#include "..\lib\xml\tinyxml2.h"
using namespace tinyxml2;

/*!
	从服务器更新链接

	\return 返回是否获取成功
*/
bool LinkHelper::UpdateLinkFromServer()
{
	//从服务器获取更新链接
	vector<wstring> candidates;
	candidates.push_back(L"https://files.cnblogs.com/files/BensonLaur/Beslyric-Link.xml");
	candidates.push_back(L"http://bensonlaur.com/files/beslyric/Beslyric-Link.xml");
	
	UpdateHelper updateHelper;
	wstring etcDir = updateHelper.GetEtcDir();
	if(!FileHelper::IsFloderExistW(etcDir))
		_wmkdir(etcDir.c_str());

	wstring xml = updateHelper.GetEtcDir()+ L"dynamicLink.xml";
	bool getValidUpdate = false;
	for(size_t i = 0; i < candidates.size(); ++i)
	{
		if(!CDownloader::DownloadFile(candidates[i], xml))
			continue; //下载失败

		if(!IsValidXml(xml))
			continue; //下载下来的内容不对

		if(!GetLinkUpdate(xml))
			continue; //xml解析出错，继续下一个

		//成功获取，可以跳出
		getValidUpdate = true;
		break;
	}

	return getValidUpdate;
}

/*
	获得所有的 ffmpeg 下载链接（为了减少单一服务器负荷，会随机打乱顺序）
*/
vector<NamedLink> LinkHelper::GetAllLinksFFmpeg()
{
	vector<NamedLink> links = GetLinksFFmpegRaw();
	links.insert(links.end(),linksFFmpegUpdate.begin(), linksFFmpegUpdate.end());

	//乱序算法 
	//srand种子要加上#include<ctime>头文件 
	srand((unsigned int)time(0));
	random_shuffle(links.begin(),links.end());

	return links;
}

/*
	获得程序自带的 ffmpeg 下载链接
*/
vector<NamedLink> LinkHelper::GetLinksFFmpegRaw()
{
	if(linksFFmpegRaw.empty())
	{
		linksFFmpegRaw.push_back(NamedLink(L"gitlab",L"https://gitlab.com/BensonLaur/resource-provider-gitlab/-/raw/master/beslyric/ffmpeg-3.4.1.exe"));
		linksFFmpegRaw.push_back(NamedLink(L"bitbucket",L"https://bitbucket.org/bensonlaur/resource-provider-bitbucket/raw/bcd1a44f30893427a5f78243e4548f9afbb82c9c/beslyric/ffmpeg-3.4.1.exe"));
	}

	return linksFFmpegRaw;
}

/*!
	检测文件是否为有效的 xml 文件

	\param filePath 被检测的文件
	\return 返回文件是否为有效的 xml 文件
*/
bool LinkHelper::IsValidXml(const std::wstring& filePath)
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
bool LinkHelper::GetLinkUpdate(const wstring& updateItemFile)
{
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
		
		linksFFmpegUpdate.clear();

		//根
		XMLElement *pRoot = doc.RootElement();
		SASSERT(pRoot);

		XMLElement* ele = pRoot->FirstChildElement();
		while(ele)
		{
			const char* eName = ele->Name();

			const char* szName = ele->Attribute("name");
			const char* szLink = ele->Attribute("link");
			
			if(eName && szName && szLink)
			{
				wstring wStrName = S_CA2W(SStringA(szName),CP_UTF8);
				wstring wStrLink = S_CA2W(SStringA(szLink),CP_UTF8);

				if(eName == string("ffmpeg"))
					linksFFmpegUpdate.push_back(NamedLink(wStrName, wStrLink));
			}

			//下一兄弟结点
			ele = ele->NextSiblingElement();
		}
	}

	return true;
}
