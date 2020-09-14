#pragma once
#include <chrono>
#include "LEscreenObjects.cpp"
#include <shobjidl_core.h>
#include <atlstr.h>
#include <queue> 

class globals {
public:
	static bool g_debug;

	static unsigned char screen;

	static std::vector<screenObject*> g_ScreenObjects;
	static level* g_level;
	static std::wstring fileName;

	static long g_mouseX;
	static long g_mouseY;
	static bool g_mouseDown;

	static unsigned long long timeNow;

	static std::map<WPARAM, bool> g_modKeys;

	static unsigned int refreshRate;
	static unsigned int g_fps;

	static unsigned char prevCell;
	static unsigned short prevTile;

	static unsigned char curStageX;
	static unsigned char curStageY;
	static bool loop;

	static bool layer;
	static bool showForeground;
	static bool showBackground;

	static std::vector<tileSet*> tilesets;
	static tileSet* bpIcons;
	static bool showIcons;

	static unsigned char curTileSet;
	static unsigned short curTile;

	static bool useDefault;
	static bool solid;
	static bool deadly;
	static float friction;
	static unsigned short sID;
	static int xOffset;
	static int yOffset;

	static bool grid;
};
