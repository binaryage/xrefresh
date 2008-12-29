// Copyright (c) 2006, Sven Groot
#pragma once

//////////////////////////////////////////////////////////////////////////
// CGuid
class CDPIHelper {
public:
	static float                                  ScaleX(float value);
	static float                                  ScaleY(float value);
	static bool                                   ScaleBitmap(CBitmap& bitmap);
	static bool                                   NeedScale();

private:
	static float                                  GetLogPixelsX();
	static float                                  GetLogPixelsY();

	static float                                  m_fScaleX;
	static float                                  m_fScaleY;
};