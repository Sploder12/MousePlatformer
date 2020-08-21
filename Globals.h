#pragma once
#include <vector>
#include <map>
#include <chrono>
#include "screenObjects.cpp"

class globals {
public:
	static bool g_debug;

	static std::vector<screenObject*> g_ScreenObjects;
	static level* g_level;
	static player* g_player;

	static long g_mouseX;
	static long g_mouseY;
	static bool g_mouseDown;

	static unsigned long long timeNow;

	static std::map<WPARAM, bool> g_modKeys;

	static unsigned int refreshRate;
	static float g_fps;
};
