/*
	BesLyric  一款 操作简单、功能实用的 专门用于制作网易云音乐滚动歌词的 歌词制作软件。
    Copyright (C) 2017  
	Author: BensonLaur <BensonLaur@163.com>
	Author:

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
* @file       NcmIDManager.h
* 
* @Describe   管理 ncm 文件名与 其网易云id 的匹配逻辑 
*/

#pragma once
#include "stdafx.h"
#include <string>
#include "..\lib\ZSingleton.hpp"
#include <map>
using namespace std;

class CNcmIDManager
{
	SINGLETON_0(CNcmIDManager)


public:
	bool SaveDataPairs();					//保存数据对
	bool LoadDataPairs();					//加载数据对

	bool FindID( wstring fileName,OUT wstring& id);			//查找ID
	void InsertNcmIDPair( wstring fileName, wstring id);	//插入ID对

	//检测ID是否有效
	//返回 false 为查询失败（网络问题等）
	bool CheckIDValidity(wstring id, OUT bool& bValid);				

private:
	//更新ID
	bool UpdateID( wstring fileName, wstring id);

private:
	map< wstring, wstring>	m_mapNcmID;	/* 储存ncm文件名和其对应ID的匹配关系 */
};
