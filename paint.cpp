#include "paint.h"

#define BGCOLOR RGB(195, 195, 195)
HBRUSH bg = CreateSolidBrush(BGCOLOR);



RECT createRECT(int left, int top, int width, int height)
{
	RECT rect = RECT();
	rect.left = left;
	rect.top = top;
	rect.right = width + left;
	rect.bottom = height + top;
	return rect;
}

void drawText(HDC* hdc, std::wstring txt, RECT rect, unsigned int flag, COLORREF bgcol)
{
	COLORREF oldBGcol = GetBkColor(*hdc);
	if (bgcol == NULL)
		bgcol = oldBGcol;
	LPCWSTR sw = txt.c_str();
	SetBkColor(*hdc, bgcol);
	DrawText(*hdc, sw, -1, &rect, flag);
	SetBkColor(*hdc, oldBGcol);
}

bool intersecting(RECT rect1, RECT rect2)
{
	if (rect1.left >= rect2.right || rect2.left >= rect1.right)
		return false;

	// If one rectangle is above other 
	if (rect1.top >= rect2.bottom || rect2.top >= rect1.bottom)
		return false;

	return true;
}

void pnt(HWND hWnd, RECT* region)
{
	PAINTSTRUCT ps;

	HDC hdc = BeginPaint(hWnd, &ps);
	
	HDC buffer = CreateCompatibleDC(hdc);
	HBITMAP buffermap = CreateCompatibleBitmap(hdc, 784, 752);
	SelectObject(buffer, buffermap);

	if (intersecting(*region, createRECT(0, 0, 768, 576))) {
		unsigned short curStage = globals::curStageX + globals::curStageY * globals::g_level->lw;
		if(globals::showBackground) globals::g_level->background[curStage].draw(&buffer, false);
		if(globals::showForeground) globals::g_level->foreground[curStage].draw(&buffer, globals::showIcons);


		if (globals::grid)
		{
			for (unsigned short i = 1; i <= 12; i++) //grid horizontal
			{
				MoveToEx(buffer, 64 * i, 0, NULL);
				LineTo(buffer, 64 * i, 576);
			}

			for (unsigned short i = 1; i <= 9; i++) //grid vertical
			{
				MoveToEx(buffer, 0, 64 * i, NULL);
				LineTo(buffer, 768, 64 * i);
			}
		}
	}

	if(intersecting(*region, createRECT(0, 576, 200, 140))) FillRect(buffer, &createRECT(0, 576, 200, 140), bg);

	if(intersecting(*region, createRECT(620, 576, 150, 140))) FillRect(buffer, &createRECT(620, 576, 150, 140), bg);

	if(intersecting(*region, createRECT(200, 576, 425, 140))) FillRect(buffer, &createRECT(200, 576, 425, 140), bg);

	for (unsigned short i = 0; i < globals::g_ScreenObjects.size(); i++) //buttons and images
	{
		if (intersecting(*region, globals::g_ScreenObjects.at(i)->rect))
		{
			if (globals::g_debug == false and globals::g_ScreenObjects.at(i)->debug == true)
				globals::g_ScreenObjects.at(i)->hide();
			else if (globals::g_ScreenObjects.at(i)->debug == true)
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
	}

	if (intersecting(*region, createRECT(530, 610, 64, 64)))
	{
		tile tyle = tile(globals::tilesets.at(globals::curTileSet - 1), globals::curTile % 11, globals::curTile / 11, globals::bpIcons);
		tyle.draw(&buffer, 530, 610, false);
	}

	BitBlt(hdc, 0, 0, 784, 752, buffer, 0, 0, SRCCOPY);
	DeleteDC(buffer);
	DeleteDC(hdc);
	DeleteObject(buffermap);
	EndPaint(hWnd, &ps);
}