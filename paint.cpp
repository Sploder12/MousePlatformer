#include "paint.h"

#define BGCOLOR RGB(28, 48, 68)
HBRUSH bg = CreateSolidBrush(BGCOLOR);

RECT createRECT(int left, int top, int width, int height)
{
	RECT rect = RECT();
	rect.left = left;
	rect.top = top;
	rect.right = width+left;
	rect.bottom = height+top;
	return rect;
}

void drawText(HDC* hdc, std::string txt, RECT rect, unsigned int flag, COLORREF bgcol)
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
	int width = GetDeviceCaps(hdc, HORZSIZE)*4;
	int height = GetDeviceCaps(hdc, VERTSIZE)*3;

	HDC buffer = CreateCompatibleDC(hdc);
	HBITMAP buffermap = CreateCompatibleBitmap(hdc, width, height);
	SelectObject(buffer, buffermap);

	FillRect(buffer, &ps.rcPaint, bg);
	COLORREF tempCol;
	RECT temprect;
	switch (globals::curScreen)
	{
	case 2:
		tempCol = globals::Resume->draw(&buffer, globals::g_mouseX, globals::g_mouseY, globals::g_mouseDown);
		SelectObject(buffer, buffermap);
		temprect = globals::Resume->rect;
		temprect.top = ((temprect.bottom + temprect.top) / 2) - 8;
		drawText(&buffer, globals::Resume->text, temprect, globals::Resume->txtFlag, tempCol);
		tempCol = globals::Options->draw(&buffer, globals::g_mouseX, globals::g_mouseY, globals::g_mouseDown);
		temprect = globals::Options->rect;
		temprect.top = ((temprect.bottom + temprect.top) / 2) - 8;
		drawText(&buffer, globals::Options->text, temprect, globals::Options->txtFlag, tempCol);
		tempCol = globals::Exit->draw(&buffer, globals::g_mouseX, globals::g_mouseY, globals::g_mouseDown);
		temprect = globals::Exit->rect;
		temprect.top = ((temprect.bottom + temprect.top) / 2) - 8;
		drawText(&buffer, globals::Exit->text, temprect, globals::Exit->txtFlag, tempCol);
		break;
	case 3:
		stage* background = globals::g_level->background;
		stage* foreground = globals::g_level->foreground;
		std::vector<tile*>* dat = &background[(globals::g_player->Cx) + (globals::g_player->Cy * globals::g_level->lw)].data;
		if (background[(globals::g_player->Cx) + (globals::g_player->Cy * globals::g_level->lw)].exists) {
			for (unsigned char i = 0; i < 108; i++)
			{
				temprect = createRECT((i % 12) * 64, int(floor(i / 12)) * 64, 64, 64);
				if (intersecting(temprect, *region)) dat->at(i)->draw(&buffer, temprect.left, temprect.top);
			}
		}
		dat = &foreground[(globals::g_player->Cx) + (globals::g_player->Cy * globals::g_level->lw)].data;
		if (foreground[(globals::g_player->Cx) + (globals::g_player->Cy * globals::g_level->lw)].exists) {
			for (unsigned char i = 0; i < 108; i++)
			{
				temprect = createRECT((i % 12) * 64, int(floor(i / 12)) * 64, 64, 64);
				if (intersecting(temprect, *region)) dat->at(i)->draw(&buffer, temprect.left, temprect.top);
			}
		}

		if (intersecting(globals::g_player->rect, *region)) globals::g_player->draw(&buffer, globals::g_mouseX, globals::g_mouseY, globals::g_mouseDown);

		for (unsigned short i = 0; i < globals::g_ScreenObjects.size(); i++)
		{
			if (intersecting(globals::g_ScreenObjects.at(i)->rect, *region))
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
		break;
	}
	BitBlt(hdc, 0, 0, width, height, buffer, 0, 0, SRCCOPY);
	DeleteDC(buffer);
	DeleteDC(hdc);
	DeleteObject(buffermap);
	EndPaint(hWnd, &ps);
}


