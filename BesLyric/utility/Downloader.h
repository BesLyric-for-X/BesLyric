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
#include <wininet.h>					//链接网络
#pragma comment( lib, "wininet.lib" ) 



//用于分割文件，组成文件
class CDownloader
{
	#define NET_DATA_BLOCK_SIZE 1024 * 10

public:
	/*
	*	@brief	下载到文件 
	*	@param	strUrl			下载链接
	*	@param	strSaveAs		保存文件的路径
	*	@return	true -- 下载成功
	*/
	static bool DownloadFile(const wstring strUrl, const wstring strSaveAs)
	{
		byte Temp[NET_DATA_BLOCK_SIZE];
		ULONG Number = 1;

		FILE *stream;

		HINTERNET hSession = InternetOpen(L"RookIE/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (hSession != NULL)
		{
			HINTERNET handle2 = InternetOpenUrl(hSession, strUrl.c_str(), NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
			if (handle2 != NULL)
			{
				if ((_wfopen_s(&stream, strSaveAs.c_str(), L"wb")) == 0)
				{
					while (Number > 0)
					{
						InternetReadFile(handle2, Temp, NET_DATA_BLOCK_SIZE - 1, &Number);

						fwrite(Temp, sizeof (char), Number, stream);
					}
					fclose(stream);
				}
				else
					return false;

				InternetCloseHandle(handle2);
				handle2 = NULL;
			}
			else
				return false;
			InternetCloseHandle(hSession);
			hSession = NULL;
		}

		return true;
	}

	/*
	*	@brief	下载到字符串
	*	@param	strUrl				下载链接
	*	@param	strSaveBuffer		保存字符串的字符串
	*	@return	true -- 下载成功
	*/
	static bool DownloadString(const wstring strUrl, wstring& strSaveBuffer)
	{
		byte Temp[NET_DATA_BLOCK_SIZE]={0};
		ULONG Number = 0;
		ULONG nTotalRecieved = 0;
		std::size_t nOriginCount = 0;
		PBYTE pBuffer = nullptr;
		strSaveBuffer = L"";


		HINTERNET hSession = InternetOpen(L"RookIE/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (hSession != NULL)
		{
			//int nMs = 5000;
			//BOOL bRet = InternetSetOption(hSession, INTERNET_OPTION_CONNECT_TIMEOUT, &nMs,4);  //该处设置了也不起作用
			HINTERNET handle2 = InternetOpenUrl(hSession, strUrl.c_str(), NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
			if (handle2 != NULL)
			{
				do{
					InternetReadFile(handle2, Temp, NET_DATA_BLOCK_SIZE - 1, &Number);
					if(Number != 0)
					{
						nTotalRecieved += Number;
						if(!Realloc( &pBuffer, nOriginCount,  nTotalRecieved)) //内存分配失败
						{
							if(nOriginCount != 0) //如果之前分配了内存，需要释放
								delete pBuffer;	
							return false;
						}

						//将新得到的内容复制追加到 pBuffer 后面
						copy(Temp + 0, Temp + Number, pBuffer + nOriginCount);

						//更新 nOriginCount 为下次 Realloc 做准备
						nOriginCount = nTotalRecieved;
					}
				}while (Number > 0);

				InternetCloseHandle(handle2);
				handle2 = NULL;
			}
			else
				return false;
			InternetCloseHandle(hSession);
			hSession = NULL;
		}

		WCHAR* pwstrBuffer = new WCHAR[nTotalRecieved];
		
		memset(pwstrBuffer, 0, 2 * nTotalRecieved);

		if(!pwstrBuffer)
		{
			if(nOriginCount != 0) //如果之前分配了内存，需要释放
				delete pBuffer;	

			return false;
		}

		int nRet = ::MultiByteToWideChar(CP_UTF8,0,(LPCSTR)pBuffer,nTotalRecieved,pwstrBuffer,nTotalRecieved);
		strSaveBuffer += pwstrBuffer;

		delete pwstrBuffer;

		if(pBuffer)
			delete pBuffer;

		return true;
	}

	/*
	*	@brief	下载到字符串
	*	@param	strUrl					下载链接
	*	@param	strSaveBuffer			保存字符串的字符串
	*	@param	unsigned int nMaxSize	最多下载大小
	*	@return	true -- 下载成功
	*/
	static bool DownloadString(const wstring strUrl, wstring& strSaveBuffer, unsigned int nMaxSize)
	{
		if(nMaxSize == 0)
		{
			strSaveBuffer = L"";
			return true;
		}

		byte Temp[NET_DATA_BLOCK_SIZE]={0};
		ULONG Number = 0;
		ULONG nTotalRecieved = 0;
		std::size_t nOriginCount = 0;
		PBYTE pBuffer = nullptr;
		strSaveBuffer = L"";

		HINTERNET hSession = InternetOpen(L"RookIE/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (hSession != NULL)
		{
			HINTERNET handle2 = InternetOpenUrl(hSession, strUrl.c_str(), NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
			if (handle2 != NULL)
			{
				do{
					unsigned int nRemain = nMaxSize - nTotalRecieved;					//计算还剩余多少要接收
					unsigned int nThisTime = nRemain < NET_DATA_BLOCK_SIZE - 1 ? nRemain : NET_DATA_BLOCK_SIZE - 1;
																						//计算本次需要接收的大小
					InternetReadFile(handle2, Temp, nThisTime, &Number);
					if(Number != 0)
					{
						nTotalRecieved += Number;
						if(!Realloc( &pBuffer, nOriginCount,  nTotalRecieved)) //内存分配失败
						{
							if(nOriginCount != 0) //如果之前分配了内存，需要释放
								delete pBuffer;	
							return false;
						}

						//将新得到的内容复制追加到 pBuffer 后面
						copy(Temp + 0, Temp + Number, pBuffer + nOriginCount);

						//更新 nOriginCount 为下次 Realloc 做准备
						nOriginCount = nTotalRecieved;
					}
				}while (Number > 0);

				InternetCloseHandle(handle2);
				handle2 = NULL;
			}
			else
				return false;
			InternetCloseHandle(hSession);
			hSession = NULL;
		}

		WCHAR* pwstrBuffer = new WCHAR[nTotalRecieved];
		
		memset(pwstrBuffer, 0, 2 * nTotalRecieved);

		if(!pwstrBuffer)
		{
			if(nOriginCount != 0) //如果之前分配了内存，需要释放
				delete pBuffer;	

			return false;
		}

		int nRet = ::MultiByteToWideChar(CP_UTF8,0,(LPCSTR)pBuffer,nTotalRecieved,pwstrBuffer,nTotalRecieved);
		strSaveBuffer += pwstrBuffer;

		delete pwstrBuffer;

		if(pBuffer)
			delete pBuffer;

		return true;
	}
private:
	/*
	*	@brief	创建或重新分配新的内存
	*	@param	pPByte				内存指针的指针
	*	@param	nOldSize			之前的内存大小， 为0时，表示新创建内存
	*	@param	nNewSize			新分配的内存大小(为0将返回false)
	*	@return	true -- 成功分配指定要的内存
	*/
	static bool Realloc(PBYTE* pPByte, std::size_t nOldSize,  std::size_t nNewSize)
	{
		if(nNewSize == 0)
			return false;
		
		PBYTE pNewBuffer = new BYTE[nNewSize];
		memset(pNewBuffer, 0, nNewSize);

		if(!pNewBuffer)
			return false;

		if(nOldSize != 0)
		{
			for(std::size_t i=0; i < nOldSize && i< nNewSize; i++)
			{
				pNewBuffer[i] = (*pPByte)[i];
			}

			delete *pPByte;
		}
		
		*pPByte = pNewBuffer;

		return true;
	};
};