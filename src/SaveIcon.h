#ifndef HS_HEADER_DEF_ICONMEMFUNC_H
#define HS_HEADER_DEF_ICONMEMFUNC_H

#if _MSC_VER >= 1000
#pragma once
#endif
//////////////////////////////////////////////////////////////////////////
#include <windows.h>

struct TIconHeader
{ 
WORD Reserved; 
WORD wType; 
WORD Count; 
};

struct TIconRes
{ 
BYTE Width; 
BYTE Height; 
WORD Colors; 
WORD Reserved1; 
WORD Reserved2; 
DWORD DIBSize; 
DWORD DIBOffset; 
}; 

VOID InitBmpHeader(HBITMAP hBitmap, BITMAPINFOHEADER& BI, int nBit);

VOID GetDIBSizes(HBITMAP Bitmap, DWORD &InfoSize, DWORD &ImageSize, int nBit);

VOID GetDIB(HBITMAP Bitmap, BITMAPINFO* BmpInfo, PVOID Bits, INT nBit);

BOOL SaveIconToFile(HICON hIcon, LPWSTR szFileName, int nBit);

#endif //HS_HEADER_DEF_ICONMEMFUNC_H