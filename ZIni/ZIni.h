#ifndef __INI_H__
#define __INI_H__

#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include "stdafx.h"

class ZIni
{
public:
	// ���캯��
	explicit ZIni(const char *filePath);

	// ��������
	virtual ~ZIni() {};
public:
	template<typename T>
	T get(const char *mainKey, const char *subKey);

private:
	std::map<std::string, std::map<std::string, std::string>> mainMap;
};



inline ZIni::ZIni(const char *filePath)
{
	std::ifstream fs(filePath);
	std::stringstream ss;
	ss << fs.rdbuf();
	std::string filestring(ss.str()); //ֱ�Ӵ��ļ���������string

	//��ʼ�������ļ����е��ַ�������ȡֻ����һ��
	std::string mainKey(""), subKey(""), subValue("");
	std::map<std::string, std::string> subMap;
	std::string::size_type lastLineBreakIndex;  //��¼��һ�����з���λ�ã�Ϊ�˱�֤���Ӷ�O(n)�����Ա�����=���ŵ�ʱ����Ҫ��ס��λ�ã�ֱ��ȡ��ǰ���keyֵ
	auto i = filestring.find_first_of('[');
	while (i < filestring.length())
	{
		switch (filestring[i])
		{
		case '[':
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
			subMap.insert({ subKey, subValue });
			mainMap.insert({ mainKey, subMap });
			break;
		}




		i++;
	}





	/*std::string sValue(fstring, lastIndex, curIndex - lastIndex);
	subValue = std::move(sValue);
	subMap.insert({ subKey, subValue });
	mainMap.insert({ mainKey, subMap });*/

}























#endif