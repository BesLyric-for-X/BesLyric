#include "stdafx.h"
#include "winfile.h"
#include "WinDialog.h"
#include "StringHelper.h"

#include <fstream> 
using namespace std;


/* 文件类 */

File::File(LPCTSTR pathFile,LPCTSTR mode)
{
	m_lpszPathFile = NULL;
	m_lpszMode = NULL;
	m_pf = NULL;
	m_encodingType = ENCODING_TYPE::OTHER;
	char *szBufferTest = NULL;
	int nTestLen = 0;

	if(_tcscmp(mode,_T("r")) == 0)
	{
		//读取文件的前两个字节，判断编码
		unsigned int len = 0;
		char *buf = NULL;
		byte firstByte = '\0';
		byte secondByte = '\0';
		ifstream in(pathFile);
		if(in)
		{
			in.seekg(0,ios::end);
			len = (unsigned int) in.tellg();
			buf = new char[len];
			in.seekg(0,ios::beg);
			in.read(buf,len);

			if(len < 2)//文件内容太少，读取失败
				return ;
			
			firstByte = buf[0];
			secondByte = buf[1];

			//读取1000个字节（或整个文件长度），为后面区别 utf-8 无Bom 和 ascii 提供测试数据
			nTestLen = len < 1000? len : 1000;
			szBufferTest = new char[nTestLen];
			
			in.clear();				//clear 后 seekg 才有效
			in.seekg(0,ios::beg);
			in.read(szBufferTest,nTestLen);

			in.close();
			delete buf;
		}
		else return;//打开文件失败

		if(firstByte == 0xef && secondByte == 0xbb)
			m_encodingType = ENCODING_TYPE::UTF_8;
		else if(firstByte == 0xff && secondByte == 0xfe)
			m_encodingType = ENCODING_TYPE::UNICODE_LITTLE_ENDIAN;
		else if(firstByte == 0xfe && secondByte == 0xff)
			m_encodingType = ENCODING_TYPE::UNICODE_BIG_ENDIAN;
		else 
		{
			//测试是否是 utf-8 无bom格式
			if(IsUTF8WithNoBom(szBufferTest, nTestLen))
				m_encodingType = ENCODING_TYPE::UTF_8_NO_BOM;
			else
				m_encodingType = ENCODING_TYPE::ASCII;	//默认ascii 码和其他编码，以ascii 方式处理
		}

		if(szBufferTest)
			delete szBufferTest;

		//根据不同的编码，打开文件
		if(m_encodingType == ENCODING_TYPE::UTF_8 || m_encodingType == ENCODING_TYPE::UTF_8_NO_BOM)
			m_pf = _tfopen(pathFile, _T("r,ccs=utf-8"));
		else if(m_encodingType == ENCODING_TYPE::UNICODE_LITTLE_ENDIAN)
			m_pf = _tfopen(pathFile, _T("r,ccs=UNICODE"));
		else if(m_encodingType == ENCODING_TYPE::UNICODE_BIG_ENDIAN)
		{	
			//UNICODE_BIG_ENDIAN 不支持_tfopen(pathFile, _T("r,ccs=UNICODE")); 方式读取

			//转换为UNICODE_LITTLE_ENDIAN文本，然后再读取

			//使用c方式读取文件
			int len = 0;
			FILE *in = _tfopen(pathFile,_T("rb"));
			if(in)
			{
				fseek(in, 0, SEEK_END);
				len = ftell(in);
				buf = new char[len];
				fseek(in, 0, SEEK_SET);
				fread(buf, sizeof(char), len, in);

				WCHAR *pWC;
				pWC = reinterpret_cast<WCHAR*>(buf);

				int index = 0;
				while(index < len/2)  //对每一个宽字节交换字节序
				{
					WCHAR temp = *pWC >> 8;
					temp |= *pWC << 8;
					*pWC = temp;
					pWC++;
					index++;
				}

				//以c的形式写入临时文件
				FILE* tFile = _tfopen(_T("temp_unicode_little_endian.txt"),_T("wb"));
				if(tFile)
				{
					fwrite(buf, sizeof(char), len, tFile);
					fclose(tFile);
				}
				else return;//写入文件失败
			}
			else return;//打开文件失败
			
			m_pf = _tfopen(_T("temp_unicode_little_endian.txt"), _T("r,ccs=UNICODE"));
		}
		else //m_encodingType == ENCODING_TYPE::ASCII
			m_pf = _tfopen(pathFile,mode);

	}
	else //if(_tcscmp(mode,_T("r"))
		m_pf = _tfopen(pathFile,mode);


	m_lpszPathFile = pathFile;
	m_lpszMode = mode;
}

//测试数据是否是UTF-8 无Bom格式 
bool File::IsUTF8WithNoBom(const void* pBuffer, long size)
{
	//参考 http://blog.csdn.net/bladeandmaster88/article/details/54767487

	bool IsUTF8 = true;     
    unsigned char* start = (unsigned char*)pBuffer;     
    unsigned char* end = (unsigned char*)pBuffer + size;     
    while (start < end)     
    {     
        if (*start < 0x80) // (10000000): 值小于0x80的为ASCII字符     
        {     
            start++;     
        }     
        else if (*start < (0xC0)) // (11000000): 值介于0x80与0xC0之间的为无效UTF-8字符     
        {     
            IsUTF8 = false;     
            break;     
        }     
        else if (*start < (0xE0)) // (11100000): 此范围内为2字节UTF-8字符     
        {     
            if (start >= end - 1)      
                break;     
            if ((start[1] & (0xC0)) != 0x80)     
            {     
                IsUTF8 = false;     
                break;     
            }     
            start += 2;     
        }      
        else if (*start < (0xF0)) // (11110000): 此范围内为3字节UTF-8字符     
        {     
            if (start >= end - 2)      
                break;     
            if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80)     
            {     
                IsUTF8 = false;     
                break;     
            }     
            start += 3;     
        }      
        else    
        {     
            IsUTF8 = false;     
            break;     
        }     
    }     
    return IsUTF8;     
}



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
		return true;
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
		return true;
	}
	else
		return false;
}

bool FileOperator::ReadAllLinesW(const wstring file, OUT vector<SStringW> *lines)
{
	File encodingFile( file.c_str(), L"r");
	if(!encodingFile.isValidFile())
		return false;

	return ReadAllLinesW(encodingFile, lines);
}

bool FileOperator::ReadAllLinesW(File& encodingFile,  OUT vector<SStringW> *lines)
{
	if(!encodingFile.isValidFile())
		return false;
	
	if(encodingFile.m_encodingType == ENCODING_TYPE::UTF_8 || encodingFile.m_encodingType == ENCODING_TYPE::UNICODE_BIG_ENDIAN 
		|| encodingFile.m_encodingType == ENCODING_TYPE::UNICODE_LITTLE_ENDIAN || encodingFile.m_encodingType == ENCODING_TYPE::UTF_8_NO_BOM)
	{
		wchar_t line[MAX_WCHAR_COUNT_OF_LINE+1];
	
		// 从文件中读取歌词，并将非空行加入到 maker.m_vLyricOrigin 向量中
		while(fgetws(line,MAX_WCHAR_COUNT_OF_LINE,encodingFile.m_pf))
		{
			line[MAX_WCHAR_COUNT_OF_LINE]='\0';//保证在最后一个字符处截断字符串

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
		while(fgets(_line,MAX_CHAR_COUNT_OF_LINE,encodingFile.m_pf))
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


bool FileOperator::WriteToUtf8File(const wstring file, wstring fileContent)
{
	vector<SStringW> lines;

	do{
		auto pos = fileContent.find_first_of(L"\n");
		if(pos != wstring::npos)
		{
			wstring line = fileContent.substr(0,pos+1);
			SStringW wline = line.c_str();
			lines.push_back(wline);

			fileContent = fileContent.substr(pos+1);
		}
		else
		{
			SStringW wline = fileContent.c_str();
			lines.push_back(wline);
			break;
		}
	}while(true);

	return WriteToUtf8File(file, lines);
}

bool FileOperator::WriteToUtf8File(const wstring file, vector<SStringW> lines)
{
	File outFile(file.c_str(),_T("w")); 

	if(!outFile.isValidFile())
		return false;

	//先写入 UTF-8 BOM 头
	fputc(0xef, outFile.m_pf);  
	fputc(0xbb, outFile.m_pf); 
	fputc(0xbf, outFile.m_pf); 

	string line;
	for(auto i=lines.begin(); i != lines.end(); i++)
	{
		//去掉一行当中可能存在的\r, 因为 fputs 会自动在 \n 前添加\r，如果不去掉，输出结尾为 \r\r\n
		SStringW _line = CStringHelper::Trim(*i,L" \t\r\n");
		_line += L"\n";

		int nRet = WideCharToMultiByte(CP_UTF8,0,_line,-1,NULL,0,NULL,NULL); 
		if(nRet>0)
		{
			char *pBuf=new char[nRet+1];
			int ret = WideCharToMultiByte(CP_UTF8,  0,_line,-1,pBuf,nRet,NULL,NULL);
			line = string(pBuf);
			if(ret == 0)
			{
				delete []pBuf;
				return false;
			}
			else
				delete []pBuf;
		}
		else
			return false;

		fputs(line.c_str(),outFile.m_pf);
	}

	return true;
}



/* FileHelper */
wstring FileHelper::GetCurrentDirectoryStr()
{
	wchar_t exeFullPath[MAX_PATH]; // Full path   

	GetModuleFileName(NULL, exeFullPath, MAX_PATH);

	wstring strPath(exeFullPath);
	strPath = strPath.substr(0, strPath.find_last_of(L"\\")+1);
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
	bool bValue = false;
	HANDLE hFind = FindFirstFile(path, &FindFileData);
	if ((hFind != INVALID_HANDLE_VALUE) && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		bValue = true;
	}
	FindClose(hFind);

	return bValue;
}

bool  FileHelper::CheckFileExist(const wstring &strPath)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(strPath.c_str(), &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return false;
	}
	else {
		FindClose(hFind);
		return true;
	}
}

bool  FileHelper::CheckFolderExist(const wstring &strPath)
{
	WIN32_FIND_DATA  wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(strPath.c_str(), &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = true;
	}
	FindClose(hFind);

	return rValue;
}

void FileHelper::SplitPath(const wstring& strPathName, OUT wstring *pstrDrive, OUT wstring *pstrDirectory, OUT wstring* pstrName, OUT wstring* pstrExt)
{
	wstring _strDrive = L"";
	wstring _strDirectory = L"";
	wstring _strName = L"";
	wstring _strExt = L"";

	//分号位置
	auto posColon = strPathName.find_first_of(L':');
	
	if (posColon != string::npos)
		_strDrive = strPathName.substr(0, posColon);

	//最后一个分隔符号位置
	auto posLastSep = strPathName.find_last_of(L"/\\");
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
	auto posDot = strPathName.find_last_of(L'.');
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

