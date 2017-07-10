#include "stdafx.h"
#include "winfile.h"
#include "FileHelper.h"

#include <fstream> 
using namespace std;

/* FileOperator */

bool FileOperator::ReadAllText(const string file, string& fileContent)
{
	ifstream in(file, ios_base::in);
	string temp = "";
	if (in)
	{
		while (!in.eof())
		{
			getline(in, temp);
			temp += "\n";
			fileContent += temp;
		}
		return false;
	}
	else
		return false;
}

bool FileOperator::ReadAllLines(const string file, OUT vector<string> *lines)
{
	ifstream in(file, ios_base::in);
	string temp = "";
	if (in)
	{
		while (!in.eof())
		{
			getline(in, temp);
			lines->push_back(temp);
		}
		return false;
	}
	else
		return false;
}

bool FileOperator::ReadAllLinesW(const string file, OUT vector<SStringW> *lines)
{
	File encodingFile( S_CA2W(SStringA(file.c_str())), L"r");
	if(!encodingFile.isValidFile())
		return false;

	if(encodingFile.m_encodingType == ENCODING_TYPE::UTF_8 || encodingFile.m_encodingType == ENCODING_TYPE::UNICODE_BIG_ENDIAN 
		|| encodingFile.m_encodingType == ENCODING_TYPE::UNICODE_LITTLE_ENDIAN )
	{
		wchar_t line[MAX_WCHAR_COUNT_OF_LINE+1];
	
		// 从文件中读取歌词，并将非空行加入到 maker.m_vLyricOrigin 向量中
		while(fgetws(line,MAX_WCHAR_COUNT_OF_LINE,encodingFile.m_pf))
		{
			line[MAX_CHAR_COUNT_OF_LINE]='\0';//保证在最后一个字符处截断字符串

			//由变长字符转换为 unicode 宽字节字符
			//MultiByteToWideChar(CP_ACP,MB_COMPOSITE,_line,strlen(_line)+1,line,MAX_WCHAR_COUNT_OF_LINE+1);

			SStringW aLine(line);
			aLine.Trim('\n');
			aLine.Trim(' ');
			aLine.Trim('\t');
			if(aLine.GetLength() ==0) 
				continue;

			lines->push_back(aLine);
		}
	}
	else //if(encodingFile.m_encodingType == ENCODING_TYPE::ASCII  或 其他
	{
		char _line[MAX_CHAR_COUNT_OF_LINE+1]; 
		wchar_t line[MAX_WCHAR_COUNT_OF_LINE+1];
	
		// 从文件中读取歌词，并将非空行加入到 maker.m_vLyricOrigin 向量中
		while(fgets(_line,MAX_WCHAR_COUNT_OF_LINE,encodingFile.m_pf))
		{
			_line[MAX_CHAR_COUNT_OF_LINE]='\0';//保证在最后一个字符处截断字符串

			//由变长字符转换为 unicode 宽字节字符
			MultiByteToWideChar(CP_ACP,MB_COMPOSITE,_line,strlen(_line)+1,line,MAX_WCHAR_COUNT_OF_LINE+1);
			SStringT aLine(line);
			aLine.Trim('\n');
			aLine.Trim(' ');
			aLine.Trim('\t');
			if(aLine.GetLength() ==0) 
				continue;

			lines->push_back(aLine);
		}
	}

	return true;
}

bool FileOperator::SaveToFile(const string file, string& fileContent)
{
	ofstream out(file, ios_base::out);
	if (out)
	{
		out << fileContent;
		return true;
	}
	else
		return false;
}


/* FileHelper */
string FileHelper::GetCurrentDirectoryStr()
{
	wchar_t exeFullPath[MAX_PATH]; // Full path   

	GetModuleFileName(NULL, exeFullPath, MAX_PATH);

	string strPath(S_CW2A(SStringW(exeFullPath)).GetBuffer(1));
	strPath = strPath.substr(0, strPath.find_last_of("\\")+1);
	return strPath;
}

bool FileHelper::CheckPathName(LPCTSTR format, LPCTSTR toChecked)
{
	int i;
	bool isFloder = false;
	//TODO：异常抛出处理
	int len = _tcslen(format);
	if (_tcscmp(format, _T("..")) == 0)
	{
		isFloder = true;
	}
	else if (len < 3 || format[0] != _T('*') || format[1] != _T('.'))
		return false;  //TODO：异常


	//获取并检查 被检查的路径字符串 toChecked 的信息
	TCHAR pathName[_MAX_PATH];
	TCHAR ext[_MAX_EXT];

	int lenPathName = 0, pos = -1;

	_tcscpy(pathName, toChecked);
	lenPathName = _tcslen(pathName);	//得到路径总长
	if (!lenPathName)
		return false;

	//得到路径中最后一个“.”的位置置于pos中
	for (i = 0; i< lenPathName; i++)
	{
		if (_T('.') == pathName[i])
			pos = i;
	}

	if (isFloder) //检查文件夹类型
	{
		return IsDirectory(pathName);
	}
	else //检查普通后缀名类型
	{
		_tcscpy(ext, &pathName[pos + 1]);  //得到路径的后缀（不包含“.”）
		if (_tcscmp(&format[2], ext) == 0)	//和 参数提供的后缀对比
			return true;
		else
			return false;
	}
}

bool FileHelper::IsDirectory(SStringW path)
{
	//查找panelFloder是否存在
	WIN32_FIND_DATA  FindFileData;
	BOOL bValue = false;
	HANDLE hFind = FindFirstFile(path, &FindFileData);
	if ((hFind != INVALID_HANDLE_VALUE) && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		bValue = true;
	}
	FindClose(hFind);

	return bValue;
}

bool  FileHelper::CheckFileExist(const string &strPath)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(S_CA2W(SStringA(strPath.c_str())), &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return false;
	}
	else {
		FindClose(hFind);
		return true;
	}
}

bool  FileHelper::CheckFolderExist(const string &strPath)
{
	WIN32_FIND_DATA  wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(S_CA2W(SStringA(strPath.c_str())), &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = true;
	}
	FindClose(hFind);

	return rValue;
}

void FileHelper::SplitPath(const string& strPathName, OUT string *pstrDrive, OUT string *pstrDirectory, OUT string* pstrName, OUT string* pstrExt)
{
	string _strDrive = "";
	string _strDirectory = "";
	string _strName = "";
	string _strExt = "";

	//分号位置
	auto posColon = strPathName.find_first_of(':');
	
	if (posColon != string::npos)
		_strDrive = strPathName.substr(0, posColon);

	//最后一个分隔符号位置
	auto posLastSep = strPathName.find_last_of("/\\");
	if (posLastSep != string::npos)
	{
		_strDirectory = strPathName.substr(0, posLastSep +1);

		posLastSep++;
	}
	else
		posLastSep = 0;

	//字符串结尾
	auto posEnd = strPathName.size();

	//寻找拓展名的"."位置
	auto posDot = strPathName.find_last_of('.');
	if (posDot != string::npos)
	{
		//特殊处理，路径中含有 . 的情况可能有 ..\\test, 所以只有当 获得的“.”位置大于  posLastSep 时， 获得的“.”位置才是拓展名的"."位置
		if (posDot > posLastSep)
			_strExt = strPathName.substr(posDot, posEnd - posDot);
		else
			posDot = posEnd;
	}
	else
		posDot = posEnd;


	_strName = strPathName.substr(posLastSep, posDot - posLastSep);

	if (pstrDrive != nullptr)
		*pstrDrive = _strDrive;
	if (pstrDirectory != nullptr)
		*pstrDirectory = _strDirectory;
	if (pstrName != nullptr)
		*pstrName = _strName;
	if (pstrExt != nullptr)
		*pstrExt = _strExt;
}

