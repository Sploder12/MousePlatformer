#pragma once
#include <Windows.h>
#include "Globals.h"
#include <string>

RECT createRECT(LONG left, LONG top, LONG width, LONG height);
void drawText(HDC* hdc, std::string txt, RECT rect, int flag, COLORREF bgcol);
void pnt(HWND hWnd);