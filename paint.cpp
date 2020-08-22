#include "paint.h"

const COLORREF BGCOLOR = RGB(30, 40, 50);
HBRUSH bg = CreateSolidBrush(BGCOLOR);

RECT createRECT(LONG left, LONG top, LONG width, LONG height)
{
	RECT rect = RECT();
	rect.left = left;
	rect.top = top;
	rect.right = width+left;
	rect.bottom = height+top;
	return rect;
}

void drawText(HDC* hdc, std::string txt, RECT rect, int flag, COLORREF bgcol)
{
	COLORREF oldBGcol = GetBkColor(*hdc);
	if (bgcol == NULL)
		bgcol = oldBGcol;
	std::wstring stemp = std::wstring(txt.begin(), txt.end());
	LPCWSTR sw = stemp.c_str();
	SetBkColor(*hdc, bgcol);
	DrawText(*hdc, sw, -1, &rect, flag);
	SetBkColor(*hdc, oldBGcol);
}

void pnt(HWND hWnd)
{
	PAINTSTRUCT ps;

	HDC hdc = BeginPaint(hWnd, &ps);
	int width = GetDeviceCaps(hdc, HORZSIZE)*4;
	int height = GetDeviceCaps(hdc, VERTSIZE)*3;

	HDC buffer = CreateCompatibleDC(hdc);
	HBITMAP buffermap = CreateCompatibleBitmap(hdc, width, height);
	SelectObject(buffer, buffermap);

	FillRect(buffer, &ps.rcPaint, bg);

	globals::g_level->draw(&buffer, globals::g_player->Cx, globals::g_player->Cy, globals::g_mouseDown);
	globals::g_player->draw(&buffer, globals::g_mouseX, globals::g_mouseY, globals::g_mouseDown);
	

	for (unsigned int i = 0; i < globals::g_ScreenObjects.size(); i++)
	{
		if(globals::g_debug == false and globals::g_ScreenObjects.at(i)->debug == true)
			globals::g_ScreenObjects.at(i)->hide();
		else if(globals::g_ScreenObjects.at(i)->debug == true)
			globals::g_ScreenObjects.at(i)->show();

		if (globals::g_ScreenObjects.at(i)->visible)
		{
			COLORREF tempCol = globals::g_ScreenObjects.at(i)->draw(&buffer, globals::g_mouseX, globals::g_mouseY, globals::g_mouseDown);
			SelectObject(buffer, buffermap);
			RECT temprect = globals::g_ScreenObjects.at(i)->rect;
			temprect.top = ((temprect.bottom + temprect.top) / 2) - 8;
			drawText(&buffer, globals::g_ScreenObjects.at(i)->text, temprect, globals::g_ScreenObjects.at(i)->txtFlag, tempCol);
		}
	}

	BitBlt(hdc, 0, 0, width, height, buffer, 0, 0, SRCCOPY);
	DeleteDC(buffer);
	DeleteDC(hdc);
	DeleteObject(buffermap);
	EndPaint(hWnd, &ps);
}


