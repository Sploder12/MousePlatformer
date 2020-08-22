#include "Globals.h"

bool globals::g_debug = false;

std::vector<screenObject*> globals::g_ScreenObjects;
level* globals::g_level;
player* globals::g_player;
Image* globals::g_mousebox;

long globals::g_mouseY = 0;
long globals::g_mouseX = 0;
bool globals::g_mouseDown = false;

unsigned long long globals::timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

std::map<WPARAM, bool> globals::g_modKeys = { {VK_SHIFT, false}, {VK_CONTROL, false}, {VK_TAB, false} };

unsigned int globals::refreshRate = 13;
float globals::g_fps = 0;