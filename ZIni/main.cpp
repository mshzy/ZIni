// ZIni.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ZIni.h"

int _tmain(int argc, _TCHAR* argv[])
{
	ZIni ini("e:\\abc.ini");

	ini.set("xxxx", "yyyy", "zzzz");
	ini.getDouble("xxxx", "yyyy", 1.5);
	//printf("%d\n", ss.length());
	//printf("%s\n", ss.c_str());

	system("pause");
	return 0;
}

