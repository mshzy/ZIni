/*  ZIni: A Real Crude Cross-Platform Ini-File-Reader Written by C++
*
*   version  :  v0.0.1
*   Author   :  SHZY
*   Email    :  188813950@qq.com
*   now-time :  2021-1-13
*
***************************************
*
*   Happy Using!
*
*/

#ifndef __INI_H__
#define __INI_H__

#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include "stdafx.h"

class ZIni
{
public:
	explicit ZIni(const char *filePath);
	virtual ~ZIni() {};
public:
	template<typename T>
	T get(const char *mainKey, const char *subKey, T defaultValue)
	{
		std::map<std::string, std::map<std::string, std::string>>::iterator mainIter;;
		std::map<std::string, std::string>::iterator subIter;
		if ((mainIter = mainMap.find(mainKey)) == mainMap.end())
		{
			return defaultValue;
		}
		if ((subIter = (*mainIter).second.find(subKey)) == (*mainIter).second.end())
		{
			return defaultValue;
		}
		std::string strTemp = (*subIter).second;
		std::stringstream ss(strTemp);
		T returnValue;
		ss >> returnValue;
		return returnValue;
	}

	std::string get(const char *mainKey, const char *subKey, const char *defaultCStr = "")
	{
		std::map<std::string, std::map<std::string, std::string>>::iterator mainIter;;
		std::map<std::string, std::string>::iterator subIter;
		if ((mainIter = mainMap.find(mainKey)) == mainMap.end())
		{
			return defaultCStr;
		}
		if ((subIter = (*mainIter).second.find(subKey)) == (*mainIter).second.end())
		{
			return defaultCStr;
		}
		return (*subIter).second;
	}

	std::string get(const char *mainKey, const char *subKey, char *defaultCStr)
	{
		return get(mainKey, subKey, std::string(defaultCStr));
	}

	std::map<std::string, std::string> & operator[] (std::string mainKey)
	{
		if (mainMap.count(mainKey) == 0)
		{
			return emptyMap;
		}
		return mainMap[mainKey];
	}

	bool is_open()
	{
		return b_open;
	}

	bool set(const char *mainKey, const char *subKey, const char *subValue)
	{
		std::map<std::string, std::map<std::string, std::string>>::iterator mainIter;;
		std::map<std::string, std::string>::iterator subIter;
		bool mainKeyExist = ((mainIter = mainMap.find(mainKey)) != mainMap.end());
		bool subKeyExist = mainKeyExist ? ((subIter = ((*mainIter).second.find(subKey))) != ((*mainIter).second.end())) : false;

		if (!mainKeyExist)
		{
			std::map<std::string, std::string> subMap;
			subMap.insert({ subKey, subValue });
			mainMap.insert({ mainKey, subMap });
			std::string buffer("\n\n[");
			buffer.reserve(128);
			buffer += mainKey; buffer += "]\n"; buffer += subKey; buffer += " = "; buffer += subValue;
			filestring += buffer;
		}
		else
		{
			if (subKeyExist)
			{
				(*subIter).second = subValue;
				std::string buffer("[");
				buffer.reserve(128);
				buffer += mainKey; buffer += ']';
				auto lineBeginIndex = filestring.find(subKey, filestring.find(buffer));
				//check if the index is valid, because there may be a value which is the same as the key.
				while (lineBeginIndex != std::string::npos)
				{
					if (isSubKeyIndexValid(lineBeginIndex + strlen(subKey)))
						break;
					lineBeginIndex = filestring.find(subKey, lineBeginIndex);
				}
				auto lineEndIndex = filestring.find('\n', lineBeginIndex);
				if (lineEndIndex != std::string::npos)    //did not reach the end-of-file
				{
					buffer = subKey; buffer += " = "; buffer += subValue;
					filestring.replace(lineBeginIndex, lineEndIndex - lineBeginIndex, buffer);
				}
				else
				{
					buffer = subKey; buffer += " = "; buffer += subValue;
					filestring.replace(lineBeginIndex, filestring.length() - lineBeginIndex, buffer);
				}
			}
			else
			{
				(*mainIter).second.insert({ subKey, subValue });
				std::string buffer("[");
				buffer.reserve(128);
				buffer += mainKey; buffer += ']';
				auto mainKeyIndex = filestring.find(buffer);
				auto writeIndex = filestring.find('\n', mainKeyIndex);
				buffer = subKey; buffer += " = "; buffer += subValue; buffer += '\n';
				filestring.insert(writeIndex + 1, buffer);
			}
		}
		return writeIntoFile();
	}

private:
	std::string filepath;
	std::string filestring;
	std::map<std::string, std::map<std::string, std::string>> mainMap;
	std::map<std::string, std::string> emptyMap;
	bool b_open = false;

private:
	std::string::size_type removeSpace(std::string &str)
	{
		str.erase(0, str.find_first_not_of(" "));
		str.erase(str.find_last_not_of(" \r") + 1); //��ֹ�л��з�\r\n��©��\r
		return str.length();
	}

	bool isSubKeyIndexValid(unsigned int index)
	{
		return '=' == filestring[filestring.find_first_not_of(' ', index)];
	}

	bool writeIntoFile()
	{
		FILE *fp = fopen(filepath.c_str(), "wb");
		if (!fp) return false;
		fwrite(filestring.c_str(), 1, filestring.length(), fp);
		fclose(fp);
		return true;
	}
};

inline ZIni::ZIni(const char *filePath)
{
	FILE *fp = fopen(filePath, "rb");
	if (fp)
	{
		b_open = true;
		filepath = filePath;
	}
	fseek(fp, 0, SEEK_END);
	int sizeOfBytes = ftell(fp);
	rewind(fp);
	filestring.reserve(sizeOfBytes * 2);   //prepare for the write member function.(avoid memory alloc)
	filestring.resize(sizeOfBytes);
	fread((void *)filestring.c_str(), 1, sizeOfBytes, fp);  //C++11 only
	fclose(fp);

	//��ʼ�������ļ����е��ַ���
	std::string mainKey(""), subKey(""), subValue("");
	std::map<std::string, std::string> subMap;
	std::string::size_type lastLineBreakIndex;  //��¼��һ�����з���λ�ã�Ϊ��Ч�ʣ���ֻ֤����һ�Σ����Ա�����=���ŵ�ʱ����Ҫ��ס��λ�ã�ֱ��ȡ��ǰ���keyֵ
	std::string::size_type firstValidIndex = filestring.find_first_of('['); //��һ��'['��λ��
	auto i = firstValidIndex;
	while (i < filestring.length())
	{
		switch (filestring[i])
		{
		case '[':
			//�Ȱ���һ��mainKey�Ķ���insert��ȥ����
			if (removeSpace(mainKey) != 0 && subMap.size() != 0)
			{
				mainMap.insert({ mainKey, subMap });
			}
			mainKey.clear();
			subMap.clear();
			for (auto j = i + 1; j < filestring.length(); ++j)
			{
				if (filestring[j] == ']')
				{
					std::string buf(filestring, i + 1, j - i - 1);
					mainKey = std::move(buf);
					i = j; //ֱ��������i�����ñշ����ŵ�λ�ã�֮��i++���ű���']'������ַ�
					break;
				}
			}
			break;
		case ';':
			for (auto j = i + 1; j < filestring.length(); ++j)
			{
				if (filestring[j] == '\n')
				{
					i = j;
					lastLineBreakIndex = i;
					break;
				}
			}
			break;
		case '\n':
			lastLineBreakIndex = i;
			break;
		case '=':
			std::string buf(filestring, lastLineBreakIndex + 1, i - lastLineBreakIndex - 1);
			subKey = std::move(buf);
			bool isLastLine = true; //�Ƿ�Ϊ���һ�С�ʵ���Ͼ���˱���Ϊfalse��Ҳ���ܱ�֤��һ���������һ�У������ļ�ĩβ�и�\n������ֻ��Ϊһ��flag���������forѭ���Ƿ���������е�if���
			for (auto j = i + 1; j < filestring.length(); ++j)
			{
				if (filestring[j] == '\n')
				{
					isLastLine = false;
					std::string buffer(filestring, i + 1, j - i - 1);
					subValue = std::move(buffer);
					i = j;
					lastLineBreakIndex = i;
					break;
				}
			}
			if (isLastLine)
			{
				//�����forѭ������֮�󣬶�û���ҵ�'\n'����ô���о����ļ�ĩβ�ˣ��ǾͰ�'='��֮���ȫ������subValue
				std::string buffer(filestring, i + 1, std::string::npos);
				subValue = std::move(buffer);
			}
			if ((removeSpace(subKey) != 0) && (removeSpace(subValue) != 0))
			{
				subMap.insert({ subKey, subValue });
			}
			subKey.clear();
			subValue.clear();
			break;
		}
		i++;
	}
	if (removeSpace(mainKey) != 0 && subMap.size() != 0)
	{
		mainMap.insert({ mainKey, subMap });
	}
}



#endif