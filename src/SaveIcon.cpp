#include "SaveIcon.h"

BOOL SaveIconToFile(HICON hIcon, LPWSTR szFileName, int nBit)
{
	BOOL ret = FALSE;
	PVOID MonoBits;
	PVOID ColorBits;
	DWORD dwWrite;
	HANDLE hFile;
	ICONINFO IconInfo;
	TIconRes tIR;
	TIconHeader tIH;
	DWORD MonoInfoSize, ColorInfoSize, MonoBitsSize, ColorBitsSize;
	BITMAPINFO* MonoInfo;
	BITMAPINFO* ColorInfo;

	switch (nBit)
	{
	case 0: case 1: case 4: case 8: case 16: case 24: case 32:
		break;
	default:
		return FALSE;
	}
	if (!hIcon || !szFileName)
	{
		return FALSE;
	}

	GetIconInfo(hIcon, &IconInfo);

	if (IconInfo.fIcon)
	{
		if (INVALID_HANDLE_VALUE !=
			(hFile = CreateFileW(szFileName, GENERIC_WRITE,
				FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0))
			)
		{
			ZeroMemory(&tIH, sizeof(TIconHeader));
			ZeroMemory(&tIR, sizeof(TIconRes));
			GetDIBSizes(IconInfo.hbmMask, MonoInfoSize, MonoBitsSize, 1);
			GetDIBSizes(IconInfo.hbmColor, ColorInfoSize, ColorBitsSize, nBit);
			MonoInfo = reinterpret_cast<BITMAPINFO*>(malloc(MonoInfoSize));
			ColorInfo = reinterpret_cast<BITMAPINFO*>(malloc(ColorInfoSize));
			MonoBits = malloc(MonoBitsSize);
			ColorBits = malloc(ColorBitsSize);
			GetDIB(IconInfo.hbmMask, MonoInfo, MonoBits, 1);
			GetDIB(IconInfo.hbmColor, ColorInfo, ColorBits, nBit);
			tIH.wType = static_cast<WORD>(IconInfo.fIcon);
			tIH.Count = 1;
			ret = WriteFile(hFile, &tIH, sizeof(TIconHeader), &dwWrite, NULL);
			
			if (!ColorInfo)
			{
				return FALSE;
			}

			tIR.Width = static_cast<BYTE>(ColorInfo->bmiHeader.biWidth);
			tIR.Height = static_cast<BYTE>(ColorInfo->bmiHeader.biHeight);
			tIR.Colors = ColorInfo->bmiHeader.biPlanes * ColorInfo->bmiHeader.biBitCount;
			tIR.DIBSize = ColorInfoSize + ColorBitsSize + MonoBitsSize;
			tIR.DIBOffset = sizeof(TIconHeader) + sizeof(TIconRes);
			ret &= WriteFile(hFile, &tIR, sizeof(TIconRes), &dwWrite, NULL);
			ColorInfo->bmiHeader.biHeight *= 2;
			ret &= WriteFile(hFile, ColorInfo, ColorInfoSize, &dwWrite, NULL);
			ret &= WriteFile(hFile, ColorBits, ColorBitsSize, &dwWrite, NULL);
			ret &= WriteFile(hFile, MonoBits, MonoBitsSize, &dwWrite, NULL);
			CloseHandle(hFile);
			free(ColorInfo);
			free(MonoInfo);
			free(ColorBits);
			free(MonoBits);
		}

	}

	DeleteObject(IconInfo.hbmColor);
	DeleteObject(IconInfo.hbmMask);

	return ret;
}

VOID InitBmpHeader(HBITMAP hBitmap, BITMAPINFOHEADER& BI, int nBit)
{
	INT Bytes;
	DIBSECTION DS;
	DS.dsBmih.biSize = 0;
	Bytes = GetObject(hBitmap, sizeof(DIBSECTION), &DS);
	if (Bytes >= sizeof(DS.dsBm) + sizeof(DS.dsBmih) &&
		DS.dsBmih.biSize >= sizeof(DS.dsBmih))
	{
		CopyMemory(&BI, &DS.dsBmih, sizeof(BITMAPINFOHEADER));
	}
	else
	{
		ZeroMemory(&BI, sizeof(BI));
		BI.biSize = sizeof(BI);
		BI.biWidth = DS.dsBm.bmWidth;
		BI.biHeight = DS.dsBm.bmHeight;
		BI.biBitCount = DS.dsBm.bmPlanes * DS.dsBm.bmBitsPixel;
	}
	if (nBit)
	{
		BI.biBitCount = static_cast<WORD>(nBit);
	}
	if (BI.biBitCount <= 8)
	{
		BI.biClrUsed = 1 << BI.biBitCount;
	}

	BI.biPlanes = 1;

	if (BI.biClrImportant > BI.biClrUsed)
	{
		BI.biClrImportant = BI.biClrUsed;
	}
	if (BI.biSizeImage == 0)
	{
		BI.biSizeImage =
			((BI.biWidth * BI.biBitCount + 31) / 32) * 4 * BI.biHeight;
	}
}

VOID GetDIBSizes(HBITMAP Bitmap,
	DWORD& InfoSize,
	DWORD& ImageSize,
	int nBit)
{
	BITMAPINFOHEADER BI;
	InitBmpHeader(Bitmap, BI, nBit);
	InfoSize = sizeof(BITMAPINFOHEADER);
	if (BI.biBitCount > 8)
	{
		if (BI.biCompression & BI_BITFIELDS)
		{
			InfoSize = InfoSize + 12;
		}
	}
	else
	{
		InfoSize += sizeof(RGBQUAD) *
			(BI.biClrUsed != 0 ? BI.biClrUsed : (1 << BI.biBitCount));
	}
	ImageSize = BI.biSizeImage;
}

VOID GetDIB(HBITMAP Bitmap, BITMAPINFO* BmpInfo, PVOID Bits, INT nBit)
{
	HDC DC = CreateCompatibleDC(NULL);
	InitBmpHeader(Bitmap, BmpInfo->bmiHeader, nBit);
	GetDIBits(DC, Bitmap, 0,
		BmpInfo->bmiHeader.biHeight, Bits, BmpInfo, DIB_RGB_COLORS);
	DeleteDC(DC);
}
