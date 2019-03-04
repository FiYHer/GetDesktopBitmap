/*
获取屏幕数据写入位图文件
*/

#pragma once

#include <windows.h>
#include <assert.h>

//获取当前屏幕位图写入位图文件
BOOL GetCurrentDesktopBitmap(const LPCSTR szBitmapPath);

// 获取HDC位图的数据
BOOL GetHdcData(HDC& hdc,RECT& stDesktopRect,PBYTE pBitmapData);

// 保存HDC为bmp文件
BOOL WriteBitmapFile(const LPCSTR szBitmapPath, HDC& hdc, RECT& stDesktopRect);

BOOL GetCurrentDesktopBitmap(const LPCSTR szBitmapPath)
{
	//判断写入是否成功
	BOOL bWrite = FALSE;
	//桌面DC
	HDC hdc = NULL;
	//桌面的宽度和高度
	RECT stDesktopRect = { 0 };

	//获取桌面的HDC
	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	if (hdc == NULL)
		return bWrite;

	//获取桌面的宽度和高度
	stDesktopRect.right =::GetSystemMetrics(SM_CXFULLSCREEN);
	stDesktopRect.bottom =::GetSystemMetrics(SM_CYFULLSCREEN);

	//位图数据写入位图文件
	if (WriteBitmapFile(szBitmapPath, hdc, stDesktopRect))
		//写入成功
		bWrite = TRUE;

	//删除hdc数据
	::DeleteDC(hdc);

	//返回结果
	return bWrite;
}

BOOL GetHdcData(HDC& hdc,RECT& stDesktopRect,PBYTE pBitmapData)
{
	//检查是否成功后去HDC里面的位图数据
	BOOL bCheck = FALSE;
	//桌面位图
	HBITMAP hBitmap = NULL;
	//缓存DC
	HDC hdcMem = NULL;
	//位图资源指针
	PBYTE pData = nullptr;
	__try
	{
		//初始化位图数据结构
		BITMAPINFO stBitmapInfo = { 0 };
		//结构大小
		stBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		//位图宽度
		stBitmapInfo.bmiHeader.biWidth = stDesktopRect.right - stDesktopRect.left;
		//位图高度
		stBitmapInfo.bmiHeader.biHeight = stDesktopRect.bottom - stDesktopRect.top;
		//标准
		stBitmapInfo.bmiHeader.biPlanes = 1;
		//位数
		stBitmapInfo.bmiHeader.biBitCount = 24;

		//创建一个缓存DC
		hdcMem = CreateCompatibleDC(hdc);
		if (hdcMem == NULL)
			__leave;

		//从内存DC里面获取位图数据
		//hBitmap 位图句柄
		//pData 位图数据指针
		hBitmap = CreateDIBSection(hdcMem, &stBitmapInfo, DIB_RGB_COLORS,
			(VOID**)&pData, NULL, NULL);
		if (hBitmap == NULL)
			__leave;

		//将位图选入缓存dc
		SelectObject(hdcMem, hBitmap);

		//将缓存DC的位图数据绘制到缓存dc里面
		SIZE sizeImg = { stBitmapInfo.bmiHeader.biWidth,stBitmapInfo.bmiHeader.biHeight };
		//复制图像
		StretchBlt(hdcMem,0,0,sizeImg.cx,sizeImg.cy,hdc,
			stDesktopRect.left,stDesktopRect.top,sizeImg.cx,sizeImg.cy,SRCCOPY);

		//内存复制
		memcpy(pBitmapData, pData, sizeImg.cx*sizeImg.cy * 3);

		bCheck = TRUE;
	}
	__finally
	{
		if (hBitmap != NULL)
			DeleteObject(hBitmap);

		if (hdcMem != NULL)
			DeleteDC(hdcMem);
	}

	return bCheck;
}

// 保存HDC为BMP文件
BOOL WriteBitmapFile(LPCSTR szBitmapPath, HDC& hdc, RECT& stDesktopRect)
{
	//存放位图数据
	INT nBitmapDataLen = stDesktopRect.bottom*stDesktopRect.right * 3;
	PBYTE pBitmapData = new BYTE[nBitmapDataLen];

	//如果获取桌面位图数据
	if (GetHdcData(hdc, stDesktopRect, pBitmapData) == FALSE)
		return FALSE;

	//构建位图信息头
	BITMAPINFOHEADER bmInfoHeader = { 0 };
	//结构大小
	bmInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	//桌面宽度
	bmInfoHeader.biWidth = stDesktopRect.right - stDesktopRect.left;
	//桌面高度
	bmInfoHeader.biHeight = stDesktopRect.bottom - stDesktopRect.top;
	//默认
	bmInfoHeader.biPlanes = 1;
	//位数
	bmInfoHeader.biBitCount = 24;

	//构建位图文件头
	BITMAPFILEHEADER bmFileHeader = { 0 };
	//默认
	bmFileHeader.bfType = 0x4d42;  //bmp  
	//位图数据开始偏移地址
	bmFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	//位图数据大小
	bmFileHeader.bfSize = bmFileHeader.bfOffBits +
		((bmInfoHeader.biWidth * bmInfoHeader.biHeight) * 3); ///3=(24 / 8)

	//创建位图文件
	HANDLE hFile = CreateFile(szBitmapPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	//写入文件
	DWORD dwWrite = 0;
	//文件头
	WriteFile(hFile, &bmFileHeader, sizeof(BITMAPFILEHEADER), &dwWrite, NULL);
	//信息头
	WriteFile(hFile, &bmInfoHeader, sizeof(BITMAPINFOHEADER), &dwWrite, NULL);
	//位图数据
	WriteFile(hFile, pBitmapData, nBitmapDataLen, &dwWrite, NULL);
	//关闭句柄
	CloseHandle(hFile);
	delete[] pBitmapData;

	return TRUE;
}
