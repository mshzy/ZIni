// ZIni.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ZIni.h"

int _tmain(int argc, _TCHAR* argv[])
{
	ZIni ini("F:\\abc.ini");

	std::string  ss = ini["xxxxx"]["test_record_rootdir"];

	printf("%d\n", ss.length());
	printf("%s\n", ss.c_str());

	system("pause");
	return 0;
}

