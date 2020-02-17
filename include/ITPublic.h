#pragma once
#include <windows.h>
//ANSI×ª»¯³ÉUNICODE
static LPWSTR ANSITOUNICODE1(const char* pBuf)
{
	int lenA = lstrlenA(pBuf);
	int lenW=0;
	LPWSTR lpszFile;
	lenW = MultiByteToWideChar(CP_ACP,0,pBuf,lenA,0,0);
	if(lenW > 0)
	{
		lpszFile = SysAllocStringLen(0,lenW);
		MultiByteToWideChar(CP_ACP,0,pBuf,lenA,lpszFile,lenW);
	}
	return lpszFile;
}