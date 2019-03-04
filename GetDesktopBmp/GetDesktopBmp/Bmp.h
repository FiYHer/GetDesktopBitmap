/*
��ȡ��Ļ����д��λͼ�ļ�
*/

#pragma once

#include <windows.h>
#include <assert.h>

//��ȡ��ǰ��Ļλͼд��λͼ�ļ�
BOOL GetCurrentDesktopBitmap(const LPCSTR szBitmapPath);

// ��ȡHDCλͼ������
BOOL GetHdcData(HDC& hdc,RECT& stDesktopRect,PBYTE pBitmapData);

// ����HDCΪbmp�ļ�
BOOL WriteBitmapFile(const LPCSTR szBitmapPath, HDC& hdc, RECT& stDesktopRect);

BOOL GetCurrentDesktopBitmap(const LPCSTR szBitmapPath)
{
	//�ж�д���Ƿ�ɹ�
	BOOL bWrite = FALSE;
	//����DC
	HDC hdc = NULL;
	//����Ŀ�Ⱥ͸߶�
	RECT stDesktopRect = { 0 };

	//��ȡ�����HDC
	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	if (hdc == NULL)
		return bWrite;

	//��ȡ����Ŀ�Ⱥ͸߶�
	stDesktopRect.right =::GetSystemMetrics(SM_CXFULLSCREEN);
	stDesktopRect.bottom =::GetSystemMetrics(SM_CYFULLSCREEN);

	//λͼ����д��λͼ�ļ�
	if (WriteBitmapFile(szBitmapPath, hdc, stDesktopRect))
		//д��ɹ�
		bWrite = TRUE;

	//ɾ��hdc����
	::DeleteDC(hdc);

	//���ؽ��
	return bWrite;
}

BOOL GetHdcData(HDC& hdc,RECT& stDesktopRect,PBYTE pBitmapData)
{
	//����Ƿ�ɹ���ȥHDC�����λͼ����
	BOOL bCheck = FALSE;
	//����λͼ
	HBITMAP hBitmap = NULL;
	//����DC
	HDC hdcMem = NULL;
	//λͼ��Դָ��
	PBYTE pData = nullptr;
	__try
	{
		//��ʼ��λͼ���ݽṹ
		BITMAPINFO stBitmapInfo = { 0 };
		//�ṹ��С
		stBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		//λͼ���
		stBitmapInfo.bmiHeader.biWidth = stDesktopRect.right - stDesktopRect.left;
		//λͼ�߶�
		stBitmapInfo.bmiHeader.biHeight = stDesktopRect.bottom - stDesktopRect.top;
		//��׼
		stBitmapInfo.bmiHeader.biPlanes = 1;
		//λ��
		stBitmapInfo.bmiHeader.biBitCount = 24;

		//����һ������DC
		hdcMem = CreateCompatibleDC(hdc);
		if (hdcMem == NULL)
			__leave;

		//���ڴ�DC�����ȡλͼ����
		//hBitmap λͼ���
		//pData λͼ����ָ��
		hBitmap = CreateDIBSection(hdcMem, &stBitmapInfo, DIB_RGB_COLORS,
			(VOID**)&pData, NULL, NULL);
		if (hBitmap == NULL)
			__leave;

		//��λͼѡ�뻺��dc
		SelectObject(hdcMem, hBitmap);

		//������DC��λͼ���ݻ��Ƶ�����dc����
		SIZE sizeImg = { stBitmapInfo.bmiHeader.biWidth,stBitmapInfo.bmiHeader.biHeight };
		//����ͼ��
		StretchBlt(hdcMem,0,0,sizeImg.cx,sizeImg.cy,hdc,
			stDesktopRect.left,stDesktopRect.top,sizeImg.cx,sizeImg.cy,SRCCOPY);

		//�ڴ渴��
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

// ����HDCΪBMP�ļ�
BOOL WriteBitmapFile(LPCSTR szBitmapPath, HDC& hdc, RECT& stDesktopRect)
{
	//���λͼ����
	INT nBitmapDataLen = stDesktopRect.bottom*stDesktopRect.right * 3;
	PBYTE pBitmapData = new BYTE[nBitmapDataLen];

	//�����ȡ����λͼ����
	if (GetHdcData(hdc, stDesktopRect, pBitmapData) == FALSE)
		return FALSE;

	//����λͼ��Ϣͷ
	BITMAPINFOHEADER bmInfoHeader = { 0 };
	//�ṹ��С
	bmInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	//������
	bmInfoHeader.biWidth = stDesktopRect.right - stDesktopRect.left;
	//����߶�
	bmInfoHeader.biHeight = stDesktopRect.bottom - stDesktopRect.top;
	//Ĭ��
	bmInfoHeader.biPlanes = 1;
	//λ��
	bmInfoHeader.biBitCount = 24;

	//����λͼ�ļ�ͷ
	BITMAPFILEHEADER bmFileHeader = { 0 };
	//Ĭ��
	bmFileHeader.bfType = 0x4d42;  //bmp  
	//λͼ���ݿ�ʼƫ�Ƶ�ַ
	bmFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	//λͼ���ݴ�С
	bmFileHeader.bfSize = bmFileHeader.bfOffBits +
		((bmInfoHeader.biWidth * bmInfoHeader.biHeight) * 3); ///3=(24 / 8)

	//����λͼ�ļ�
	HANDLE hFile = CreateFile(szBitmapPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	//д���ļ�
	DWORD dwWrite = 0;
	//�ļ�ͷ
	WriteFile(hFile, &bmFileHeader, sizeof(BITMAPFILEHEADER), &dwWrite, NULL);
	//��Ϣͷ
	WriteFile(hFile, &bmInfoHeader, sizeof(BITMAPINFOHEADER), &dwWrite, NULL);
	//λͼ����
	WriteFile(hFile, pBitmapData, nBitmapDataLen, &dwWrite, NULL);
	//�رվ��
	CloseHandle(hFile);
	delete[] pBitmapData;

	return TRUE;
}
