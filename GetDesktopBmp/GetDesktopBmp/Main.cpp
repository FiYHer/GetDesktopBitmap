
#include "Bmp.h"
#include <stdio.h>

int main(int nNum, char** szCmdList, char** szPath)
{
	if (GetCurrentDesktopBitmap("D://TestBitmap.bmp"))
		printf("�ɹ�\n");
	else
		printf("ʧ��\n");

	system("pause");
	return 0;
}


