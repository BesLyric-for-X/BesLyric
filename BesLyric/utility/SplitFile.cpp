#include "stdAfx.h"
#include "SplitFile.h"
#include "WinFile.h"
#include "WinDialog.h"

bool CSplitFile::SplitFile(wstring strSrcFile, wstring strToPath, wstring strToName, unsigned int nBlockSize)
{
	if(!::FileHelper::CheckFileExist(strSrcFile))
		return false;
	
	if(!::FileHelper::CheckFolderExist(strToPath))
		return false;

	FILE *pFile = NULL;
	
	wstring strSrcExt;
	FileHelper::SplitPath(strSrcFile, NULL,NULL, NULL, &strSrcExt);

	wstring strTargetFileNamePre =  strToPath + L"\\" + strToName;
	
	errno_t ret = _wfopen_s(& pFile, strSrcFile.c_str(), L"rb");
	
	if(ret!= RET_SUCCEEDED || pFile == NULL)
		return false;

	//获得原文件大小
	fseek(pFile, 0, SEEK_END);
	int nFileSize = ftell(pFile);
	fclose(pFile);										//获取文件大小

	//将文件后缀写入 strTargetFileNamePre.ext 中
	pFile = NULL;
	ret = _wfopen_s(& pFile, (strTargetFileNamePre+L".ext" + SERVER_FILE_EXTENTION_W).c_str(), L"w");
	
	if(ret!= RET_SUCCEEDED || pFile == NULL)
		return false;

	_fputts( (strSrcExt+L"\n").c_str(), pFile);

	//写入文件大小
	WCHAR szBuffer[MAX_BUFFER_SIZE/2];
	_fputts(_itow(nFileSize,szBuffer,10), pFile);						//写入文件

	fclose(pFile);

	
	//将分割的文件块分别写入 strTargetFileNamePre[1|2|.]中
	pFile = NULL;
	ret = _wfopen_s(& pFile, strSrcFile.c_str(), L"rb");
	
	if(ret!= RET_SUCCEEDED || pFile == NULL)
		return false;

	char* pBlockData;
	pBlockData = new char[nBlockSize];

	int nCount = 1;
	int nSize ;
	while( (nSize = fread(pBlockData, sizeof(char),nBlockSize,  pFile)))
	{
		wchar_t szBuffer[MAX_BUFFER_SIZE];
		wstring strFileName = strTargetFileNamePre+ L"." + _itow(nCount++, szBuffer, 10) + SERVER_FILE_EXTENTION_W;
		
		FILE *pFileTarget;
		ret = _wfopen_s(& pFileTarget, strFileName.c_str(), L"wb");
		
		if(ret!= RET_SUCCEEDED || pFileTarget == NULL)
			return false;

		fwrite( pBlockData,sizeof(char), nSize, pFileTarget);
		fclose(pFileTarget);
	}

	delete pBlockData;

	fclose(pFile);
	return true;
}

bool CSplitFile::MergeFile(wstring strSplitFilePath, wstring strFileName, wstring strMergeToPath)
{
	if(!::FileHelper::CheckFolderExist(strSplitFilePath))
		return false;
	
	if(!::FileHelper::CheckFolderExist(strMergeToPath))
		return false;

	int nMaxCount = 0;

	if(false == GetMaxSplitedCount(strSplitFilePath, strFileName, nMaxCount)) //获得文件被分割的块数
		return false;

	if(nMaxCount == 0) 
		return false; //没有存在文件

	//获得合并后的文件的拓展名,和 合并后文件大小
	WCHAR szExt[MAX_BUFFER_SIZE/2];
	int nSizeOfFileRecord = 0;

	wstring strMergedFile = strMergeToPath +L"\\"+ strFileName +L".ext" + SERVER_FILE_EXTENTION_W;
	if(false == GetMergedFileExtAndSize(strMergedFile, szExt, MAX_BUFFER_SIZE/2, nSizeOfFileRecord))
		return false;

	//分别读取各个被分割的文件，组合成一个文件
	int nCumulateSize = 0;
	wstring strSplitPathNamePre = strSplitFilePath +L"\\"+ strFileName;
	wstring strMergedFilePathName = strMergeToPath +L"\\"+ strFileName + szExt;

	if(!MergeToOneFile(strMergedFilePathName,strSplitPathNamePre , nMaxCount, nCumulateSize))
		return false;

	if(nCumulateSize != nSizeOfFileRecord)
		return false;

	return true;
}

bool CSplitFile::GetMaxSplitedCount(wstring strSplitFilePath,wstring strFileName, int &nMaxCount)
{
	//获得目录下所有被分割的文件名 以 strFileName 为前缀
	WIN32_FIND_DATAW FindFileData;
	WCHAR szFind[MAX_BUFFER_SIZE /2];
	wcscpy_s(szFind, strSplitFilePath.c_str());
	wcscat_s(szFind, L"\\*.*");//过虑的名字

	HANDLE hFind = ::FindFirstFileW(szFind,&FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)
		return false;
	
	vector<wstring> vecFileNames;
	for (;;) //遍历获得文件名
	{
		vecFileNames.push_back(FindFileData.cFileName);

		if(!FindNextFileW(hFind,&FindFileData))
			break;
	}
	FindClose(hFind);

	nMaxCount = 0;
	for(auto iterName = vecFileNames.begin(); iterName != vecFileNames.end();iterName++ )
	{
		wstring strName,strExt;
		FileHelper::SplitPath(*iterName, NULL,NULL, &strName, &strExt); 

		if(strExt == SERVER_FILE_EXTENTION_W) //是服务器文件拓展名，为我们要得到的文件
		{
			//strName 为去掉服务名后缀的字符串
			FileHelper::SplitPath( strName, NULL,NULL, &strName, &strExt);  //再分割一遍，得到的后缀 为 .ext 或 .1 - .n

			if(strName == strFileName)//我们需要处理的文件名
			{
				if(strExt.size() < 2)
					return false;
				else //包括 "."至少会有2个字符 
				{
					strExt = strExt.substr(1); 
					int nCount = _wtoi(strExt.c_str()); 
					if(nCount > nMaxCount )
						nMaxCount = nCount;		//获得最大的文件数量
				}
			}
		}
	}

	return true;
}

bool CSplitFile::GetMergedFileExtAndSize(wstring strMergedFile, WCHAR* szExt, int nSize,int &nSizeOfFileRecord)
{
	FILE *pf = NULL;
	errno_t ret = _wfopen_s(& pf, strMergedFile.c_str(), L"r");
	
	if(ret!= RET_SUCCEEDED || pf == NULL)
		return false;

	WCHAR szFileSize[MAX_BUFFER_SIZE/2];
	_fgetts( szExt, MAX_BUFFER_SIZE/2, pf); //fget会读取换行符，导致后面路径访问失败
	_fgetts( szFileSize, MAX_BUFFER_SIZE/2, pf);

	for(auto i = 0; szExt[i] != L'\0'; i++)
		if(szExt[i] == L'\n')
			szExt[i] = L'\0';

	nSizeOfFileRecord = _wtoi(szFileSize);
	fclose(pf);

	return true;
}

bool CSplitFile::MergeToOneFile(wstring strMergedFilePathName,wstring  strTargetFileNamePreW, int nMaxCount, int &nCumulateSize)
{
	FILE *pFile = NULL;
	errno_t ret = _wfopen_s(&pFile, strMergedFilePathName.c_str(), L"wb");
	
	if(ret!= RET_SUCCEEDED || pFile == NULL)
		return false;
	
	for(int i=1; i <= nMaxCount; i++)
	{
		//
		wchar_t szBuffer[MAX_BUFFER_SIZE];
		wstring strFileNameW = strTargetFileNamePreW+ L"." + _itow(i, szBuffer, 10) + SERVER_FILE_EXTENTION_W;
		
		FILE *pFileSplited;
		ret = _wfopen_s(&pFileSplited, strFileNameW.c_str(), L"rb");
		
		if(ret!= RET_SUCCEEDED || pFileSplited == NULL)
			return false;

		//得到单个数据块数据

		fseek(pFileSplited, 0, SEEK_END);
		int nFileSize = ftell(pFileSplited);										//获取文件大小
		rewind(pFileSplited);
		
		bool bFailToWrite = false;
		char* pBlockData = new char[nFileSize];			//分配整个文件的大小，获取文件数据块的指针
		if (pBlockData == NULL)
			bFailToWrite =true;
		else if (fread(pBlockData, 1, nFileSize, pFileSplited) != nFileSize)		//文件拷贝到m_pFileData中
			bFailToWrite = true;

		fclose(pFileSplited);   

		if(bFailToWrite)		//失败了，也要等到 关闭 pFileSplited 后再返回
		{
			fclose(pFile);		// 关闭前面打开的文件
			return false;
		}

		//将分块的数据写入文件
		fwrite(pBlockData,sizeof(char), nFileSize, pFile);
		
		nCumulateSize += nFileSize; //累积文件大小
	}
	
	fclose(pFile);

	return true;
}


