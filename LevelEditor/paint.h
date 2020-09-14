#pragma once
#include <Windows.h>
#include "LEglobals.h"
#include <string>

RECT createRECT(int left, int top, int width, int height);
void drawText(HDC* hdc, std::string txt, RECT rect, unsigned int flag, COLORREF bgcol);
void pnt(HWND hWnd, RECT* region);