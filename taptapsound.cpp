#include "framework.h"
#include "taptapsound.h"
#include <chrono>

#define MAX_LOADSTRING 100
#define SWIDTH 784
#define SHEIGHT 615

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND wnd;                                      // current window
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HBITMAP MMenu;
HBITMAP Paused;
HBITMAP Options;

HBITMAP TSimg;
HBITMAP TSmask;
HBITMAP playerSprite;
HBITMAP playerMask;

HBITMAP mouseSpriteU;
HBITMAP mouseSpriteA;
HBITMAP mouseSpriteB;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TAPTAPSOUND, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TAPTAPSOUND));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0x0002, 0x0202))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TAPTAPSOUND));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TAPTAPSOUND);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
    HDC hdcMem, hdcMem2;
    HBITMAP hbmMask;
    BITMAP bm;

    // Create monochrome (1 bit) mask bitmap.  
    GetObject(hbmColour, sizeof(BITMAP), &bm);
    hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

    // Get some HDCs that are compatible with the display driver
    hdcMem = CreateCompatibleDC(0);
    hdcMem2 = CreateCompatibleDC(0);

    SelectBitmap(hdcMem, hbmColour);
    SelectBitmap(hdcMem2, hbmMask);

    // Set the background colour of the colour image to the colour
    // you want to be transparent.
    SetBkColor(hdcMem, crTransparent);

    // Copy the bits from the colour image to the B+W mask... everything
    // with the background colour ends up white while everythig else ends up
    // black...Just what we wanted.
    BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

    // Take our new mask and use it to turn the transparent colour in our
    // original colour image to black so the transparency effect will
    // work right.
    BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

    DeleteDC(hdcMem);
    DeleteDC(hdcMem2);

    return hbmMask;
}

void resume(int n) 
{
    globals::prevScreen = globals::curScreen;
    globals::curScreen = 3;
}

void options(int n) 
{
    globals::prevScreen = globals::curScreen;
    globals::curScreen = 1; 
}

void quit(int n)
{
    //delete globals::g_level;
    globals::prevScreen = globals::curScreen;
    globals::curScreen = 0;
    globals::g_player->lvl -= 1;
    globals::g_player->save();
    globals::g_player->lvl = 1;
}

bool loadLevel(std::string file)
{
    bool retur = globals::g_level->loadLevel(file);
    globals::g_player->Cx = globals::g_level->startCX;
    globals::g_player->lvlStartCX = globals::g_level->startCX;
    globals::g_player->Cy = globals::g_level->startCY;
    globals::g_player->lvlStartCY = globals::g_level->startCY;
    globals::g_player->setX(float(globals::g_level->startX * 64));
    globals::g_player->setY(float(globals::g_level->startY * 64));
    globals::g_player->lvlStartX = globals::g_level->startX * 64;
    globals::g_player->lvlStartY = globals::g_level->startY * 64;
    if (!globals::g_debug) globals::g_player->canMousebox = globals::g_level->startRune;
    if (!globals::g_debug) (globals::g_player->canMousebox) ? globals::g_mousebox->show() : globals::g_mousebox->hide();
    globals::g_player->save();
    globals::g_player->lvl += 1;
    return retur;
}

tileSet* tileset = new tileSet(TSimg, TSmask, 64, 64);
std::vector<tileSet*> tilesets;
level* lvl;

Text* mouseXtxt = new Text(createRECT(0, 0, 150, 18), RGB(255, 255, 255), "NULL", DT_LEFT, true);
Text* mouseYtxt = new Text(createRECT(0, 17, 150, 18), RGB(255, 255, 255), "NULL", DT_LEFT, true);
Text* mouseDowntxt = new Text(createRECT(0, 34, 150, 18), RGB(255, 255, 255), "NULL", DT_LEFT, true);

Text* playerXtxt = new Text(createRECT(100, 0, 150, 18), RGB(255, 255, 255), "NULL", DT_LEFT, true);
Text* playerYtxt = new Text(createRECT(100, 17, 150, 18), RGB(255, 255, 255), "NULL", DT_LEFT, true);

Text* timetxt = new Text(createRECT(0, 51, 250, 18), RGB(255, 255, 255), "Time Now: " + std::to_string(globals::timeNow), DT_LEFT, true);

Text* modtxt1 = new Text(createRECT(0, 75, 250, 18), RGB(255, 255, 255), "Shifting: " + std::to_string(globals::g_modKeys.at(VK_SHIFT)), DT_LEFT, true);
Text* modtxt2 = new Text(createRECT(0, 92, 250, 18), RGB(255, 255, 255), "Control: " + std::to_string(globals::g_modKeys.at(VK_CONTROL)), DT_LEFT, true);
Text* modtxt3 = new Text(createRECT(0, 109, 250, 18), RGB(255, 255, 255), "Tab: " + std::to_string(globals::g_modKeys.at(VK_TAB)), DT_LEFT, true);

Text* debugtxt = new Text(createRECT(0, 130, 250, 18), RGB(255, 255, 255), "Debug: " + std::to_string(globals::g_debug), DT_LEFT, true);

Text* fpstxt = new Text(createRECT(0, 150, 250, 18), RGB(255, 255, 255), "FPS: " + std::to_string(globals::g_fps), DT_LEFT, true);
Text* Afpstxt = new Text(createRECT(0, 168, 250, 18), RGB(255, 255, 255), "AFPS: " + std::to_string(globals::g_fps), DT_LEFT, true);

Image* mousebox = new Image(createRECT(0, 0, 64, 64), mouseSpriteU);

void startN(int n)
{
    globals::g_player->xvel = 0.0f;
    globals::g_player->yvel = 0.0f;
    globals::g_player->lvl = 1;
    loadLevel("level1.txt");
    globals::prevScreen = globals::curScreen;
    globals::curScreen = 3;
}

void cont(int n)
{
    if (globals::g_player->load())
    {
        globals::g_player->xvel = 0.0f;
        globals::g_player->yvel = 0.0f;
        loadLevel("level" + std::to_string(globals::g_player->lvl) + ".txt");
        globals::prevScreen = globals::curScreen;
        globals::curScreen = 3;
    }
    else startN(0);
}

void end(int n)
{
    delete globals::NewGame;
    delete globals::Continue;
    delete globals::Options0;
    delete globals::End;

    delete globals::Resume;
    delete globals::Options;
    delete globals::Exit;

    delete globals::ExitOps;

    delete globals::g_player;
    if(!globals::g_level->uninit)delete globals::g_level;

    unsigned short tmp = unsigned short(tilesets.size());
    for (unsigned short i = 0; i < tmp; i++)
    {
        delete tilesets.at(i);
    }

    tmp = unsigned short(globals::g_ScreenObjects.size());
    for (unsigned short i = 0; i < tmp; i++)
    {
        delete globals::g_ScreenObjects.at(i);
    }

    exit(0);
}

void exitOptions(int i)
{
    globals::curScreen = globals::prevScreen;
}

void buildScreenObjects()
{
    globals::NewGame = new Button(createRECT(225, 35, 300, 100), RGB(70, 125, 150), &startN, "New Game");
    globals::Continue = new Button(createRECT(225, 165, 300, 100), RGB(70, 125, 150), &cont, "Continue");
    globals::Options0 = new Button(createRECT(225, 295, 300, 100), RGB(70, 125, 150), &options, "Options");
    globals::End = new Button(createRECT(225, 425, 300, 100), RGB(70, 125, 150), &end, "Exit");
    
    globals::Resume = new Button(createRECT(225, 90, 300, 100), RGB(70, 125, 150), &resume, "Resume");
    globals::Options = new Button(createRECT(225, 240, 300, 100), RGB(70, 125, 150), &options, "Options");
    globals::Exit = new Button(createRECT(225, 390, 300, 100), RGB(70, 125, 150), &quit, "Exit");

    globals::ExitOps = new Button(createRECT(225, 390, 300, 100), RGB(70, 125, 150), &exitOptions, "Exit");

    tilesets.reserve(1);
    tileset->source = TSimg;
    tileset->sourceMask = TSmask;
    tilesets.push_back(tileset);

    lvl = new level("level1.txt", &tilesets);
    globals::g_level = lvl;

    globals::g_player = new player(createRECT(lvl->startX,lvl->startY,64,64), playerSprite, playerMask, lvl, &wnd);
    globals::g_player->Cx = lvl->startCX;
    globals::g_player->Cy = lvl->startCY;
    globals::g_player->lvlStartCX = lvl->startCX;
    globals::g_player->lvlStartCY = lvl->startCY;
    globals::g_player->setX(float(lvl->startX * 64));
    globals::g_player->setY(float(lvl->startY * 64));
    globals::g_player->lvlStartX = lvl->startX * 64;
    globals::g_player->lvlStartY = lvl->startY * 64;
    if (!globals::g_debug) globals::g_player->canMousebox = globals::g_level->startRune;
    else globals::g_player->canMousebox = true;
    if (!globals::g_debug) (globals::g_player->canMousebox) ? mousebox->show() : mousebox->hide();

    globals::g_mousebox = mousebox;

    globals::g_ScreenObjects.reserve(13);

    globals::g_ScreenObjects.push_back(mouseXtxt);
    globals::g_ScreenObjects.push_back(mouseYtxt);
    globals::g_ScreenObjects.push_back(mouseDowntxt);

    globals::g_ScreenObjects.push_back(playerXtxt);
    globals::g_ScreenObjects.push_back(playerYtxt);

    globals::g_ScreenObjects.push_back(timetxt);
    
    globals::g_ScreenObjects.push_back(modtxt1);
    globals::g_ScreenObjects.push_back(modtxt2);
    globals::g_ScreenObjects.push_back(modtxt3);

    globals::g_ScreenObjects.push_back(debugtxt);

    globals::g_ScreenObjects.push_back(fpstxt);
    globals::g_ScreenObjects.push_back(Afpstxt);

    globals::g_ScreenObjects.push_back(globals::g_mousebox);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
      CW_USEDEFAULT, 0, SWIDTH, SHEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   //MMenu = LoadBitmap(hInst, MAKEINTRESOURCE(@TODO));
   //Paused = LoadBitmap(hInst, MAKEINTRESOURCE(@TODO));
   //Options = LoadBitmap(hInst, MAKEINTRESOURCE(@TODO));

   TSimg = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TILESET1));
   TSmask = CreateBitmapMask(TSimg, RGB(0,0,1));
   playerSprite = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PLAYER1));
   playerMask = CreateBitmapMask(playerSprite, RGB(0, 0, 1));
   mouseSpriteU = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BRICK1));
   mouseSpriteA = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BRICKA));
   mouseSpriteB = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BRICKB));
   
   buildScreenObjects();
   SetTimer(hWnd, 1, globals::refreshRate, NULL);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void pressed(WPARAM key)
{
    switch (key)
    {
    case VK_ESCAPE:
        if (globals::curScreen > 0) 
        {
            if (globals::curScreen == 1)
            {
                exitOptions(0);
            }
            else
            {
                globals::curScreen = (globals::curScreen == 3) ? 2 : 3;
            }
            RECT temp = { 0, 0, SWIDTH, SHEIGHT };
            InvalidateRect(wnd, &temp, true);
        }
        break;
    case 0x44:
        if (globals::g_modKeys.at(VK_SHIFT) && globals::g_modKeys.at(VK_TAB) && globals::g_modKeys.at(VK_CONTROL))
        {
            globals::g_debug = !globals::g_debug;
            debugtxt->text = "Debug: " + std::to_string(globals::g_debug);
            RECT temp = { 0, 0, SWIDTH, SHEIGHT };
            InvalidateRect(wnd, &temp, true);
        }
        else
        {
            globals::g_player->movementKeys.at(0x44) = true;
        }
        break;
    case 0x52:
        if (globals::g_modKeys.at(VK_SHIFT) && globals::g_modKeys.at(VK_TAB) && globals::g_modKeys.at(VK_CONTROL) && globals::g_debug)
        {
            if (globals::curScreen == 3) {
                globals::g_player->lvl -= 1;
                loadLevel("level" + std::to_string(globals::g_player->lvl) + ".txt");
                RECT temp = { 0, 0, SWIDTH, SHEIGHT };
                InvalidateRect(wnd, &temp, true);
            }
        }
        break;
    case VK_SPACE:
    case 0x41:
    case 0x53:
    case 0x57:
        globals::g_player->movementKeys.at(key) = true;
        break;
    }
}

void depressed(WPARAM key)
{
    switch (key)
    {
    case VK_SPACE:
    case 0x41:
    case 0x44:
    case 0x53:
    case 0x57:
        globals::g_player->movementKeys.at(key) = false;
        break;
    }
}

unsigned long long fpsTimer = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
unsigned int times = 0;
unsigned int Atimes = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    wnd = hWnd;
    switch (message)
    {
    case WM_COMMAND:
        {
            short wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
        RECT temp;
        GetUpdateRect(hWnd, &temp, true);
        pnt(hWnd, &temp);
        Atimes++;
        }
        break;
    case WM_ERASEBKGND:
        return TRUE;
    case WM_MOUSEMOVE:
        globals::g_mouseX = GET_X_LPARAM(lParam);
        globals::g_mouseY = GET_Y_LPARAM(lParam);
        if (globals::g_debug)
        {
            mouseXtxt->text = "Mouse X: " + std::to_string(globals::g_mouseX);
            mouseYtxt->text = "Mouse Y: " + std::to_string(globals::g_mouseY);
        }

        if(globals::curScreen == 3)
            if (globals::g_mousebox->rect.left + 32 > 34 && globals::g_mousebox->rect.left + 32 < 734 && globals::g_mousebox->rect.top+32 > 35 && !globals::g_player->collideMouseX(float(mousebox->rect.left), float(mousebox->rect.top), false) && !globals::g_player->collideMouseY(float(mousebox->rect.left), float(mousebox->rect.top), false))
                if (globals::g_modKeys.at(VK_SHIFT) || globals::g_mouseDown) globals::g_player->mousebox = true;
        break;
    case WM_LBUTTONDOWN:
        globals::g_mouseDown = true;
        mouseDowntxt->text = "Mouse Down: True";
        switch(globals::curScreen) 
        {
        case 0:
            if (globals::NewGame->active && globals::NewGame->Touching(globals::g_mouseX, globals::g_mouseY))
            {
                globals::NewGame->press(0);
                InvalidateRect(hWnd, &createRECT(0, 0, SWIDTH, SHEIGHT), true);
            }
            if (globals::Continue->active && globals::Continue->Touching(globals::g_mouseX, globals::g_mouseY))
            {
                globals::Continue->press(0);
                InvalidateRect(hWnd, &createRECT(0, 0, SWIDTH, SHEIGHT), true);
            }
            if (globals::Options0->active && globals::Options0->Touching(globals::g_mouseX, globals::g_mouseY))
            {
                globals::Options0->press(0);
                InvalidateRect(hWnd, &createRECT(0, 0, SWIDTH, SHEIGHT), true);
            }
            if (globals::End->active && globals::End->Touching(globals::g_mouseX, globals::g_mouseY))
            {
                globals::End->press(0);
                InvalidateRect(hWnd, &createRECT(0, 0, SWIDTH, SHEIGHT), true);
            }
            break;
        case 1:
            if (globals::ExitOps->active && globals::ExitOps->Touching(globals::g_mouseX, globals::g_mouseY))
            {
                globals::ExitOps->press(0);
                InvalidateRect(hWnd, &createRECT(0, 0, SWIDTH, SHEIGHT), true);
            }
            break;
        case 2:
            if (globals::Resume->active && globals::Resume->Touching(globals::g_mouseX, globals::g_mouseY))
            {
                globals::Resume->press(0);
                InvalidateRect(hWnd, &createRECT(0, 0, SWIDTH, SHEIGHT), true);
            }
            if (globals::Options->active && globals::Options->Touching(globals::g_mouseX, globals::g_mouseY))
            {
                globals::Options->press(0);
                InvalidateRect(hWnd, &createRECT(0, 0, SWIDTH, SHEIGHT), true);
            }
            if (globals::Exit->active && globals::Exit->Touching(globals::g_mouseX, globals::g_mouseY))
            {
                globals::Exit->press(0);
                InvalidateRect(hWnd, &createRECT(0, 0, SWIDTH, SHEIGHT), true);
            }

            break;
        case 3:
            if (globals::g_mousebox->rect.left + 32 > 34 && globals::g_mousebox->rect.left + 32 < 734 && globals::g_mousebox->rect.top + 32 > 35 && !globals::g_player->collideMouseX(float(mousebox->rect.left), float(mousebox->rect.top), false) && !globals::g_player->collideMouseY(float(mousebox->rect.left), float(mousebox->rect.top), false))
                globals::g_player->mousebox = true;
            else mousebox->image = mouseSpriteB;
            for (unsigned short i = 0; i < globals::g_ScreenObjects.size(); i++)
            {
                if (globals::g_ScreenObjects.at(i)->active && globals::g_ScreenObjects.at(i)->Touching(globals::g_mouseX, globals::g_mouseY))
                {
                    globals::g_ScreenObjects.at(i)->press(i);
                }
            }
            break;
        }
        break;
    case WM_LBUTTONUP:
        globals::g_mouseDown = false;
        if (globals::curScreen == 3)
            if(!globals::g_modKeys.at(VK_SHIFT)) globals::g_player->mousebox = false;
        mouseDowntxt->text = "Mouse Down: False";
        break;
    case WM_KEYDOWN:
        switch (wParam) 
        {
        case VK_SHIFT:
            globals::g_modKeys.at(VK_SHIFT) = true;
            if (globals::curScreen == 3)
                if (globals::g_mousebox->rect.left + 32 > 34 && globals::g_mousebox->rect.left + 32 < 734 && globals::g_mousebox->rect.top + 32 > 35 && !globals::g_player->collideMouseX(float(mousebox->rect.left), float(mousebox->rect.top), false) && !globals::g_player->collideMouseY(float(mousebox->rect.left), float(mousebox->rect.top), false))
                    globals::g_player->mousebox = true;
                else mousebox->image = mouseSpriteB;
            break;
        case VK_CONTROL:
            globals::g_modKeys.at(VK_CONTROL) = true;
            break;
        case VK_TAB:
            globals::g_modKeys.at(VK_TAB) = true;
            break;
        default:
            pressed(wParam);
        }
        if (globals::g_debug)
        {
            modtxt1->text = "Shifting: " + std::to_string(globals::g_modKeys.at(VK_SHIFT));
            modtxt2->text = "Control: " + std::to_string(globals::g_modKeys.at(VK_CONTROL));
            modtxt3->text = "Tab: " + std::to_string(globals::g_modKeys.at(VK_TAB));
        }
        break;
    case WM_KEYUP:
        switch (wParam)
        {
        case VK_SHIFT:
            globals::g_modKeys.at(VK_SHIFT) = false;
            if (globals::curScreen == 3)
                if(!globals::g_mouseDown)globals::g_player->mousebox = false;
            break;
        case VK_CONTROL:
            globals::g_modKeys.at(VK_CONTROL) = false;
            break;
        case VK_TAB:
            globals::g_modKeys.at(VK_TAB) = false;
            break;
        default:
            depressed(wParam);
        }
        if (globals::g_debug)
        {
            modtxt1->text = "Shifting: " + std::to_string(globals::g_modKeys.at(VK_SHIFT));
            modtxt2->text = "Control: " + std::to_string(globals::g_modKeys.at(VK_CONTROL));
            modtxt3->text = "Tab: " + std::to_string(globals::g_modKeys.at(VK_TAB));
        }
        break;
    case WM_TIMER:
        switch (wParam)
        {
        case 1:
            globals::timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            if(globals::g_debug) timetxt->text = "Time Now: " + std::to_string(globals::timeNow);
            if (globals::timeNow - fpsTimer >= 1000)
            {
                fpsTimer += 1000;
                globals::g_fps = times;
                if (globals::g_debug) 
                {
                    fpstxt->text = "FPS: " + std::to_string(globals::g_fps);
                    Afpstxt->text = "AFPS: " + std::to_string(Atimes);
                }
                times = 1;
                Atimes = 0;
            }
            else {
                times++;
            }
            switch(globals::curScreen )
            {
            case 0:
            case 1:
            case 2:
                InvalidateRect(hWnd, &createRECT(0, 0, SWIDTH, SHEIGHT), true);
                break;
            case 3:
                unsigned short playerCell = (globals::g_player->Cx) + (globals::g_player->Cy * globals::g_level->lw);
                RECT temp = createRECT(mousebox->rect.left - 32, mousebox->rect.top - 32, 128, 128);
                if (globals::g_player->canMousebox) globals::g_mousebox->show();
                if (!globals::g_player->mousebox)
                {
                    mousebox->rect.left = globals::g_mouseX - 32;
                    mousebox->rect.right = globals::g_mouseX + 32;
                    mousebox->rect.top = globals::g_mouseY - 32;
                    mousebox->rect.bottom = globals::g_mouseY + 32;
                    if (!globals::g_modKeys.at(VK_SHIFT) && !globals::g_mouseDown)
                        mousebox->image = mouseSpriteU;
                    InvalidateRect(hWnd, &(temp), true);
                }
                else {
                    mousebox->image = mouseSpriteA;
                    InvalidateRect(hWnd, &(temp), true);
                }
                temp = createRECT(globals::g_player->rect.left - 16, globals::g_player->rect.top - 16, 86, 86);
                InvalidateRect(hWnd, &(temp), true);
                globals::g_player->movePlayer(&(globals::g_level->foreground[playerCell]), float(mousebox->rect.left), float(mousebox->rect.top), globals::g_player->mousebox);
                temp = createRECT(globals::g_player->rect.left - 16, globals::g_player->rect.top - 16, 86, 86);
                InvalidateRect(hWnd, &(temp), true);
                if (globals::g_player->rect.left >= 768)
                {
                    if (globals::g_player->Cx < globals::g_level->lw - 1)
                    {
                        globals::g_player->Cx += 1;
                        globals::g_player->setX(-62);
                        globals::g_player->moveY(-globals::g_player->yvel);
                    }
                    else
                    {
                        if (globals::g_level->endS == 2 && globals::g_player->Cy == globals::g_level->endCY)
                        {
                            loadLevel("level" + std::to_string(globals::g_player->lvl) + ".txt");
                        }
                        else
                        {
                            globals::g_player->xvel = -1.0f;
                            globals::g_player->setX(766);
                        }
                    }
                    InvalidateRect(hWnd, &createRECT(0, 0, SWIDTH, SHEIGHT), true);
                }
                else if (globals::g_player->rect.left + 64 <= 0)
                {
                    if (globals::g_player->Cx > 0)
                    {
                        globals::g_player->Cx -= 1;
                        globals::g_player->setX(767);
                        globals::g_player->moveY(-globals::g_player->yvel);
                    }
                    else
                    {
                        if (globals::g_level->endS == 0 && globals::g_player->Cy == globals::g_level->endCY)
                        {
                            loadLevel("level" + std::to_string(globals::g_player->lvl) + ".txt");
                        }
                        else
                        {
                            globals::g_player->xvel = 1.0f;
                            globals::g_player->setX(-63);
                        }
                    }
                    InvalidateRect(hWnd, &createRECT(0, 0, SWIDTH, SHEIGHT), true);
                }

                if (globals::g_player->rect.top >= 576)
                {
                    if (globals::g_player->Cy < globals::g_level->lh - 1)
                    {
                        globals::g_player->Cy += 1;
                        globals::g_player->setY(-62);
                    }
                    else
                    {
                        if (globals::g_level->endS == 3 && globals::g_player->Cx == globals::g_level->endCX)
                        {
                            loadLevel("level" + std::to_string(globals::g_player->lvl) + ".txt");
                        }
                        else
                        {
                            globals::g_player->yvel = 0;
                            globals::g_player->setY(575);
                            globals::g_player->grounded = true;
                        }
                    }
                    InvalidateRect(hWnd, &createRECT(0, 0, SWIDTH, SHEIGHT), true);
                }
                else if (globals::g_player->rect.top + 64 <= 0)
                {
                    if (globals::g_player->Cy > 0)
                    {
                        globals::g_player->Cy -= 1;
                        globals::g_player->setY(575);
                    }
                    else
                    {
                        if (globals::g_level->endS == 0 && globals::g_player->Cx == globals::g_level->endCX)
                        {
                            globals::g_level->loadLevel("level" + std::to_string(globals::g_player->lvl) + ".txt");
                        }
                        else
                        {
                            globals::g_player->yvel = 0;
                            globals::g_player->setY(-62);
                        }
                    }
                    InvalidateRect(hWnd, &createRECT(0, 0, SWIDTH, SHEIGHT), true);
                }
                break;
            }
            playerXtxt->text = "Player X: " + std::to_string(globals::g_player->rect.left);
            playerYtxt->text = "Player Y: " + std::to_string(globals::g_player->rect.top);
            if(globals::g_debug) InvalidateRect(hWnd, &createRECT(0, 0, 210, 190), true);
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

