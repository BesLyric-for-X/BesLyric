#pragma once
#include "../Define.h"
#include "../utility/SSingleton.h"

#include <string>
#include <vector>
using namespace std;


class NamedLink{
public:
	NamedLink( const wstring& _name,const wstring& _link):link(_link),name(_name){}
public:
	wstring link;
	wstring name;
};

//!链接辅助器
class LinkHelper: public Singleton<LinkHelper>{

public:	
	/*!
		从服务器更新链接

		\return 返回是否获取成功
	*/
	bool UpdateLinkFromServer();

	/*
		获得所有的 ffmpeg 下载链接（为了减少单一服务器负荷，会随机打乱顺序）
	*/
	vector<NamedLink> GetAllLinksFFmpeg();

private:
	/*
		获得程序自带的 ffmpeg 下载链接
	*/
	vector<NamedLink> GetLinksFFmpegRaw();

	/*!
		检测文件是否为有效的 xml 文件

		\param filePath 被检测的文件
		\return 返回文件是否为有效的 xml 文件
	*/
	bool IsValidXml(const std::wstring& filePath);
	
	/*!
		获得更新的链接内容

		\param updateItemFile 存储更新项信息的xml
		\return 返回是否获取成功
	*/
	bool GetLinkUpdate(const wstring& updateItemFile);

private:
	vector<NamedLink> linksFFmpegRaw;

	vector<NamedLink> linksFFmpegUpdate;
};