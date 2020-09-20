#pragma once
#include <Windows.h>
#include "../Define.h"
#include "../utility/SSingleton.h"
#include "helper\SCriticalSection.h"

#include <string>
#include <vector>
using namespace std;


class LinkValue{
public:
	LinkValue( const wstring& _value,const wstring& _link):link(_link),value(_value){}
public:
	wstring link;
	wstring value;
};

//!链接辅助器
class LinkHelper: public Singleton<LinkHelper>, public SCriticalSection{

public:	
	/*!
		从服务器更新链接

		\return 返回是否获取成功
	*/
	bool UpdateLinkFromServer();

	/*
		获得所有的 ffmpeg 下载链接（为了减少单一服务器负荷，会随机打乱顺序）
	*/
	vector<LinkValue> GetAllLinksFFmpeg();
	
	/*
		获得所有的 ip 获取链接（为了减少单一服务器负荷，会随机打乱顺序）
	*/
	vector<LinkValue> GetAllLinksIp();

private:
	
	/*!
		检测文件是否为有效的 xml 文件

		\param filePath 被检测的文件
		\return 返回文件是否为有效的 xml 文件
	*/
	bool IsValidXml(const std::wstring& filePath);
	
	/*
		获得程序自带的 ffmpeg 下载链接
	*/
	vector<LinkValue> GetLinksFFmpegRaw();
	
	/*
		获得程序自带的 ip 下载获取
	*/
	vector<LinkValue> GetLinksIpRaw();
	
	/*!
		获得更新的链接内容

		\param updateItemFile 存储更新项信息的xml
		\return 返回是否获取成功
	*/
	bool GetLinkUpdate(const wstring& updateItemFile);

	//判断是否完成了更新
	bool HasFinishedUpdate();
	
private:
	vector<LinkValue> linksFFmpegRaw;
	vector<LinkValue> linksFFmpegUpdate;
	
	vector<LinkValue> linksIpRaw;
	vector<LinkValue> linksIpUpdate;
};