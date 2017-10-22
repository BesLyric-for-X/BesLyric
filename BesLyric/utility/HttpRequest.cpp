#include "stdafx.h"
#include "HttpRequest.h"

/*
*	@brief	HttpGet请求
*	@param	strUrl			请求链接
*	@param	strParameter	请求的参数
*	@param	resultContent	成功返回的数据
*	@return	true -- 请求成功返回
*/
bool CHttpRequest::Get(string strUrl, string strParameter, string& resultContent)
{
#ifdef WIN32
    //此处一定要初始化一下，否则gethostbyname返回一直为空
    WSADATA wsa = { 0 };
    WSAStartup(MAKEWORD(2, 2), &wsa);
	
	string strHost = strUrl.substr( 0, strUrl.find_first_of('/')-0);
	bool bRet = SocketRequest(strHost, GetRequestHeader(strUrl,strParameter, false), resultContent);

	WSACleanup();
#else
	bool bRet = SocketRequest(strHost, GetRequestHeader(strUrl,strParameter, false), resultContent);
#endif

	return bRet;
}

bool CHttpRequest::Get(string strUrl, string strParameter, wstring& resultContent)
{
	string strResult;
	bool bRet = Get(strUrl, strParameter, strResult);

	if(!bRet)
		return false;

	resultContent = ToUtf8(strResult);

	return true;
}

/*
*	@brief	HttpPost请求
*	@param	strUrl			请求链接
*	@param	strParameter	请求的参数
*	@param	resultContent	成功返回的数据
*	@return	true -- 请求成功返回
*/
bool CHttpRequest::Post(string strUrl, string strParameter, string& resultContent)
{
#ifdef WIN32
    //此处一定要初始化一下，否则gethostbyname返回一直为空
    WSADATA wsa = { 0 };
    WSAStartup(MAKEWORD(2, 2), &wsa);

	string strHost = strUrl.substr( 0, strUrl.find_first_of('/')-0);
	bool bRet = SocketRequest(strHost, GetRequestHeader(strUrl,strParameter, true), resultContent);

	WSACleanup();
#else
	bool bRet = SocketRequest(strHost, GetRequestHeader(strUrl,strParameter, true), resultContent);
#endif

	return bRet;
}

bool CHttpRequest::Post(string strUrl, string strParameter, wstring& resultContent)
{
	string strResult;
	bool bRet = Post(strUrl, strParameter, strResult);

	if(!bRet)
		return false;

	resultContent = ToUtf8(strResult);

	return true;
}

string CHttpRequest::GetRequestHeader(string strUrl, string strParameter, bool isPost)
{
	string strHost = strUrl.substr( 0, strUrl.find_first_of('/')-0);
	string strPath = strUrl.substr( strUrl.find_first_of('/'), strUrl.size() - strUrl.find_first_of('/'));
	
	std::stringstream stream;
	if(isPost)
	{
		//POST请求方式
		stream << "POST " << strPath;
		stream << " HTTP/1.0\r\n";
		stream << "Host: "<< strHost << "\r\n";
		stream << "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n";
		stream << "Content-Type:application/x-www-form-urlencoded\r\n";
		stream << "Content-Length:" << strParameter.length()<<"\r\n";
		stream << "Connection:close\r\n\r\n";
		stream << strParameter.c_str();
	}
	else
	{
		stream << "GET " << strPath << "?" << strParameter;
		stream << " HTTP/1.0\r\n";
		stream << "Host: " << strHost << "\r\n";
		stream <<"User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n";
		stream <<"Connection:close\r\n\r\n";
	}
	return stream.str();
}

/*
*	@brief	Http socket 请求
*	@param	strHost			请求的主机名
*	@param	strRequest		请求的具体内容
*	@param	resultContent	成功返回的数据
*	@return	true -- 请求成功返回
*/
bool CHttpRequest::SocketRequest(string strHost, string strRequest, string& resultContent)
{
	resultContent = string("");

	//建立连接
	int sockfd;
	struct sockaddr_in address;
	struct hostent *server;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	address.sin_family = AF_INET;
	address.sin_port = htons(80);
	server = gethostbyname(strHost.c_str());
	if(server == nullptr)
		return false;  //获取失败，可能是网络连接问题
	memcpy((char *)&address.sin_addr.s_addr,(char*)server->h_addr, server->h_length);

	if(-1 == connect(sockfd,(struct sockaddr *)&address,sizeof(address))){
		return false; //连接失败
	}

	//接收数据

	byte Temp[HTTP_DATA_BLOCK_SIZE];
	std::size_t nOriginCount = 0;
	PBYTE pBuffer = nullptr;
	int nRecieve= 0;
	int nTotalRecieved = 0;

#ifdef WIN32
	send(sockfd, strRequest.c_str(),strRequest.size(),0);

	while(nRecieve = recv(sockfd, (char*)Temp, HTTP_DATA_BLOCK_SIZE,0))
	{
		assert( nRecieve != -1);
		nTotalRecieved += nRecieve;
		if(!Realloc( &pBuffer, nOriginCount,  nTotalRecieved+1)) //内存分配失败
		{
			if(nOriginCount != 0) //如果之前分配了内存，需要释放
				delete pBuffer;	
			return false;
		}

		//将新得到的内容复制追加到 pBuffer 后面
		copy(Temp + 0, Temp + nRecieve, pBuffer + nOriginCount);

		//更新 nOriginCount 为下次 Realloc 做准备
		nOriginCount = nTotalRecieved;
	}

	closesocket(sockfd);

#else
	write(sockfd,strRequest.c_str(),strRequest.size());

	while(nRecieve = read(sockfd, (char*)Temp, NET_DATA_BLOCK_SIZE))  
	{
		assert( nRecieve != -1);
		nTotalRecieved += nRecieve;
		if(!Realloc( &pBuffer, nOriginCount,  nTotalRecieved+1)) //内存分配失败
		{
			if(nOriginCount != 0) //如果之前分配了内存，需要释放
				delete pBuffer;	
			return false;
		}

		//将新得到的内容复制追加到 pBuffer 后面
		copy(Temp + 0, Temp + nRecieve, pBuffer + nOriginCount);

		//更新 nOriginCount 为下次 Realloc 做准备
		nOriginCount = nTotalRecieved;
	}
	close(sockfd);
#endif

	pBuffer[nTotalRecieved] = '\0';
	resultContent = (char*)pBuffer;

	if(pBuffer)
		delete pBuffer;
	
	return true;
}


//用于将获得的数据转换为utf-8宽字节字符串
wstring CHttpRequest::ToUtf8(string strAscii)
{
	wstring wstrResult;

	int nRet=MultiByteToWideChar(CP_UTF8,0,strAscii.c_str(),strAscii.size(),NULL,0); 
    if(nRet>0)
    {
        wchar_t *pBuf=new wchar_t[nRet+1];
        MultiByteToWideChar(CP_UTF8,0,strAscii.c_str(),strAscii.size(),pBuf,nRet+1);
		pBuf[nRet] = '\0'; //最后总是多了一串字符，截断
        wstrResult = wstring(pBuf);
        delete []pBuf;
    }

	return wstrResult;
}

/*
*	@brief	创建或重新分配新的内存
*	@param	pPByte				内存指针的指针
*	@param	nOldSize			之前的内存大小， 为0时，表示新创建内存
*	@param	nNewSize			新分配的内存大小(为0将返回false)
*	@return	true -- 成功分配指定要的内存
*/
bool CHttpRequest::Realloc(PBYTE* pPByte, std::size_t nOldSize,  std::size_t nNewSize)
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