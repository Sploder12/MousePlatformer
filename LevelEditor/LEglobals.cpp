#include "LEGlobals.h"

bool globals::g_debug = true;

unsigned char globals::screen = 0; //0 is main, 1 is new popup

std::vector<screenObject*> globals::g_ScreenObjects;
level* globals::g_level;
std::wstring globals::fileName = L"![untitledLevel]!";

long globals::g_mouseY = 0;
long globals::g_mouseX = 0;
bool globals::g_mouseDown = false;

unsigned long long globals::timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

std::map<WPARAM, bool> globals::g_modKeys = { {VK_SHIFT, false}, {VK_CONTROL, false}, {VK_TAB, false} };

unsigned int globals::refreshRate = 13;
unsigned int globals::g_fps = 0;

//used for placing optimizations
unsigned char globals::prevCell = 61337; //defaults are values that would not be possible normally
unsigned short globals::prevTile = 65535;

unsigned char globals::curStageX = 0;
unsigned char globals::curStageY = 0; 
bool globals::loop = true;

bool globals::layer = true; //true is foreground, false is background
bool globals::showForeground = true;
bool globals::showBackground = true;

std::vector<tileSet*> globals::tilesets;
tileSet* globals::bpIcons;
bool globals::showIcons = true;

unsigned char globals::curTileSet = 1;
unsigned short globals::curTile = 0;

//tile mods
bool globals::useDefault = true;
bool globals::solid = true;
bool globals::deadly = false;
float globals::friction = 0.01f;
unsigned short globals::sID = 0;
int globals::xOffset = 0;
int globals::yOffset = 0;

bool globals::grid = true;