#include "framework.h"
#include "levelEditor.h"

#define MAX_LOADSTRING 100
#define SWIDTH 784
#define SHEIGHT 752

#define OBJECTCOLOR RGB(155, 160, 165)

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND wnd;                                      // current window
HMENU menu;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HBITMAP TSimg;
HBITMAP TSmask;

HBITMAP BPimg;
HBITMAP BPmask;

HBITMAP StartFlag;
HBITMAP StartFlagMask;

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
    LoadStringW(hInstance, IDC_LEVELEDITOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LEVELEDITOR));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0x0002, 0x020A))
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LEVELEDITOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LEVELEDITOR);
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

void optimizeCur()
{
    unsigned short curstage = globals::curStageX + globals::curStageY * globals::g_level->lw;
    globals::g_level->foreground[curstage].optimize();
    globals::g_level->background[curstage].optimize();
}

void unoptimizeCur()
{
    unsigned short curstage = globals::curStageX + globals::curStageY * globals::g_level->lw;
    globals::g_level->foreground[curstage].unoptimize();
    globals::g_level->background[curstage].unoptimize();
}

Text* stageYn = new Text(createRECT(30, 633, 125, 25), RGB(195, 195, 195), L"Stage Y: " + std::to_wstring(globals::curStageY));
void moveDstage(int n)
{
    optimizeCur();
    if (globals::curStageY > globals::g_level->lh - 2)
    {
        if (globals::loop)globals::curStageY = 0;
    }
    else globals::curStageY++;
    unoptimizeCur();
    stageYn->text = L"Stage Y: " + std::to_wstring(globals::curStageY);
    RECT temp = { 0, 0, 768, 576 };
    InvalidateRect(wnd, &temp, true);
}

void moveUstage(int n)
{
    optimizeCur();
    if (globals::curStageY < 1)
    {
        if (globals::loop)globals::curStageY = globals::g_level->lh - 1;
    }
    else globals::curStageY--;
    unoptimizeCur();
    stageYn->text = L"Stage Y: " + std::to_wstring(globals::curStageY);
    RECT temp = { 0, 0, 768, 576 };
    InvalidateRect(wnd, &temp, true);
}

Text* stageXn = new Text(createRECT(30, 613, 125, 25), RGB(195, 195, 195), L"Stage X: " + std::to_wstring(globals::curStageX));
void moveLstage(int n)
{
    optimizeCur();
    if (globals::curStageX < 1)
    {
        if (globals::loop)globals::curStageX = globals::g_level->lw - 1;
    }
    else globals::curStageX--;
    unoptimizeCur();
    stageXn->text = L"Stage X: " + std::to_wstring(globals::curStageX);
    RECT temp = { 0, 0, 768, 576 };
    InvalidateRect(wnd, &temp, true);
}

void moveRstage(int n)
{
    optimizeCur();
    if (globals::curStageX > globals::g_level->lw - 2)
    {
        if (globals::loop)globals::curStageX = 0;
    }
    else globals::curStageX++;
    unoptimizeCur();
    stageXn->text = L"Stage X: " + std::to_wstring(globals::curStageX);
    RECT temp = { 0, 0, 768, 576 };
    InvalidateRect(wnd, &temp, true);
}

void settile(int n)
{
    globals::curTile = n;
    RECT temp = { 530, 610, 594, 674 };
    InvalidateRect(wnd, &temp, true);
}

HRESULT setFileName()
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileSaveDialog* pFileSave;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
            IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileSave->Show(wnd);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileSave->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
                        globals::fileName = pszFilePath;
                        //load(pszFilePath);
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileSave->Release();
        }
        CoUninitialize();
    }
    return 0;
}

void save(bool backupSave = false)
{
    std::ofstream file;
    if (!backupSave)
    {
        if (globals::fileName == L"![untitledLevel]!") setFileName();

        file.open(globals::fileName);
    }
    else file.open("backup.wbd");
    std::queue<std::string> modifierQ;
    if (file.is_open())
    {
        file << '@' << std::to_string(globals::g_level->lw) << '/' //This is just for the level header
            << std::to_string(globals::g_level->lh) << '/'
            << std::to_string(globals::g_level->startCX) << '/'
            << std::to_string(globals::g_level->startCY) << '/'
            << std::to_string(globals::g_level->startX) << '/'
            << std::to_string(globals::g_level->startY) << '/'
            << std::to_string(globals::g_level->endCX) << '/'
            << std::to_string(globals::g_level->endCY) << '/'
            << std::to_string(globals::g_level->endS) << '/'
            << ((globals::g_level->startRune)?'T':'F') << '/' << '\n';

        for (unsigned short i = 0; i < globals::g_level->lw * globals::g_level->lh; i++)
        {
            stage* curStageF = &globals::g_level->foreground[i];
            file << "#stage:" + std::to_string(i) + "[tileset:" + "1" + "]\n";
            file << '=';
            for (unsigned char j = 0; j < 108; j++)
            {
                if (j % 12 == 0 && j != 0)
                {
                    file << ",\n|";
                }

                unsigned short tileID = (curStageF->data.at(j)->tx + curStageF->data.at(j)->ty * 11);
                file << ((tileID < 10) ? "0" + std::to_string(tileID) : std::to_string(tileID));

                //defaults for comparison
                bool solid = (tileID > 10);
                float friction = (tileID > 32 && tileID < 44) ? 0.02f : 0.3f;
                bool killer = (tileID > 21 && tileID < 33) ? true : false;
                //stupiid work around

                if (curStageF->data.at(j)->solid != solid) modifierQ.emplace('M'+std::to_string(j)+"(s="+((curStageF->data.at(j)->solid)?"T)\n":"F)\n"));
                if (curStageF->data.at(j)->killer != killer) modifierQ.emplace('M' + std::to_string(j) + "(k=" + ((curStageF->data.at(j)->killer) ? "T)\n" : "F)\n"));
                if (curStageF->data.at(j)->friction != friction) modifierQ.emplace('M' + std::to_string(j) + "(f=" + std::to_string(curStageF->data.at(j)->friction) + ")\n");

                if (curStageF->data.at(j)->imgXOff != 0) modifierQ.emplace('M' + std::to_string(j) + "(x=" + std::to_string(curStageF->data.at(j)->imgXOff) + ")\n");

                if (curStageF->data.at(j)->imgYOff != 0) modifierQ.emplace('M' + std::to_string(j) + "(y=" + std::to_string(curStageF->data.at(j)->imgYOff) + ")\n");
              
                if (curStageF->data.at(j)->specialID != 0) modifierQ.emplace('M' + std::to_string(j) + "(S=" + std::to_string(curStageF->data.at(j)->specialID) + ")\n");
            }
            file << ";\n";

            size_t siz = modifierQ.size();
            for (size_t j = 0; j < siz; j++)
            {
                file << modifierQ.front();
                modifierQ.pop();
            }

            stage* curStageB = &globals::g_level->background[i];
            file << 'B';
            for (unsigned char j = 0; j < curStageB->data.size(); j++) //using the size so handmade levels can be properly saved
            {

                if (j % 12 == 0 && j != 0)
                {
                    file << ",\n-";
                }

                unsigned short tileID = (curStageB->data.at(j)->tx + curStageB->data.at(j)->ty * 11);
                file << ((tileID < 10) ? "0" + std::to_string(tileID) : std::to_string(tileID));
            }
            file << ";\n";
        }

        file.close();
    }

}

void saveAs()
{
    setFileName();
    save();
}

unsigned char tlw, tlh;
unsigned char tscx, tscy, tsx, tsy;
unsigned char tex, tey, tes;
bool tswr;
void newF(unsigned char lw, unsigned char lh, bool del = true)
{
    globals::fileName = L"![untitledLevel]!";
    if (del)
    {
        save(true);
        delete globals::g_level;
    }
    globals::g_level = new level(lw, lh, &globals::tilesets, globals::bpIcons);
    globals::g_level->startCX = tscx;
    globals::g_level->startCY = tscy;
    globals::g_level->startX = tsx;
    globals::g_level->startY = tsy;
    globals::g_level->endCX = tex;
    globals::g_level->endCY = tey;
    globals::g_level->endS = tes;
    globals::g_level->startRune = tswr;
    globals::curStageX = 0;
    globals::curStageY = 0;
    unoptimizeCur();
    stageXn->text = L"Stage X: " + std::to_wstring(globals::curStageX);
    stageYn->text = L"Stage Y: " + std::to_wstring(globals::curStageY);
    RECT temp = { 0, 0, 768, 576 };
    InvalidateRect(wnd, &temp, true);
    
}

void selectForeground(int n); //forward declartion b/c buttons

void selectBackground(int n);

void moveRtile(int n);
void moveLtile(int n);

void xAddOff(int n);
void yAddOff(int n);
void xSubOff(int n);
void ySubOff(int n);

void resetOffsets(int n);

void Addsid(int n);
void Subsid(int n);

void Addfric(int n);
void Subfric(int n);

tileSet* tileset = new tileSet(TSimg, TSmask, 64, 64);
level* lvl;

//debug stuff
Text* mouseXtxt = new Text(createRECT(0, 0, 150, 18), RGB(255, 255, 255), L"NULL", DT_LEFT, true);
Text* mouseYtxt = new Text(createRECT(0, 17, 150, 18), RGB(255, 255, 255), L"NULL", DT_LEFT, true);
Text* mouseDowntxt = new Text(createRECT(0, 34, 150, 18), RGB(255, 255, 255), L"NULL", DT_LEFT, true);

Text* timetxt = new Text(createRECT(0, 51, 250, 18), RGB(255, 255, 255), L"Time Now: " + std::to_wstring(globals::timeNow), DT_LEFT, true);

Text* modtxt1 = new Text(createRECT(0, 75, 250, 18), RGB(255, 255, 255), L"Shifting: " + std::to_wstring(globals::g_modKeys.at(VK_SHIFT)), DT_LEFT, true);
Text* modtxt2 = new Text(createRECT(0, 92, 250, 18), RGB(255, 255, 255), L"Control: " + std::to_wstring(globals::g_modKeys.at(VK_CONTROL)), DT_LEFT, true);
Text* modtxt3 = new Text(createRECT(0, 109, 250, 18), RGB(255, 255, 255), L"Tab: " + std::to_wstring(globals::g_modKeys.at(VK_TAB)), DT_LEFT, true);

Text* debugtxt = new Text(createRECT(0, 130, 250, 18), RGB(255, 255, 255), L"Debug: " + std::to_wstring(globals::g_debug), DT_LEFT, true);

Text* fpstxt = new Text(createRECT(0, 150, 250, 18), RGB(255, 255, 255), L"FPS: " + std::to_wstring(globals::g_fps), DT_LEFT, true);
Text* Afpstxt = new Text(createRECT(0, 168, 250, 18), RGB(255, 255, 255), L"AFPS: " + std::to_wstring(globals::g_fps), DT_LEFT, true);

//UI
Button* stageL = new Button(createRECT(5, 620, 50, 32), OBJECTCOLOR, moveLstage, L"<-");
Button* stageR = new Button(createRECT(135, 620, 50, 32), OBJECTCOLOR, moveRstage, L"->");

Button* stageU = new Button(createRECT(69, 580, 50, 32), OBJECTCOLOR, moveUstage, L"/\\");
Button* stageD = new Button(createRECT(69, 658, 50, 32), OBJECTCOLOR, moveDstage, L"\\/");

Text* loopTxt = new Text(createRECT(125, 583, 40, 25), RGB(195, 195, 195), L"Wrap");
CheckBox* loopStage = new CheckBox(createRECT(165, 580, 30, 30), OBJECTCOLOR, &globals::loop);

Button* selFore = new Button(createRECT(200, 610, 150, 32), OBJECTCOLOR, selectForeground, L"Foreground");
Button* selBack = new Button(createRECT(200, 650, 150, 32), OBJECTCOLOR, selectBackground, L"Background");
CheckBox* showFore = new CheckBox(createRECT(351, 610, 32, 32), OBJECTCOLOR, &globals::showForeground, L"Vis");
CheckBox* showBack = new CheckBox(createRECT(351, 650, 32, 32), OBJECTCOLOR, &globals::showBackground, L"Vis");

Text* curTileTxt = new Text(createRECT(406, 653, 32, 32), RGB(195, 195, 195), L"0");
Button* rTile = new Button(createRECT(436, 660, 18, 18), OBJECTCOLOR, moveRtile, L">");
Button* lTile = new Button(createRECT(390, 660, 18, 18), OBJECTCOLOR, moveLtile, L"<");

Text* xOffTxt = new Text(createRECT(622, 590, 120, 18), RGB(195, 195, 195), L"X Offset: 0");
Text* yOffTxt = new Text(createRECT(622, 610, 120, 18), RGB(195, 195, 195), L"Y Offset: 0");

Button* xOffP = new Button(createRECT(730, 590, 36, 18), OBJECTCOLOR, xAddOff, L">");
Button* yOffP = new Button(createRECT(730, 610, 36, 18), OBJECTCOLOR, yAddOff, L">");
Button* xOffM = new Button(createRECT(597, 590, 36, 18), OBJECTCOLOR, xSubOff, L"<");
Button* yOffM = new Button(createRECT(597, 610, 36, 18), OBJECTCOLOR, ySubOff, L"<");

Button* resetOffsetsB = new Button(createRECT(675, 635, 25, 25), OBJECTCOLOR, resetOffsets, L"R");

CheckBox* useDefaultBtn = new CheckBox(createRECT(545, 590, 18, 18), OBJECTCOLOR, &globals::useDefault);
Text* useDefaultTxt = new Text(createRECT(450, 590, 100, 18), RGB(195, 195, 195), L"Use Default:");

CheckBox* useSolidBtn = new CheckBox(createRECT(545, 610, 18, 18), OBJECTCOLOR, &globals::solid);
Text* useSolidTxt = new Text(createRECT(450, 610, 100, 18), RGB(195, 195, 195), L"Solid:");

CheckBox* useKillerBtn = new CheckBox(createRECT(545, 630, 18, 18), OBJECTCOLOR, &globals::deadly);
Text* useKillerTxt = new Text(createRECT(450, 630, 100, 18), RGB(195, 195, 195), L"Deadly:");

Text* sidTxt = new Text(createRECT(622, 665, 120, 18), RGB(195, 195, 195), L"SID: 0");
Button* sidP = new Button(createRECT(730, 665, 36, 18), OBJECTCOLOR, Addsid, L">");
Button* sidM = new Button(createRECT(597, 665, 36, 18), OBJECTCOLOR, Subsid, L"<");

Text* frictionTxt = new Text(createRECT(460, 650, 100, 18), RGB(195, 195, 195), L"Friction: 0.3");
Button* frictionP = new Button(createRECT(511, 668, 18, 18), OBJECTCOLOR, Addfric, L">");
Button* frictionM = new Button(createRECT(490, 668, 18, 18), OBJECTCOLOR, Subfric, L"<");
Button* frictionPP = new Button(createRECT(530, 668, 18, 18), OBJECTCOLOR, Addfric, L">>");
Button* frictionMM = new Button(createRECT(471, 668, 18, 18), OBJECTCOLOR, Subfric, L"<<");

void selectForeground(int n)
{
    selFore->active = false;
    selBack->active = true;
    globals::layer = 1;
}

void selectBackground(int n)
{
    selBack->active = false;
    selFore->active = true;
    globals::layer = 0;
}

void moveLtile(int n)
{
    if (globals::curTile < 1)
        globals::curTile = 98;
    else globals::curTile--;
    curTileTxt->text = std::to_wstring(globals::curTile);
    if (globals::useDefault)
    {
        globals::solid = (globals::curTile > 10);
        globals::friction = (globals::curTile > 32 && globals::curTile < 44) ? 0.02f : 0.3f; //for slippery blocks
        globals::deadly = (globals::curTile > 21 && globals::curTile < 33) ? true : false;
        std::wstring fric = std::to_wstring(globals::friction);
        frictionTxt->text = L"Friction: " + fric.substr(0, fric.find_last_not_of('0') + 1);
    }
    RECT temp = { 530, 590, 594, 654 };
    InvalidateRect(wnd, &temp, true);
}

void moveRtile(int n)
{
    if (globals::curTile > 97)
        globals::curTile = 0;
    else globals::curTile++;
    curTileTxt->text = std::to_wstring(globals::curTile);
    if (globals::useDefault)
    {
        globals::solid = (globals::curTile > 10);
        globals::friction = (globals::curTile > 32 && globals::curTile < 44) ? 0.02f : 0.3f; //for slippery blocks
        globals::deadly = (globals::curTile > 21 && globals::curTile < 33) ? true : false;
        std::wstring fric = std::to_wstring(globals::friction);
        frictionTxt->text = L"Friction: " + fric.substr(0, fric.find_last_not_of('0') + 1);
    }
    RECT temp = { 530, 590, 594, 654 };
    InvalidateRect(wnd, &temp, true);
}

void xAddOff(int n)
{
    globals::xOffset += 1;
    xOffTxt->text = L"X Offset: " + std::to_wstring(globals::xOffset);
}

void yAddOff(int n)
{
    globals::yOffset += 1;
    yOffTxt->text = L"Y Offset: " + std::to_wstring(globals::yOffset);
}

void xSubOff(int n)
{
    globals::xOffset -= 1;
    xOffTxt->text = L"X Offset: " + std::to_wstring(globals::xOffset);
}

void ySubOff(int n)
{
    globals::yOffset -= 1;
    yOffTxt->text = L"Y Offset: " + std::to_wstring(globals::yOffset);
}

void resetOffsets(int n)
{
    globals::yOffset = 0;
    globals::xOffset = 0;
    globals::sID = 0;
    yOffTxt->text = L"Y Offset: 0";
    xOffTxt->text = L"X Offset: 0";
    sidTxt->text = L"SID: 0";
}

void Addsid(int n)
{
    globals::sID++;
    sidTxt->text = L"SID: " + std::to_wstring(globals::sID);
}

void Subsid(int n)
{
    globals::sID--;
    sidTxt->text = L"SID: " + std::to_wstring(globals::sID);
}

void Addfric(int n)
{
    globals::friction += (0.01f * n);
    std::wstring fric = std::to_wstring(globals::friction);
    frictionTxt->text = L"Friction: " + fric.substr(0, fric.find_last_not_of('0')+1);
}

void Subfric(int n)
{
    globals::friction -= (0.01f * n);
    std::wstring fric = std::to_wstring(globals::friction);
    frictionTxt->text = L"Friction: " + fric.substr(0, fric.find_last_not_of('0') + 1);
}

void buildScreenObjects()
{
    globals::tilesets.reserve(1);
    tileset->source = TSimg;
    tileset->sourceMask = TSmask;
    globals::tilesets.push_back(tileset);

    globals::bpIcons = new tileSet(BPimg, BPmask, 8, 8);

    newF(1, 1, false);

    globals::g_ScreenObjects.reserve(46);

    globals::g_ScreenObjects.push_back(mouseXtxt);
    globals::g_ScreenObjects.push_back(mouseYtxt);
    globals::g_ScreenObjects.push_back(mouseDowntxt);

    globals::g_ScreenObjects.push_back(timetxt);

    globals::g_ScreenObjects.push_back(modtxt1);
    globals::g_ScreenObjects.push_back(modtxt2);
    globals::g_ScreenObjects.push_back(modtxt3);

    globals::g_ScreenObjects.push_back(debugtxt);

    globals::g_ScreenObjects.push_back(fpstxt);
    globals::g_ScreenObjects.push_back(Afpstxt);

    globals::g_ScreenObjects.push_back(stageL);
    globals::g_ScreenObjects.push_back(stageR);
    globals::g_ScreenObjects.push_back(stageXn);

    globals::g_ScreenObjects.push_back(stageU);
    globals::g_ScreenObjects.push_back(stageD);
    globals::g_ScreenObjects.push_back(stageYn);

    globals::g_ScreenObjects.push_back(loopStage);
    globals::g_ScreenObjects.push_back(loopTxt);

    selFore->active = false;
    globals::g_ScreenObjects.push_back(selFore);
    globals::g_ScreenObjects.push_back(selBack);

    globals::g_ScreenObjects.push_back(showFore);
    globals::g_ScreenObjects.push_back(showBack);

    globals::g_ScreenObjects.push_back(curTileTxt);
    globals::g_ScreenObjects.push_back(rTile);
    globals::g_ScreenObjects.push_back(lTile);

    globals::g_ScreenObjects.push_back(xOffTxt);
    globals::g_ScreenObjects.push_back(yOffTxt);

    globals::g_ScreenObjects.push_back(xOffP);
    globals::g_ScreenObjects.push_back(yOffP);
    globals::g_ScreenObjects.push_back(xOffM);
    globals::g_ScreenObjects.push_back(yOffM);

    globals::g_ScreenObjects.push_back(resetOffsetsB);

    globals::g_ScreenObjects.push_back(useDefaultBtn);
    globals::g_ScreenObjects.push_back(useDefaultTxt);

    useSolidBtn->active = false;
    globals::g_ScreenObjects.push_back(useSolidBtn);
    globals::g_ScreenObjects.push_back(useSolidTxt);

    useKillerBtn->active = false;
    globals::g_ScreenObjects.push_back(useKillerBtn);
    globals::g_ScreenObjects.push_back(useKillerTxt);

    frictionP->active = false;
    frictionM->active = false;
    frictionPP->active = false;
    frictionMM->active = false;
    globals::g_ScreenObjects.push_back(frictionP);
    globals::g_ScreenObjects.push_back(frictionM);
    globals::g_ScreenObjects.push_back(frictionPP);
    globals::g_ScreenObjects.push_back(frictionMM);
    globals::g_ScreenObjects.push_back(frictionTxt);

    globals::g_ScreenObjects.push_back(sidP);
    globals::g_ScreenObjects.push_back(sidM);
    globals::g_ScreenObjects.push_back(sidTxt);
}

void freeMem()
{
    delete globals::bpIcons;
    
    if (!globals::g_level->uninit)delete globals::g_level;

    unsigned short tmp = unsigned short(globals::tilesets.size());
    for (unsigned short i = 0; i < tmp; i++)
    {
        delete globals::tilesets.at(i);
    }

    tmp = unsigned short(globals::g_ScreenObjects.size());
    for (unsigned short i = 0; i < tmp; i++)
    {
        delete globals::g_ScreenObjects.at(i);
    }
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

   TSimg = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TILESET1));
   TSmask = CreateBitmapMask(TSimg, RGB(0, 0, 1));
   BPimg = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BPSET));
   BPmask = CreateBitmapMask(BPimg, RGB(0, 0, 1));

   //StartFlag = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SFLAG));
   //StartFlagMask = CreateBitmapMask(StartFlag, RGB(0, 0, 1));

   buildScreenObjects();
   SetTimer(hWnd, 1, globals::refreshRate, NULL);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void load(LPWSTR file)
{

    if (globals::g_level->loadLevel(file))
    {
        globals::fileName = file;
        globals::curStageX = 0;
        globals::curStageY = 0;
        unoptimizeCur();
        stageXn->text = L"Stage X: " + std::to_wstring(globals::curStageX);
        stageYn->text = L"Stage Y: " + std::to_wstring(globals::curStageY);
        RECT temp = { 0, 0, 768, 576 };
        InvalidateRect(wnd, &temp, true);
    }
    else
    {

    }
}

HRESULT loadDBox()
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileOpen->Show(wnd);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
                        load(pszFilePath);
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    return 0;
}

void gridToggle()
{
    globals::grid = !globals::grid;
    CheckMenuItem(menu, ID_VIEW_GRID, (globals::grid) ? MF_CHECKED : MF_UNCHECKED);
    RECT temp = { 0, 0, 768, 576 };
    InvalidateRect(wnd, &temp, true);
}

void iconsToggle()
{
    globals::showIcons = !globals::showIcons;
    CheckMenuItem(menu, ID_VIEW_ICONS, (globals::showIcons) ? MF_CHECKED : MF_UNCHECKED);
    RECT temp = { 0, 0, 768, 576 };
    InvalidateRect(wnd, &temp, true);
}

void foregroundToggle()
{
    globals::showForeground = !globals::showForeground;
    CheckMenuItem(menu, ID_VIEW_SHOWFOREGROUND, (globals::showForeground) ? MF_CHECKED : MF_UNCHECKED);
    RECT temp = { 0, 0, 768, 576 };
    InvalidateRect(wnd, &temp, true);
}

void backgroundToggle()
{
    globals::showBackground = !globals::showBackground;
    CheckMenuItem(menu, ID_VIEW_SHOWBACKGROUND, (globals::showBackground) ? MF_CHECKED : MF_UNCHECKED);
    RECT temp = { 0, 0, 768, 576 };
    InvalidateRect(wnd, &temp, true);
}

void pressed(WPARAM key)
{
    switch (key)
    {
    case VK_LEFT:
        moveLstage(0);
        break;
    case VK_RIGHT:
        moveRstage(0);
        break;
    case VK_UP:
        moveUstage(0);
        break;
    case VK_DOWN:
        moveDstage(0);
        break;
    case 0x47:
        gridToggle();
        break;
    case 0x49:
        iconsToggle();
        break;
    case 0x46:
        foregroundToggle();
        break;
    case 0x42:
        backgroundToggle();
        break;
    case 0x53:
        if (globals::g_modKeys.at(VK_CONTROL)) save();
        break;
    case 0x57:
        globals::loop = !globals::loop;
        break;
    case 0x44:
        if (globals::g_modKeys.at(VK_SHIFT) && globals::g_modKeys.at(VK_TAB) && globals::g_modKeys.at(VK_CONTROL))
        {
            globals::g_debug = !globals::g_debug;
            debugtxt->text = L"Debug: " + std::to_wstring(globals::g_debug);
            RECT temp = { 0, 0, SWIDTH, SHEIGHT };
            InvalidateRect(wnd, &temp, true);
        }
        break;
    }
}

void place(unsigned short tileID)
{
    unsigned short stage = globals::curStageX + globals::curStageY * globals::g_level->lw;

    unsigned char cX = unsigned char(floor(globals::g_mouseX / 64));
    unsigned char cY = unsigned char(floor(globals::g_mouseY / 64));
    unsigned char cell = cX + cY * 12;

    if (cell != globals::prevCell || tileID != globals::prevTile)
    {
        globals::prevCell = cell;
        globals::prevTile = tileID;

        unsigned char tX = tileID % 11;
        unsigned char tY = unsigned char(floor(tileID / 11));

        std::vector<tile*>* lvlPtr = nullptr;
        if (globals::layer) lvlPtr = &globals::g_level->foreground[stage].data; //swag strats   
        else lvlPtr = &globals::g_level->background[stage].data;

        tile* tyle = new tile(globals::tilesets.at(globals::curTileSet - 1), tX, tY, globals::bpIcons);
        if (globals::useDefault)
        {
            tyle->solid = (tileID > 10);
            tyle->friction = (tileID > 32 && tileID < 44) ? 0.02f : 0.3f; //for slippery blocks
            tyle->killer = (tileID > 21 && tileID < 33) ? true : false;
        }
        else
        {
            tyle->solid = globals::solid;
            tyle->killer = globals::deadly;
            tyle->friction = globals::friction;
            tyle->specialID = globals::sID;
        }
        tyle->hitXOff = globals::xOffset;
        tyle->hitYOff = globals::yOffset;
        tyle->imgXOff = globals::xOffset;
        tyle->imgYOff = globals::yOffset;

        if (!tyle->isEqual(lvlPtr->at(cell))) //invalidating the rect is very very very cpu heavy, we want to avoid it whenever possible
        {

            delete lvlPtr->at(cell); //cool memory mangement strats
            lvlPtr->at(cell) = tyle;
            
            RECT temp = { 0, 0, 768, 576 };
            InvalidateRect(wnd, &temp, true);
        }
        else delete tyle;
    }
}

BOOL CALLBACK newProc(HWND hwndDlg,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_EDIT1:
        {
            int len = GetWindowTextLength(GetDlgItem(hwndDlg, IDC_EDIT1));
            if (len > 0)
            {
                LPTSTR buf = new TCHAR[len];
                GetDlgItemText(hwndDlg, IDC_EDIT1, buf, len + 1);
               
                int tmp = _ttoi(buf);
                if (tmp < 1 || tmp > 255)
                {
                    tlw = 1;
                }
                else
                {
                    tlw = tmp;
                }
                //delete[] buf;
            }
            break;
        }
        case IDC_EDIT2:
        {
            int len = GetWindowTextLength(GetDlgItem(hwndDlg, IDC_EDIT2));
            if (len > 0)
            {
                LPTSTR buf = new TCHAR[len];
                GetDlgItemText(hwndDlg, IDC_EDIT2, buf, len + 1);

                int tmp = _ttoi(buf);
                if (tmp < 1 || tmp > 255)
                {
                    tlh = 1;
                }
                else
                {
                    tlh = tmp;
                }
                //delete[] buf;
            }
            break;
        }
        case IDC_ENDX:
        {
            int len = GetWindowTextLength(GetDlgItem(hwndDlg, IDC_ENDX));
            if (len > 0)
            {
                LPTSTR buf = new TCHAR[len];
                GetDlgItemText(hwndDlg, IDC_ENDX, buf, len + 1);

                int tmp = _ttoi(buf);
                if (tmp < 0 || tmp >= tlw)
                {
                    tex = 0;
                }
                else
                {
                    tex = tmp;
                }
                //delete[] buf;
            }
            break;
        }
        case IDC_ENDY:
        {
            int len = GetWindowTextLength(GetDlgItem(hwndDlg, IDC_ENDY));
            if (len > 0)
            {
                LPTSTR buf = new TCHAR[len];
                GetDlgItemText(hwndDlg, IDC_ENDY, buf, len + 1);

                int tmp = _ttoi(buf);
                if (tmp < 0 || tmp >= tlh)
                {
                    tey = 0;
                }
                else
                {
                    tey = tmp;
                }
                //delete[] buf;
            }
            break;
        }
        case IDC_STARTCX:
        {
            int len = GetWindowTextLength(GetDlgItem(hwndDlg, IDC_STARTCX));
            if (len > 0)
            {
                LPTSTR buf = new TCHAR[len];
                GetDlgItemText(hwndDlg, IDC_STARTCX, buf, len + 1);

                int tmp = _ttoi(buf);
                if (tmp < 0 || tmp >= tlw)
                {
                    tscx = 0;
                }
                else
                {
                    tscx = tmp;
                }
                //delete[] buf;
            }
            break;
        }
        case IDC_STARTCY:
        {
            int len = GetWindowTextLength(GetDlgItem(hwndDlg, IDC_STARTCY));
            if (len > 0)
            {
                LPTSTR buf = new TCHAR[len];
                GetDlgItemText(hwndDlg, IDC_STARTCY, buf, len + 1);

                int tmp = _ttoi(buf);
                if (tmp < 0 || tmp >= tlh)
                {
                    tscy = 0;
                }
                else
                {
                    tscy = tmp;
                }
                //delete[] buf;
            }
            break;
        }
        case IDC_STARTX:
        {
            int len = GetWindowTextLength(GetDlgItem(hwndDlg, IDC_STARTX));
            if (len > 0)
            {
                LPTSTR buf = new TCHAR[len];
                GetDlgItemText(hwndDlg, IDC_STARTX, buf, len + 1);

                int tmp = _ttoi(buf);
                if (tmp < 0 || tmp >= 12)
                {
                    tsx = 0;
                }
                else
                {
                    tsx = tmp;
                }
                //delete[] buf;
            }
            break;
        }
        case IDC_STARTY:
        {
            int len = GetWindowTextLength(GetDlgItem(hwndDlg, IDC_STARTY));
            if (len > 0)
            {
                LPTSTR buf = new TCHAR[len];
                GetDlgItemText(hwndDlg, IDC_STARTY, buf, len + 1);

                int tmp = _ttoi(buf);
                if (tmp < 0 || tmp >= 9)
                {
                    tsy = 0;
                }
                else
                {
                    tsy = tmp;
                }
                //delete[] buf;
            }
            break;
        }
        case IDC_RADIOTOP:
            tes = 0;
            break;
        case IDC_RADIORIGHT:
            tes = 1;
            break;
        case IDC_RADIOBOTTOM:
            tes = 2;
            break;
        case IDC_RADIOLEFT:
            tes = 3;
            break;
        case IDC_CHECK1SWR:
            tswr = !tswr;
            break;
        case IDOK:
            
        case IDCANCEL:
            EndDialog(hwndDlg, wParam);
            return TRUE;
        }
    }
    return FALSE;
}


unsigned long long fpsTimer = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
unsigned int times = 0;
unsigned int Atimes = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    wnd = hWnd;
    menu = GetMenu(hWnd);
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case ID_FILE_NEW:
        {
            if (DialogBox(hInst, MAKEINTRESOURCE(IDD_NEWPOPUP), wnd, (DLGPROC)newProc) == IDOK)
            {
                newF(tlw, tlh);
            }
            break;
        }
        case ID_FILE_SAVE:
            save();
            break;
        case ID_FILE_SAVEAS:
            saveAs();
            break;
        case ID_FILE_LOAD:
            loadDBox();
            break;
        case ID_VIEW_GRID:
            gridToggle();
            break;
        case ID_VIEW_ICONS:
            iconsToggle();
            break;
        case ID_VIEW_SHOWFOREGROUND:
            foregroundToggle();
            break;
        case ID_VIEW_SHOWBACKGROUND:
            backgroundToggle();
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
        RECT temp;
        GetUpdateRect(hWnd, &temp, true);
        pnt(hWnd, &temp);
        Atimes++;
        break;
    case WM_ERASEBKGND:
        return TRUE;
    case WM_MOUSEWHEEL:
        if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) moveRtile(0);
        else moveLtile(0);
        break;
    case WM_MOUSEMOVE:
    {
        globals::g_mouseX = GET_X_LPARAM(lParam);
        globals::g_mouseY = GET_Y_LPARAM(lParam);
        if (globals::g_mouseDown) {
            if (globals::g_mouseX > 0 && globals::g_mouseX < 768)
            {
                if (globals::g_mouseY > 0 && globals::g_mouseY < 576)
                {
                    place(globals::curTile);
                }
            }
        }
        if (globals::g_debug)
        {
            mouseXtxt->text = L"Mouse X: " + std::to_wstring(globals::g_mouseX);
            mouseYtxt->text = L"Mouse Y: " + std::to_wstring(globals::g_mouseY);
        }
        break;
    }
    case WM_LBUTTONDOWN:
        globals::g_mouseDown = true;
        mouseDowntxt->text = L"Mouse Down: True";

        if (stageL->Touching(globals::g_mouseX, globals::g_mouseY)) {
            stageL->press(0);
            break;
        }

        if (stageR->Touching(globals::g_mouseX, globals::g_mouseY)) {
            stageR->press(0);
            break;
        }

        if (stageU->Touching(globals::g_mouseX, globals::g_mouseY)) {
            stageU->press(0);
            break;
        }

        if (stageD->Touching(globals::g_mouseX, globals::g_mouseY)) {
            stageD->press(0);
            break;
        }

        if (selFore->Touching(globals::g_mouseX, globals::g_mouseY)) {
            selFore->press(0);
            break;
        }

        if (selBack->Touching(globals::g_mouseX, globals::g_mouseY)) {
            selBack->press(0);
            break;
        }

        if (useDefaultBtn->Touching(globals::g_mouseX, globals::g_mouseY)) {
            useDefaultBtn->press(0);
            useKillerBtn->active = !globals::useDefault;
            useSolidBtn->active = !globals::useDefault;
            frictionP->active = !globals::useDefault;
            frictionM->active = !globals::useDefault;
            frictionPP->active = !globals::useDefault;
            frictionMM->active = !globals::useDefault;
            break;
        }

        if (showFore->Touching(globals::g_mouseX, globals::g_mouseY)) {
            showFore->press(0);
            CheckMenuItem(menu, ID_VIEW_SHOWFOREGROUND, (globals::showForeground) ? MF_CHECKED : MF_UNCHECKED);
            RECT temp = { 0, 0, 768, 576 };
            InvalidateRect(wnd, &temp, true);
            break;
        }

        if (showBack->Touching(globals::g_mouseX, globals::g_mouseY)) {
            showBack->press(0);
            CheckMenuItem(menu, ID_VIEW_SHOWBACKGROUND, (globals::showBackground) ? MF_CHECKED : MF_UNCHECKED);
            RECT temp = { 0, 0, 768, 576 };
            InvalidateRect(wnd, &temp, true);
            break;
        }

        if (loopStage->Touching(globals::g_mouseX, globals::g_mouseY)) {
            loopStage->press(0);
            break;
        }

        if (rTile->Touching(globals::g_mouseX, globals::g_mouseY)) {
            rTile->press(0);
            break;
        }

        if (lTile->Touching(globals::g_mouseX, globals::g_mouseY)) {
            lTile->press(0);
            break;
        }

        if (xOffP->Touching(globals::g_mouseX, globals::g_mouseY)) {
            xOffP->press(0);
            break;
        }

        if (yOffP->Touching(globals::g_mouseX, globals::g_mouseY)) {
            yOffP->press(0);
            break;
        }

        if (xOffM->Touching(globals::g_mouseX, globals::g_mouseY)) {
            xOffM->press(0);
            break;
        }

        if (yOffM->Touching(globals::g_mouseX, globals::g_mouseY)) {
            yOffM->press(0);
            break;
        }

        if (resetOffsetsB->Touching(globals::g_mouseX, globals::g_mouseY)) {
            resetOffsetsB->press(0);
            break;
        }

        if (useSolidBtn->Touching(globals::g_mouseX, globals::g_mouseY)) {
            useSolidBtn->press(0);
            break;
        }

        if (useKillerBtn->Touching(globals::g_mouseX, globals::g_mouseY)) {
            useKillerBtn->press(0);
            break;
        }

        if (frictionP->Touching(globals::g_mouseX, globals::g_mouseY)) {
            frictionP->press(1);
            break;
        }

        if (frictionM->Touching(globals::g_mouseX, globals::g_mouseY)) {
            frictionM->press(1);
            break;
        }

        if (frictionPP->Touching(globals::g_mouseX, globals::g_mouseY)) {
            frictionPP->press(10);
            break;
        }

        if (frictionMM->Touching(globals::g_mouseX, globals::g_mouseY)) {
            frictionMM->press(10);
            break;
        }

        if (globals::g_mouseX > 0 && globals::g_mouseX < 768)
        {
            if (globals::g_mouseY > 0 && globals::g_mouseY < 576)
            {
                place(globals::curTile);
                break;
            }
        }
        break;
    case WM_LBUTTONUP:
        globals::g_mouseDown = false;
        mouseDowntxt->text = L"Mouse Down: False";
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
        if (globals::g_debug)
        {
            modtxt1->text = L"Shifting: " + std::to_wstring(globals::g_modKeys.at(VK_SHIFT));
            modtxt2->text = L"Control: " + std::to_wstring(globals::g_modKeys.at(VK_CONTROL));
            modtxt3->text = L"Tab: " + std::to_wstring(globals::g_modKeys.at(VK_TAB));
        }
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
        }
        if (globals::g_debug)
        {
            modtxt1->text = L"Shifting: " + std::to_wstring(globals::g_modKeys.at(VK_SHIFT));
            modtxt2->text = L"Control: " + std::to_wstring(globals::g_modKeys.at(VK_CONTROL));
            modtxt3->text = L"Tab: " + std::to_wstring(globals::g_modKeys.at(VK_TAB));
        }
        break;
    case WM_TIMER:
        switch (wParam)
        {
        case 1:
            globals::timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            if (globals::g_debug) timetxt->text = L"Time Now: " + std::to_wstring(globals::timeNow);
            if (globals::timeNow - fpsTimer >= 1000)
            {
                fpsTimer += 1000;
                globals::g_fps = times;
                if (globals::g_debug)
                {
                    fpstxt->text = L"FPS: " + std::to_wstring(globals::g_fps);
                    Afpstxt->text = L"AFPS: " + std::to_wstring(Atimes);
                }
                times = 1;
                Atimes = 0;
            }
            else {
                times++;
            }
            if (globals::g_debug) InvalidateRect(hWnd, &createRECT(0, 0, 200, 190), true);
            RECT temp = { 5, 580, 770, 720 }; //bottom bar
            InvalidateRect(wnd, &temp, true);
            break;
        }
        break;
    case WM_DESTROY:
        save(true); //saves a backup of current level
        freeMem();
        PostQuitMessage(0);
        exit(0); //for whatever reason the open/save dialog boxes make this necessary.
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}