
#include "Bmp.h"
#include <stdio.h>

int main(int nNum, char** szCmdList, char** szPath)
{
	if (GetCurrentDesktopBitmap("D://TestBitmap.bmp"))
		printf("³É¹¦\n");
	else
		printf("Ê§°Ü\n");

	system("pause");
	return 0;
}


