// Copyright (c) 2006, Sven Groot

#include "stdafx.h"

using namespace Gdiplus;

const float g_baseDpi = 96.0f;

float CDPIHelper::m_fScaleX = CDPIHelper::GetLogPixelsX() / g_baseDpi;
float CDPIHelper::m_fScaleY = CDPIHelper::GetLogPixelsY() / g_baseDpi;

float 
CDPIHelper::ScaleX(float value)
{
	return value * m_fScaleX;
}

float 
CDPIHelper::ScaleY(float value)
{
	return value * m_fScaleY;
}


float 
CDPIHelper::GetLogPixelsX()
{
	CDC dc(GetDC(NULL));
	int value = dc.GetDeviceCaps(LOGPIXELSX);
	return static_cast<float>(value);
}

float 
CDPIHelper::GetLogPixelsY()
{
	CDC dc(GetDC(NULL));
	int value = dc.GetDeviceCaps(LOGPIXELSY);
	return static_cast<float>(value);
}

bool 
CDPIHelper::NeedScale()
{
	return (!(m_fScaleX==1 && m_fScaleY==1));
}

bool 
CDPIHelper::ScaleBitmap(CBitmap& bitmap)
{
	if (!NeedScale()) return true;

	CDC sourceDC(CreateCompatibleDC(NULL));
	if (sourceDC.IsNull()) return false;
	sourceDC.SelectBitmap(bitmap);

	BITMAPINFO info;
	ZeroMemory(&info, sizeof(info));
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	if (GetDIBits(sourceDC, bitmap, 0, 0, NULL, &info, 0))
	{
		int scaledWidth = static_cast<int>(ScaleX(static_cast<float>(info.bmiHeader.biWidth)));
		int scaledHeight = static_cast<int>(ScaleY(static_cast<float>(info.bmiHeader.biHeight)));
		CBitmap result;
		result.CreateCompatibleBitmap(sourceDC, scaledWidth, scaledHeight);
		if (result.IsNull()) return false;
		CDC destDC(CreateCompatibleDC(NULL));
		if (destDC.IsNull()) return false;
		destDC.SelectBitmap(result);
		destDC.SetStretchBltMode(HALFTONE);
		if (destDC.StretchBlt(0, 0, scaledWidth, scaledHeight, sourceDC, 0, 0, info.bmiHeader.biWidth, info.bmiHeader.biHeight, SRCCOPY))
		{
			// rescale done
			bitmap = result.Detach();
		}
	}
	return true;
}