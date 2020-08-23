#include <windows.h>
#include <string>
#include <fstream>
#include <algorithm>

struct screenObject
{
	RECT rect = RECT();
	COLORREF color = RGB(255,255,255);
	std::string text;
	BOOLEAN active = true;
	BOOLEAN visible = true;

	int txtFlag = 1;

	bool debug = false;

	screenObject(RECT rect, COLORREF color, std::string txt, int txtFlag = 1, bool debug = false): 
		rect(rect), color(color), text(txt), txtFlag(txtFlag), debug(debug) {}

	BOOLEAN Touching(long mx, long my)
	{
		RECT leEpicRekt = this->rect;
		if (mx >= leEpicRekt.left && mx <= leEpicRekt.right) //check x
		{
			if (my >= leEpicRekt.top && my <= leEpicRekt.bottom) //check y
			{
				return true;
			}
		}
		return false;
	}

	virtual void press(int n) = 0;

	virtual void hide() = 0;

	virtual void show() = 0;

	virtual COLORREF draw(HDC * hdc, long mousX = 0, long mouseY = 0, bool mouseDown = false) = 0;
};

struct Button: public screenObject
{
	void(*onPress)(int);

	Button(RECT rect, COLORREF color, void(*function)(int), std::string txt = "", int txtFlag = 1, bool debug = false) :
		screenObject(rect, color, txt, txtFlag, debug), onPress(function) {}
	
	void screenObject::press(int n)
	{
		(this->onPress)(n);
	}

	void screenObject::hide()
	{
		this->active = false;
		this->visible = false;
	}

	void screenObject::show()
	{
		this->active = true;
		this->visible = true;
	}

	COLORREF screenObject::draw(HDC* hdc, long mousX, long mouseY, bool mouseDown)
	{
		COLORREF tempcolor = this->color;
		HBRUSH brush = NULL;
		if (this->active)
		{
			if (this->Touching(mousX, mouseY))
			{
				if (mouseDown)
				{
					tempcolor = RGB(GetRValue(this->color) - 20, GetGValue(this->color) - 20, GetBValue(this->color) - 20);
					brush = CreateSolidBrush(tempcolor);
					FillRect(*hdc, &this->rect, brush);
				}
				else
				{
					tempcolor = RGB(GetRValue(this->color) + 15, GetGValue(this->color) + 15, GetBValue(this->color) + 15);
					brush = CreateSolidBrush(tempcolor);
					FillRect(*hdc, &this->rect, brush);
				}
			}
			else
			{
				brush = CreateSolidBrush(this->color);
				FillRect(*hdc, &this->rect, brush);
			}
		}
		else
		{
			tempcolor = RGB(GetRValue(this->color) - 25, GetGValue(this->color) - 25, GetBValue(this->color) - 25);
			brush = CreateSolidBrush(tempcolor);
			FillRect(*hdc, &this->rect, brush);
		}
		DeleteObject(brush);
		return tempcolor;
	}
};

struct TextBox : public screenObject
{
	BOOLEAN inputting = false;

	TextBox(RECT rect, COLORREF color, std::string txt = "", int txtFlag = DT_LEFT, bool debug = false) :
		screenObject(rect, color, txt, txtFlag, debug) {}

	void screenObject::press(int n = 0)
	{
		this->inputting = true;
	}

	void screenObject::hide()
	{
		this->active = false;
		this->visible = false;
		this->inputting = false;
	}

	void screenObject::show()
	{
		this->active = true;
		this->visible = true;
	}

	COLORREF screenObject::draw(HDC* hdc, long mousX, long mouseY, bool mouseDown)
	{
		COLORREF tempcolor = this->color;
		HBRUSH brush = NULL;
		if (this->active)
		{
			if (this->Touching(mousX, mouseY))
			{
				if (mouseDown)
				{
					tempcolor = RGB(GetRValue(this->color) - 20, GetGValue(this->color) - 20, GetBValue(this->color) - 20);
					brush = CreateSolidBrush(tempcolor);
					FillRect(*hdc, &this->rect, brush);
				}
				else
				{
					tempcolor = RGB(GetRValue(this->color) + 15, GetGValue(this->color) + 15, GetBValue(this->color) + 15);
					brush = CreateSolidBrush(tempcolor);
					FillRect(*hdc, &this->rect, brush);
				}
			}
			else
			{
				brush = CreateSolidBrush(this->color);
				FillRect(*hdc, &this->rect, brush);
			}
		}
		else
		{
			tempcolor = RGB(GetRValue(this->color) - 25, GetGValue(this->color) - 25, GetBValue(this->color) - 25);
			brush = CreateSolidBrush(tempcolor);
			FillRect(*hdc, &this->rect, brush);
		}
		DeleteObject(brush);
		return tempcolor;
	}
};

struct Text: public screenObject 
{
	Text(RECT rect, COLORREF color, std::string txt, int txtFlag = 1, bool debug = false) :
		screenObject(rect, color, txt, txtFlag, debug) {}

	void screenObject::press(int n = 0)
	{
	}

	void screenObject::hide()
	{
		this->active = false;
		this->visible = false;
	}

	void screenObject::show()
	{
		this->active = true;
		this->visible = true;
	}

	COLORREF screenObject::draw(HDC* hdc, long mousX, long mouseY, bool mouseDown)
	{
		return this->color;
	}
};

struct BImage: public screenObject 
{
	HBITMAP image = NULL;

	BImage(RECT rect, HBITMAP img, std::string txt = "", int txtFlag = 1, bool debug = false) :
		screenObject(rect, RGB(0, 0, 0), txt, txtFlag, debug), image(img) {}

	void screenObject::press(int n = 0)
	{
	}

	void screenObject::hide()
	{
		this->active = false;
		this->visible = false;
	}

	void screenObject::show()
	{
		this->active = true;
		this->visible = true;
	}

	COLORREF screenObject::draw(HDC* hdc, long mousX, long mouseY, bool mouseDown)
	{
		BITMAP bm;
		HDC hdcMem = CreateCompatibleDC(*hdc);
		HGDIOBJ hbmOld = SelectObject(hdcMem, image);
		GetObject(image, sizeof(bm), &bm);
		BitBlt(*hdc, rect.left, rect.top, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
		DeleteObject(hbmOld);
		return this->color;
	}

	~BImage()
	{
		DeleteObject(image);
	}
};

struct Image : public screenObject
{
	HANDLE image = NULL;

	Image(RECT rect, HANDLE img, std::string txt = "", int txtFlag = 1, bool debug = false) :
		screenObject(rect, RGB(0, 0, 0), txt, txtFlag, debug), image(img) {}

	void screenObject::press(int n = 0)
	{
	}

	void screenObject::hide()
	{
		this->active = false;
		this->visible = false;
	}

	void screenObject::show()
	{
		this->active = true;
		this->visible = true;
	}

	COLORREF screenObject::draw(HDC* hdc, long mousX, long mouseY, bool mouseDown)
	{
		BITMAP bm;
		HDC hdcMem = CreateCompatibleDC(*hdc);
		HGDIOBJ hbmOld = SelectObject(hdcMem, image);
		GetObject(image, sizeof(bm), &bm);
		BitBlt(*hdc, rect.left, rect.top, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
		DeleteObject(hbmOld);
		return this->color;
	}

	~Image()
	{
		DeleteObject(image);
	}
};

struct tileSet
{
	HANDLE source;
	unsigned int tw;
	unsigned int th;

	tileSet(HANDLE img, unsigned int tw, unsigned int th) :
	source(img), tw(tw), th(th){}

	void draw(HDC* hdc, int x, int y, unsigned int tx, unsigned int ty)
	{
		BITMAP bm;
		HDC hdcMem = CreateCompatibleDC(*hdc);
		HGDIOBJ hbmOld = SelectObject(hdcMem, source);
		GetObject(source, sizeof(bm), &bm);
		BitBlt(*hdc, x, y, tw, th, hdcMem, tx * tw, ty * th, SRCCOPY);
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
		DeleteObject(hbmOld);
	}
};

struct tile
{
	tileSet* tiles;
	unsigned int tx;
	unsigned int ty;
	
	int imgXOff = 0;
	int imgYOff = 0;
	int hitXOff = 0;
	int hitYOff = 0;

	unsigned short specialID = 0;

	bool solid;
	float friction = 0.0f;
	bool killer = false;

	tile()
	{
		tiles = nullptr;
		tx = 0;
		ty = 0;
		solid = false;
	}

	tile(tileSet* tileS, unsigned int tx, unsigned int ty, bool solid = true, float friction = 0.1f, bool killer = false) :
		tiles(tileS), tx(tx), ty(ty), solid(solid), friction(friction), killer(killer) {}

	void draw(HDC * hdc, int x, int y)
	{
		tiles->draw(hdc, x+this->imgXOff, y+this->imgYOff, this->tx, this->ty);
	}
};

struct stage
{
	std::vector<tile*> data; //12:9 of 64x64

	bool exists = false;

	stage()
	{
		exists = false;
	}

	stage(std::string data, std::vector<tileSet*>* tileSets, unsigned int tilesetID)
	{
		loadStage(data, tileSets, tilesetID);
	}

	void loadStage(std::string data, std::vector<tileSet*>* tileSets, unsigned int tilesetID)
	{
		std::string temp = "";
		this->data.reserve(108);
		for (unsigned int i = 0; i < 108; i++)
		{
			temp = "";
			temp += data.at(i * 2);
			temp += data.at((i * 2) + 1);
			unsigned int tileID = std::stoi(temp);
			bool solid = (tileID > 10);
			float friction = (tileID > 32 && tileID < 44) ? 0.02f : 0.3f; //for slippery blocks
			bool killer = (tileID > 21 && tileID < 33) ? true : false;
			this->data.emplace_back(new tile(tileSets->at(tilesetID-1), tileID%11, (unsigned int)floor(tileID/11), solid, friction, killer));
		}
		exists = true;
	}

	void draw(HDC* hdc)
	{
		if (exists)
		{
			for (unsigned int i = 0; i < data.size(); i++)
			{
				data.at(i)->draw(hdc, (i % 12) * 64, (int)floor(i / 12) * 64);
			}
		}
	}
	
	~stage()
	{
		size_t size = this->data.size();
		for (unsigned int i = 0; i < size; i++)
		{
			if (this->data.size() > 0) delete this->data.at(i);
		}
		this->data.clear();
	}
};

struct level : public screenObject
{
	stage* foreground = nullptr; //THESE ARE POINTERS TO ARRAYS
	stage* background = nullptr;
	unsigned int lw = 0; //width of level (in stages)
	unsigned int lh = 0; //height of level (in stages)
	unsigned int startCX;
	unsigned int startCY;
	unsigned int startX;
	unsigned int startY;
	unsigned int endCX;
	unsigned int endCY;
	unsigned int endS;
	bool startRune = false;

	bool uninit = true;

	const std::map<char, int> sIDs = { {'@',0}, {'#',1}, {'=',2}, {'|',3}, {'M',4}, {'B', 5}, {'-',6} };
	std::vector<tileSet*>* tileSets;
	
	level(std::string file, std::vector<tileSet*>* tileSet):
	screenObject(rect, RGB(0,0,0), ""), tileSets(tileSet)
	{
		if (!loadLevel(file))
			delete this;
	}

	bool loadLevel(std::string file)
	{
		if(lw != 0 && lh != 0)
		{
			delete[] foreground;
			delete[] background;
			this->uninit = true;
		}
		std::fstream lvlDat;
		lvlDat.open(file, std::ios::in);
		if (lvlDat.is_open())
		{
			std::string dat;
			size_t start;
			size_t end;

			unsigned int stag;
			unsigned int tilesetID;
			std::string temp = "";
			while (std::getline(lvlDat, dat)) {
				switch (sIDs.at(dat.at(0)))
				{
				case 0: //Header
					end = 0;
					for (size_t i = 0; i < 10; i++)
					{
						temp = "";
						start = end + 1;
						end = dat.find_first_of('/', start);
						for (size_t i = start; i < end; i++)
						{
							temp += dat.at(i);
						}
						switch (i)
						{
						case 0:
							lw = std::stoi(temp);
							break;
						case 1:
							lh = std::stoi(temp);
							break;
						case 2:
							startCX = std::stoi(temp);
							break;
						case 3:
							startCY = std::stoi(temp);
							break;
						case 4:
							startX = std::stoi(temp);
							break;
						case 5:
							startY = std::stoi(temp);
							break;
						case 6:
							endCX = std::stoi(temp);
							break;
						case 7:
							endCY = std::stoi(temp);
							break;
						case 8:
							endS = std::stoi(temp);
							break;
						case 9:
							startRune = (temp.at(0) == 'T')?true : false;
							break;
						}
					}
					break;
				case 1: //stageHeader
					temp = "";
					start = dat.find_first_of(':') + 1;
					end = dat.find_first_of('[', start);
					for (size_t i = start; i < end; i++)
					{
						temp += dat.at(i);
					}
					stag = std::stoi(temp);

					temp = "";
					start = dat.find_first_of(':', end) + 1;
					end = dat.find_first_of(']', start);
					for (size_t i = start; i < end; i++)
					{
						temp += dat.at(i);
					}
					tilesetID = std::stoi(temp);
					break;
				case 2: //StartstageData
					temp = "";
					if (dat.at(0) == 'N')
						foreground[stag] = stage();
					else temp += dat.substr(1, 24);
					break;
				case 3: //stageData
					temp += dat.substr(1, 24);
					if (dat.at(25) == ';')
						foreground[stag].loadStage(temp, tileSets, tilesetID);
					break;
				case 4: //tileMods
					unsigned int tileModindex;
					temp = dat.substr(1, dat.find_first_of('(')-1);
					tileModindex = std::stoi(temp);
					end = dat.find_first_of(')');
					temp = dat.substr(dat.find_first_of('(')+1,end- (dat.find_first_of('(') + 1));
					switch (temp.at(0))
					{
					case 's':
						foreground[stag].data.at(tileModindex)->solid = (temp.at(2) == 'T');
						break;
					case 'k':
						foreground[stag].data.at(tileModindex)->killer = (temp.at(2) == 'T');
						break;
					case 'x':
						foreground[stag].data.at(tileModindex)->hitXOff = std::stoi(temp.substr(2));
						foreground[stag].data.at(tileModindex)->imgXOff = std::stoi(temp.substr(2));
						break;
					case 'y':
						foreground[stag].data.at(tileModindex)->hitYOff = std::stoi(temp.substr(2));
						foreground[stag].data.at(tileModindex)->imgYOff = std::stoi(temp.substr(2));
						break;
					case 'X':
						foreground[stag].data.at(tileModindex)->hitXOff = std::stoi(temp.substr(2));
						break;
					case 'Y':
						foreground[stag].data.at(tileModindex)->hitYOff = std::stoi(temp.substr(2));
						break;
					case 'S':
						foreground[stag].data.at(tileModindex)->specialID = std::stoi(temp.substr(2));
						break;
					case 'f':
						foreground[stag].data.at(tileModindex)->friction = std::stof(temp.substr(2));
						break;
					}
				
					break;
				case 5: //bgstart
					temp = "";
					if (dat.at(0) == 'N')
						background[stag] = stage();
					else temp += dat.substr(1, 24);
					break;
				case 6: //bgdata
					temp += dat.substr(1, 24);
					if (dat.at(25) == ';')
						background[stag].loadStage(temp, tileSets, tilesetID);
					break;
				default:
					return false;
				}
				if (uninit) 
				{
					uninit = false;
					stage* fGrnd = new stage[lw * lh];
					stage* bGrnd = new stage[lw * lh];
					this->foreground = fGrnd;
					this->background = bGrnd;
				}
			}
			lvlDat.close();
			return true;
		}
		else return false;
	}

	void screenObject::press(int n = 0)
	{
	}

	void screenObject::hide()
	{
		this->active = false;
		this->visible = false;
	}

	void screenObject::show()
	{
		this->active = true;
		this->visible = true;
	}

	COLORREF screenObject::draw(HDC* hdc, long playerCX, long playerCY, bool mouseDown)
	{
		background[(playerCX)+(playerCY * lw)].draw(hdc);
		foreground[(playerCX)+(playerCY*lw)].draw(hdc);
		return this->color;
	}

	~level()
	{
		delete[] foreground;
		delete[] background;
	}
};

struct player : screenObject
{
	unsigned int Cx;
	unsigned int Cy;

	HANDLE sprite;

	level* lvlptr = nullptr;
	HWND* wnd = nullptr;

	std::map<WPARAM, bool> movementKeys = { {0x41, false}, {0x44, false}, {0x53, false}, {0x57, false}, {VK_SPACE, false} }; //A D S W SPACE

	float xvel = 0;
	float yvel = 0;

	float xaccel = 1.2f;

	const float maxXVel = 6;
	const float maxYVel = 6;

	bool grounded = true;
	float curFriction = 0.0f;

	bool canMousebox = false;
	bool mousebox = false;

	int lvl = 2;

	unsigned int lvlStartX = 1;
	unsigned int lvlStartY = 1;
	unsigned int lvlStartCX = 1;
	unsigned int lvlStartCY = 1;

	player(RECT xy, HANDLE sprite, level* lvl, HWND* wnd, std::string loadDat = "") :
		screenObject(xy, RGB(255, 255, 255), ""), sprite(sprite), lvlptr(lvl), wnd(wnd)
	{
		if (loadDat != "") loadPlayer(loadDat);
	}

	void loadPlayer(std::string data)
	{
	}

	void moveX(float x)
	{
		this->rect.left += long(x);
		this->rect.right += long(x + 64);
	}

	void moveY(float y)
	{
		this->rect.top += long(y);
		this->rect.bottom += long(y + 64);

	}

	void setX(float x)
	{
		this->rect.left = long(x);
		this->rect.right = long(x + 64);
	}

	void setY(float y)
	{
		this->rect.top = long(y);
		this->rect.bottom = long(y + 64);
	}

	void kill()
	{
		RECT temp = { 0, 0, 784, 615 };
		InvalidateRect(*this->wnd, &temp, true);
		setX(float(this->lvlStartX));
		setY(float(this->lvlStartY));
		this->Cx = lvlStartCX;
		this->Cy = lvlStartCY;
		this->xvel = 0;
		this->yvel = 0;
	}

	bool touching(float x, float x2, float wid = 64)
	{
		return((x >= x2) && (x <= (x2 + wid)));
	}

	bool touchingE(float x, float x2, float wid = 64)
	{
		return x >= x2 && x < x2 + wid;
	}

	void specialCollide(tile* curTile)
	{
		RECT temp = { 0, 0, 784, 615 };
		InvalidateRect(*this->wnd, &temp, true);
		switch (curTile->tx)
		{
		case 0:
			this->canMousebox = true;
			curTile->tx = 2;
			curTile->ty = 0;
			curTile->solid = false;
			break;
		case 1:
			this->lvl += 1;
			curTile->tx = 1;
			curTile->ty = 0;
			curTile->solid = false;
			break;
		case 2:
			curTile->tx = 4;
			curTile->ty = 1;
			curTile->hitYOff += 22;
			int txoff = curTile->imgXOff;
			int tyoff = curTile->imgYOff;
			for (unsigned int j = 0; j < this->lvlptr->lw * this->lvlptr->lh; j++)
			{
				stage* stage = &(this->lvlptr->foreground[j]);
				for (unsigned short i = 0; i < 108; i++)
				{
					if (stage->data.at(i)->specialID == curTile->specialID)
					{
						stage->data.at(i)->imgXOff = 800;
						stage->data.at(i)->imgYOff = 800;
						stage->data.at(i)->solid = false;
					}
				}
			}
			curTile->imgXOff = txoff;
			curTile->imgYOff = tyoff;
			curTile->solid = true;
			break;
		}
	}

	bool collideX(stage* stage)
	{
		float expectedX = this->rect.left + this->xvel;
		unsigned char Cy = (this->rect.top < 0) ? 0 : unsigned char(round(this->rect.top / 64));
		unsigned char start = (Cy > 0) ? ((Cy - 1) * 12): 0;
		unsigned char end = (Cy < 8) ? ((Cy + 2) * 12): 108;
		for (unsigned char i = start; i < end; i++)
		{
			tile* curTile = stage->data.at(i);
			float tileX = (float)((i % 12 * 64) + curTile->hitXOff);
			float tileY = (float)((floor(i / 12) * 64) + curTile->hitYOff);
			if (curTile->solid) {
				bool res1 = touching(expectedX, tileX);
				bool res2 = touching(expectedX + 64, tileX);
				if (res1 || res2)
				{
					if ((this->rect.top >= tileY && this->rect.top < tileY + 64) || (this->rect.top + 64 > tileY&& this->rect.top + 64 < tileY + 64))
					{
						if (!curTile->killer) {
							if (res1) this->setX(tileX + 64);
							if (res2) this->setX(tileX - 64);
						}
						else this->kill();

						if (curTile->ty == 4)
						{
							specialCollide(curTile);
						}

						return true;
					}
				}
			}
		}
		return false;
	}

	bool collideMouseX(float mx, float my, bool correct = true)
	{
		float expectedX = this->rect.left + this->xvel;
		bool res1 = touching(expectedX, mx, 64);
		bool res2 = touching(expectedX + 64, mx, 64);
		if (res1 || res2)
		{
			if ((this->rect.top >= my && this->rect.top < my + 64) || (this->rect.top + 64 > my&& this->rect.top + 64 < my + 64))
			{
				if (res1 && correct) this->setX(mx + 64);
				if (res2 && correct) this->setX(mx - 64);

				return true;
			}
		}
		return false;
	}

	bool collideY(stage* stage)
	{
		float expectedY = this->rect.top + this->yvel;
		unsigned char Cy = (expectedY < 0) ? 0 : unsigned char(round(expectedY / 64));
		unsigned char start = (Cy > 0) ? ((Cy - 1) * 12) : 0;
		unsigned char end = (Cy < 8) ? ((Cy + 2) * 12) : 108;
		for (unsigned char i = start; i < end; i++)
		{
			tile* curTile = stage->data.at(i);
			float tileX = (float)((i % 12 * 64) + curTile->hitXOff);
			float tileY = (float)((floor(i / 12) * 64) + curTile->hitYOff);
			if (curTile->solid) {
				bool res1 = touching(expectedY, tileY);
				bool res2 = touching(expectedY + 64, tileY);
				if (res1 || res2)
				{
					if ((this->rect.left >= tileX && this->rect.left < tileX + 64) || (this->rect.left + 64 > tileX&& this->rect.left + 64 < tileX + 64))
					{
						if (!curTile->killer)
						{
							if (res1)
							{
								this->setY(tileY + 64);
								this->yvel = 0;
							}

							if (res2)
							{
								this->setY(tileY - 64);
								this->grounded = true;
								this->xaccel = 1.2f;
								this->curFriction = curTile->friction;
								this->yvel = 0;
							}

							if (curTile->ty == 4)
							{
								specialCollide(curTile);
							}
						}
						else this->kill();
						return true;
					}
				}
			}
		}
		return false;
	}

	bool collideMouseY(float mx, float my, bool correct = true)
	{
		float expectedY = this->rect.top + this->yvel;
		bool res1 = touching(expectedY, my, 64);
		bool res2 = touching(expectedY + 64, my, 64);
		if (res1 || res2)
		{
			if ((this->rect.left >= mx && this->rect.left < mx + 64) || (this->rect.left + 64 > mx&& this->rect.left + 64 < mx + 64))
			{
				if (res1 && correct)
				{
					this->setY(my + 64);
					this->yvel = 0;
				}

				if (res2 && correct)
				{
					this->setY(my - 64);
					this->grounded = true;
					this->xaccel = 1.2f;
					this->curFriction = 0.5f;
					this->yvel = 0;
				}
				return true;
			}
		}
		return false;
	}

	void movePlayer(stage* stage, float mx, float my, bool shifting)
	{
		this->mousebox = shifting;
		if (this->movementKeys.at(0x41) && !this->movementKeys.at(0x44))
		{
			if (this->xvel > -this->maxXVel)
				this->xvel -= this->xaccel;
		}
		else if (this->movementKeys.at(0x44) && !this->movementKeys.at(0x41))
		{
			if (this->xvel < this->maxXVel)
				this->xvel += this->xaccel;
		}
		else if (this->xvel != 0.0f)
		{
			this->xvel = (this->xvel < 0.0f) ? this->xvel + curFriction : this->xvel - curFriction;
		}

		if (this->xvel > -0.001f && this->xvel < 0.001f)
		{
			this->xvel = 0.0f;
		}

		if (collideX(stage))
			this->xvel = 0.0f;
		else moveX(this->xvel);

		if (this->mousebox && this->canMousebox)
		{
			if (collideMouseX(mx, my))
				this->xvel = 0.0f;
		}

		if (this->grounded)
		{
			if (this->movementKeys.at(0x57) || this->movementKeys.at(VK_SPACE))
			{
				this->yvel -= 13.0f;
				this->grounded = false;
				this->xaccel = 0.25;
				this->curFriction = 0.1f; //friction due to air
			}
		}

		if (this->movementKeys.at(0x53) && (this->rect.left > -60 && this->rect.left < 766))
		{
			this->yvel += 0.5f;
		}

		this->yvel += 0.5f;

		if (collideY(stage))
			this->yvel = 0.0f;
		else moveY(this->yvel);

		if (this->mousebox && this->canMousebox)
		{
			if (collideMouseY(mx, my))
				this->yvel = 0.0f;
		}
	}


	void screenObject::press(int n = 0)
	{
	}

	void screenObject::hide()
	{
		this->active = false;
		this->visible = false;
	}

	void screenObject::show()
	{
		this->active = true;
		this->visible = true;
	}

	COLORREF screenObject::draw(HDC* hdc, long mousX, long mouseY, bool mouseDown)
	{
		BITMAP bm;
		HDC hdcMem = CreateCompatibleDC(*hdc);
		HGDIOBJ hbmOld = SelectObject(hdcMem, sprite);
		GetObject(sprite, sizeof(bm), &bm);
		BitBlt(*hdc, rect.left, rect.top, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
		DeleteObject(hbmOld);
		return this->color;
	}
};