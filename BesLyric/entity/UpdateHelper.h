#pragma once
#include "../Define.h"

#include <string>
#include <vector>
using namespace std;

//!更新辅助器
class UpdateHelper{
public:
	/*!
		检测文件是否为有效的 xml 文件

		\param filePath 被检测的文件
		\return 返回文件是否为有效的 xml 文件
	*/
	bool IsValidXml(const std::wstring& filePath);
	
	/*!
		获得更新文件内容

		\param updateItemFile 存储更新项信息的xml
		\return 返回是否获取成功
	*/
	bool GetUpdateItem(const wstring& updateItemFile, vector<UpdateItem>& updateItems);

	//更新内容自检，如果当前已有的内容和更新xml不同，删除更新xml文件
	void UpdateItemSelfCheck();

	//获得运行目录下的 etc/ 目录
	wstring GetEtcDir();

	//确保相对路径目录存在
	bool MakeSureRelativeLocalExist(const wstring& basePath, const wstring& relativePath);
	
	//获取文件的 md5 码
	static bool GetFileMd5(wstring filePath, string& strMd5);
};