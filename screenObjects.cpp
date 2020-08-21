#include <windows.h>
#include <string>
#include <fstream>

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

struct player : screenObject
{
	int Cx;
	int Cy;

	HANDLE sprite;

	std::map<WPARAM, bool> movementKeys = { {0x41, false}, {0x44, false}, {0x53, false}, {0x57, false}, {VK_SPACE, false} }; //A D S W SPACE

	float xvel = 0;
	float yvel = 0;

	const float maxXVel = 5;
	const float maxYVel = 5;

	bool grounded = true;

	unsigned int lvl = 1;

	player(RECT xy, HANDLE sprite, std::string loadDat = "", unsigned int lvl = 1) :
		screenObject(xy, RGB(255,255,255), ""), sprite(sprite), lvl(lvl) 
	{
		if (loadDat != "") loadPlayer(loadDat);
	}

	void loadPlayer(std::string data)
	{
	}

	void moveX(int x)
	{
		this->rect.left += x;
		this->rect.right += x + 64;
	}

	void moveY(int y)
	{
		this->rect.top += y;
		this->rect.bottom += y + 64;
	}

	void setX(int x)
	{
		this->rect.left = x;
		this->rect.right = x + 64;
	}

	void setY(int y)
	{
		this->rect.top = y;
		this->rect.bottom = y + 64;
	}

	bool touching(float x, float x2, float wid=64)
	{
		return x > x2 && x < x2 + wid;
	}

	bool collideX(stage* stage)
	{
		float expectedX = this->rect.left + this->xvel;
		unsigned int tileID = 0;
		for (unsigned int i = 0; i < 108; i++)
		{
			tile* curTile = stage->data.at(i);
			tileID = (curTile->tx) + (curTile->ty*11);
			float tileX = (float)(i%12 * 64);
			float tileY = (float)(floor(i / 12) * 64);
			switch(tileID)
			{
			case 13:
				if (touching(expectedX, tileX) || touching(expectedX+64, tileX))
				{
					if (touching(this->rect.top, tileY) || touching(this->rect.top + 64, tileY))
					{
						return true;
					}
				}
				break;
			default:
				break;
			}
		}
		return false;
	}

	bool collideY(stage* stage)
	{

	}

	void movePlayer(stage* stage)
	{

		if (this->movementKeys.at(0x41) && !this->movementKeys.at(0x44))
		{
			if(this->xvel > -this->maxXVel)
				this->xvel -= 1;
		}
		else if (this->movementKeys.at(0x44) && !this->movementKeys.at(0x41))
		{
			if (this->xvel < this->maxXVel)
				this->xvel += 1;
		}

		if (collideX(stage))
			this->xvel = 0;
		else moveX(this->xvel);
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
	bool solid;

	tile()
	{
		tiles = nullptr;
		tx = 0;
		ty = 0;
		solid = false;
	}

	tile(tileSet* tileS, unsigned int tx, unsigned int ty, bool solid = true) :
		tiles(tileS), tx(tx), ty(ty), solid(solid) {}

	void draw(HDC * hdc, int x, int y)
	{
		tiles->draw(hdc, x, y, tx, ty);
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
			this->data.emplace_back(new tile(tileSets->at(tilesetID-1), tileID%11, (unsigned int)floor(tileID/11)));
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
		size_t size = data.size();
		for (unsigned int i = 0; i < size; i++)
		{
			delete data.at(i);
		}
		data.clear();
	}
};

struct level : public screenObject
{
	stage* foreground = nullptr; //THESE ARE POINTERS TO ARRAYS
	stage* background = nullptr;
	unsigned int lw; //width of level (in stages)
	unsigned int lh; //height of level (in stages)
	unsigned int startCX;
	unsigned int startCY;
	unsigned int startX;
	unsigned int startY;
	unsigned int endCX;
	unsigned int endCY;
	unsigned int endS;

	bool uninit = true;

	const std::map<char, int> sIDs = { {'@',0}, {'#',1}, {'=',2}, {'|',3} };
	std::vector<tileSet*>* tileSets;
	
	level(std::string file, std::vector<tileSet*>* tileSet):
	screenObject(rect, RGB(0,0,0), ""), tileSets(tileSet)
	{
		if (!loadLevel(file))
			delete this;
	}

	bool loadLevel(std::string file)
	{
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
					for (size_t i = 0; i < 9; i++)
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
		foreground[(playerCX)+(playerCY*lw)].draw(hdc);
		return this->color;
	}

	~level()
	{
		for (size_t i = 0; i < lw * lh; i++)
		{
			if(foreground + 1 != nullptr)
				delete (foreground + i);

			if (background + 1 != nullptr)
				delete (background + i);
		}
	}
};