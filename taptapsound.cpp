#include "framework.h"
#include "taptapsound.h"
#include <chrono>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HBITMAP TSimg;
HBITMAP playerSprite;

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
    while (GetMessage(&msg, nullptr, 0, 0))
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

void thing(int n)
{
    globals::g_ScreenObjects.at(0)->color = RGB(50, 120, 200);
}

tileSet* tileset = new tileSet(TSimg, 64, 64);
std::vector<tileSet*> tilesets;
level* lvl;;

Text* mouseXtxt = new Text(createRECT(0, 0, 150, 18), RGB(255, 255, 255), "NULL", DT_LEFT, true);
Text* mouseYtxt = new Text(createRECT(0, 17, 150, 18), RGB(255, 255, 255), "NULL", DT_LEFT, true);
Text* mouseDowntxt = new Text(createRECT(0, 34, 150, 18), RGB(255, 255, 255), "NULL", DT_LEFT, true);

Text* timetxt = new Text(createRECT(0, 51, 250, 18), RGB(255, 255, 255), "Time Now: " + std::to_string(globals::timeNow), DT_LEFT, true);

Text* modtxt1 = new Text(createRECT(0, 75, 250, 18), RGB(255, 255, 255), "Shifting: " + std::to_string(globals::g_modKeys.at(VK_SHIFT)), DT_LEFT, true);
Text* modtxt2 = new Text(createRECT(0, 92, 250, 18), RGB(255, 255, 255), "Control: " + std::to_string(globals::g_modKeys.at(VK_CONTROL)), DT_LEFT, true);
Text* modtxt3 = new Text(createRECT(0, 109, 250, 18), RGB(255, 255, 255), "Tab: " + std::to_string(globals::g_modKeys.at(VK_TAB)), DT_LEFT, true);

Text* debugtxt = new Text(createRECT(0, 140, 250, 18), RGB(255, 255, 255), "Debug: " + std::to_string(globals::g_debug), DT_LEFT, true);

Text* fpstxt = new Text(createRECT(0, 165, 250, 18), RGB(255, 255, 255), "FPS: " + std::to_string(globals::g_fps), DT_LEFT, true);
void buildScreenObjects()
{
    tileset->source = TSimg;
    tilesets.push_back(tileset);
    lvl = new level("level1.txt", &tilesets);
    globals::g_level = lvl;

    globals::g_player = new player(createRECT(lvl->startX,lvl->startY,64,64), playerSprite);
    globals::g_player->Cx = lvl->startCX;
    globals::g_player->Cy = lvl->startCY;
    globals::g_player->setX(lvl->startX*64);
    globals::g_player->setY(lvl->startY*64);

    globals::g_ScreenObjects.reserve(9);

    globals::g_ScreenObjects.push_back(mouseXtxt);
    globals::g_ScreenObjects.push_back(mouseYtxt);
    globals::g_ScreenObjects.push_back(mouseDowntxt);

    globals::g_ScreenObjects.push_back(timetxt);
    
    globals::g_ScreenObjects.push_back(modtxt1);
    globals::g_ScreenObjects.push_back(modtxt2);
    globals::g_ScreenObjects.push_back(modtxt3);

    globals::g_ScreenObjects.push_back(debugtxt);

    globals::g_ScreenObjects.push_back(fpstxt);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
      CW_USEDEFAULT, 0, 784, 616, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   TSimg = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TILESET1));
   playerSprite = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PLAYER1));
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
    case 0x44:
        if (globals::g_modKeys.at(VK_SHIFT) && globals::g_modKeys.at(VK_TAB) && globals::g_modKeys.at(VK_CONTROL))
        {
            globals::g_debug = !globals::g_debug;
            debugtxt->text = "Debug: " + std::to_string(globals::g_debug);
        }
            
        break;
    }
}

void depressed(WPARAM key)
{
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
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
        pnt(hWnd);
        }
        break;
    case WM_ERASEBKGND:
        return TRUE;
    case WM_MOUSEMOVE:
        globals::g_mouseX = GET_X_LPARAM(lParam);
        globals::g_mouseY = GET_Y_LPARAM(lParam);
        mouseXtxt->text = "Mouse X: " + std::to_string(globals::g_mouseX);
        mouseYtxt->text = "Mouse Y: " + std::to_string(globals::g_mouseY);
        break;
    case WM_LBUTTONDOWN:
        globals::g_mouseDown = true;
        mouseDowntxt->text = "Mouse Down: True";
        for (unsigned int i = 0; i < globals::g_ScreenObjects.size(); i++)
        {
            if (globals::g_ScreenObjects.at(i)->active && globals::g_ScreenObjects.at(i)->Touching(globals::g_mouseX, globals::g_mouseY))
            {
                globals::g_ScreenObjects.at(i)->press(i);
            }
        }
        break;
    case WM_LBUTTONUP:
        globals::g_mouseDown = false;
        mouseDowntxt->text = "Mouse Down: False";
        break;
    case WM_KEYDOWN:
        switch (wParam) 
        {
        case VK_SHIFT:
            globals::g_modKeys.at(VK_SHIFT) = true;
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
        modtxt1->text = "Shifting: " + std::to_string(globals::g_modKeys.at(VK_SHIFT));
        modtxt2->text = "Control: " + std::to_string(globals::g_modKeys.at(VK_CONTROL));
        modtxt3->text = "Tab: " + std::to_string(globals::g_modKeys.at(VK_TAB));
        break;
    case WM_KEYUP:
        switch (wParam)
        {
        case VK_SHIFT:
            globals::g_modKeys.at(VK_SHIFT) = false;
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
        modtxt1->text = "Shifting: " + std::to_string(globals::g_modKeys.at(VK_SHIFT));
        modtxt2->text = "Control: " + std::to_string(globals::g_modKeys.at(VK_CONTROL));
        modtxt3->text = "Tab: " + std::to_string(globals::g_modKeys.at(VK_TAB));
        break;
    case WM_TIMER:
        switch (wParam)
        {
        case 1:
            unsigned long long lasttime = globals::timeNow;
            globals::timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            globals::g_fps = 1000/(float)(globals::timeNow - lasttime);
            fpstxt->text = "FPS: " + std::to_string(globals::g_fps);
            timetxt->text = "Time Now: " + std::to_string(globals::timeNow);
            InvalidateRect(hWnd, &createRECT(0, 0, 10000, 10000), true);
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

