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

#pragma once
#include "stdafx.h"

#include <WinSock2.h>
#include <iostream>
#include <sstream>
using namespace std;

#ifdef WIN32
#pragma comment(lib,"ws2_32.lib")
#endif

//用于分Http请求
class CHttpRequest
{
#define HTTP_DATA_BLOCK_SIZE 1024*10

public:
	/*
	*	@brief	HttpGet请求
	*	@param	strUrl			请求链接
	*	@param	strParameter	请求的参数
	*	@param	resultContent	成功返回的数据
	*	@return	true -- 请求成功返回
	*/
	bool Get(string strUrl, string strParameter, string& resultContent);
	bool Get(string strUrl, string strParameter, wstring& resultContent);

	/*
	*	@brief	HttpPost请求
	*	@param	strUrl			请求链接
	*	@param	strParameter	请求的参数
	*	@param	resultContent	成功返回的数据
	*	@return	true -- 请求成功返回
	*/
	bool Post(string strUrl, string strParameter, string& resultContent);
	bool Post(string strUrl, string strParameter, wstring& resultContent);

private:
	string GetRequestHeader(string strUrl, string strParameter, bool isPost);

	/*
	*	@brief	Http socket 请求
	*	@param	strHost			请求的主机名
	*	@param	strRequest		请求的具体内容
	*	@param	resultContent	成功返回的数据
	*	@return	true -- 请求成功返回
	*/
	bool SocketRequest(string strHost, string strRequest, string& resultContent);

	//用于将获得的数据转换为utf-8宽字节字符串
	wstring ToUtf8(string strAscii);

	/*
	*	@brief	创建或重新分配新的内存
	*	@param	pPByte				内存指针的指针
	*	@param	nOldSize			之前的内存大小， 为0时，表示新创建内存
	*	@param	nNewSize			新分配的内存大小(为0将返回false)
	*	@return	true -- 成功分配指定要的内存
	*/
	static bool Realloc(PBYTE* pPByte, std::size_t nOldSize,  std::size_t nNewSize);
};