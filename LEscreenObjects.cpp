#include <windows.h>
#include <string>
#include <fstream>
#include <vector>
#include <map>

struct screenObject
{
	RECT rect = RECT();
	COLORREF color = RGB(255, 255, 255);
	std::wstring text;
	bool active = true;
	bool visible = true;

	unsigned short txtFlag = 1;

	bool debug = false;

	screenObject(RECT rect, COLORREF color, std::wstring txt, unsigned int txtFlag = 1, bool debug = false) :
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

	void hide()
	{
		this->active = false;
		this->visible = false;
	}

	void show()
	{
		this->active = true;
		this->visible = true;
	}

	virtual COLORREF draw(HDC* hdc, long mousX = 0, long mouseY = 0, bool mouseDown = false) = 0;
};

struct Button : public screenObject
{
	void(*onPress)(int);

	Button(RECT rect, COLORREF color, void(*function)(int), std::wstring txt = L"", unsigned int txtFlag = 1, bool debug = false) :
		screenObject(rect, color, txt, txtFlag, debug), onPress(function) {}

	void screenObject::press(int n)
	{
		(this->onPress)(n);
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

struct CheckBox : public screenObject
{
	bool* valPtr;

	CheckBox(RECT rect, COLORREF color, bool* valPtr, std::wstring txt = L"", unsigned int txtFlag = 1, bool debug = false) :
		screenObject(rect, color, txt, txtFlag, debug), valPtr(valPtr) {}

	void screenObject::press(int n)
	{
		*this->valPtr = !*(this->valPtr);
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
					if (*this->valPtr)
						FillRect(*hdc, &this->rect, brush);
					else
						FrameRect(*hdc, &this->rect, brush);
				}
				else
				{
					tempcolor = RGB(GetRValue(this->color) + 15, GetGValue(this->color) + 15, GetBValue(this->color) + 15);
					brush = CreateSolidBrush(tempcolor);
					if (*this->valPtr)
						FillRect(*hdc, &this->rect, brush);
					else
						FrameRect(*hdc, &this->rect, brush);
				}
			}
			else
			{
				brush = CreateSolidBrush(this->color);
				if (*this->valPtr)
					FillRect(*hdc, &this->rect, brush);
				else
					FrameRect(*hdc, &this->rect, brush);
			}
		}
		else
		{
			tempcolor = RGB(GetRValue(this->color) - 25, GetGValue(this->color) - 25, GetBValue(this->color) - 25);
			brush = CreateSolidBrush(tempcolor);
			if (*this->valPtr)
				FillRect(*hdc, &this->rect, brush);
			else
				FrameRect(*hdc, &this->rect, brush);
		}
		DeleteObject(brush);
		return tempcolor;
	}
};

struct Text : public screenObject
{
	Text(RECT rect, COLORREF color, std::wstring txt, unsigned int txtFlag = 1, bool debug = false) :
		screenObject(rect, color, txt, txtFlag, debug) {}

	void screenObject::press(int n = 0)
	{
	}

	COLORREF screenObject::draw(HDC* hdc, long mousX, long mouseY, bool mouseDown)
	{
		return this->color; //see drawText in paint.cpp
	}
};

struct Image : public screenObject
{
	HANDLE image = NULL;

	Image(RECT rect, HANDLE img, std::wstring txt = L"", unsigned int txtFlag = 1, bool debug = false) :
		screenObject(rect, RGB(0, 0, 0), txt, txtFlag, debug), image(img) {}

	void screenObject::press(int n = 0)
	{
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
	HANDLE source = NULL;
	HANDLE sourceMask = NULL;
	unsigned int tw;
	unsigned int th;

	tileSet(HANDLE img, HANDLE imgMask, unsigned int tw, unsigned int th) :
		source(img), sourceMask(imgMask), tw(tw), th(th) {}

	void draw(HDC* hdc, int x, int y, unsigned int tx, unsigned int ty)
	{
		BITMAP bm;
		HDC hdcMem = CreateCompatibleDC(*hdc);
		HGDIOBJ hbmOld = SelectObject(hdcMem, source);
		GetObject(source, sizeof(bm), &bm);

		SelectObject(hdcMem, this->sourceMask);
		BitBlt(*hdc, x, y, tw, th, hdcMem, tx * tw, ty * th, SRCAND);

		SelectObject(hdcMem, this->source);
		BitBlt(*hdc, x, y, tw, th, hdcMem, tx * tw, ty * th, SRCPAINT);
	
		DeleteDC(hdcMem);
		DeleteObject(hbmOld);
	}

	~tileSet()
	{
		DeleteObject(source);
	}
};

struct tile
{
	tileSet* tiles;
	unsigned char tx;
	unsigned char ty;

	tileSet* icons;

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
		icons = nullptr;
		solid = false;
	}

	tile(tileSet* tileS, unsigned char tx, unsigned char ty, tileSet* icons, bool solid = true, float friction = 0.1f, bool killer = false) :
		tiles(tileS), tx(tx), ty(ty), icons(icons), solid(solid), friction(friction), killer(killer) {}

	void draw(HDC* hdc, int x, int y, bool sicos)
	{
		tiles->draw(hdc, x + this->imgXOff, y + this->imgYOff, this->tx, this->ty);
		
		if (sicos)
		{
			if (solid) icons->draw(hdc, x + this->imgXOff + 1, y + this->imgYOff + 1, 1, 0);
			else icons->draw(hdc, x + this->imgXOff + 1, y + this->imgYOff + 1, 0, 0);

			if (killer) icons->draw(hdc, x + this->imgXOff + 9, y + this->imgYOff + 1, 0, 1);
			else icons->draw(hdc, x + this->imgXOff + 9, y + this->imgYOff + 1, 1, 1);
		}

	}

	bool isEqual(const tile * other) const
	{
		if (other->tiles != this->tiles) return false;
		if (other->icons != this->icons) return false;
		if (other->specialID != this->specialID) return false;
		if (other->solid != this->solid) return false;
		if (other->friction != this->friction) return false;
		if (other->killer != this->killer) return false;
		if (other->imgXOff != this->imgXOff) return false;
		if (other->imgYOff != this->imgYOff) return false;
		if (other->hitXOff != this->hitXOff) return false;
		if (other->hitYOff != this->hitYOff) return false;
		if (other->tx != this->tx) return false;
		if (other->ty != this->ty) return false;

		return true;
	}
};

struct stage
{
	std::vector<tile*> data; //12:9 of 64x64

	bool exists = false;

	stage()
	{
		this->data.reserve(108);
	}

	stage(std::string data, std::vector<tileSet*>* tileSets, unsigned short tilesetID, tileSet* icons)
	{
		loadStage(data, tileSets, tilesetID, icons);
	}

	void loadStage(std::string data, std::vector<tileSet*>* tileSets, unsigned short tilesetID, tileSet* icons)
	{
		std::string temp = "";
		for (unsigned int i = 0; i < 108; i++)
		{
			temp = "";
			temp += data.at(i * 2);
			temp += data.at((i * 2) + 1);
			unsigned short tileID = std::stoi(temp);
			bool solid = (tileID > 10);
			float friction = (tileID > 32 && tileID < 44) ? 0.02f : 0.3f; //for slippery blocks
			bool killer = (tileID > 21 && tileID < 33) ? true : false;
			this->data.emplace_back(new tile(tileSets->at(tilesetID - 1), tileID % 11, (unsigned int)floor(tileID / 11), icons, solid, friction, killer));
		}
		exists = true;
	}

	void populateEmpty(std::vector<tileSet*>* tileSets, unsigned short tilesetID, tileSet* icons)
	{
		for (unsigned int i = 0; i < 108; i++)
		{
			this->data.emplace_back(new tile(tileSets->at(tilesetID - 1), 0, 0, icons, false, 0.3f));
		}
		exists = true;
	}

	void draw(HDC* hdc, bool sicos)
	{
		if (exists)
		{
			for (unsigned short i = 0; i < data.size(); i++)
			{
				data.at(i)->draw(hdc, (i % 12) * 64, (int)floor(i / 12) * 64, sicos);
			}
		}
	}

	~stage()
	{
		size_t size = this->data.size();
		for (unsigned short i = 0; i < size; i++)
		{
			if (this->data.size() > 0) delete this->data.at(i);
		}
		this->data.clear();
	}
};

#define SIDS std::map<char, unsigned char>{ {'@',0}, {'#',1}, {'=',2}, {'|',3}, {'M',4}, {'B', 5}, {'-',6} }

struct level : public screenObject
{
	stage* foreground = nullptr; //THESE ARE POINTERS TO ARRAYS
	stage* background = nullptr;
	unsigned char lw = 0; //width of level (in stages)
	unsigned char lh = 0; //height of level (in stages)
	unsigned char startCX;
	unsigned char startCY;
	unsigned int startX;
	unsigned int startY;
	unsigned char endCX;
	unsigned char endCY;
	unsigned char endS;
	bool startRune = false;

	bool uninit = true;

	std::vector<tileSet*>* tileSets;
	tileSet* iconSet;

	level(unsigned char lw, unsigned char lh, std::vector<tileSet*>* tileS, tileSet* iconSet) :
		screenObject(rect, RGB(0,0,0), L""), lw(lw), lh(lh), tileSets(tileS), iconSet(iconSet)
	{
		startCX = 0;
		startCY = 0;
		startX = 0;
		startY = 0;
		endCX = 0;
		endCY = 0;
		endS = 0;
		uninit = false;
		stage* fGrnd = new stage[lw * lh];
		stage* bGrnd = new stage[lw * lh];
		for (unsigned short i = 0; i < lw * lh; i++)
		{
			fGrnd[i].populateEmpty(tileS, 1, iconSet);
			bGrnd[i].populateEmpty(tileS, 1, iconSet);
		}
		this->foreground = fGrnd;
		this->background = bGrnd;
	}

	level(LPCWSTR file, std::vector<tileSet*>* tileS, tileSet* iconSet) :
		screenObject(rect, RGB(0, 0, 0), L""), tileSets(tileS), iconSet(iconSet)
	{
		if (!loadLevel(file))
			delete this;
	}

	bool loadLevel(LPCWSTR file)
	{
		if (lw != 0 && lh != 0)
		{
			delete[] foreground;
			delete[] background;
			this->uninit = true;
			lw = 0;
			lh = 0;
		}
		std::fstream lvlDat;
		lvlDat.open(file, std::ios::in);
		if (lvlDat.is_open())
		{
			std::string dat;
			unsigned int start;
			unsigned int end;

			unsigned short stag = 0;
			unsigned short tilesetID;
			std::string temp = "";
			while (std::getline(lvlDat, dat)) {
				switch (SIDS.at(dat.at(0)))
				{
				case 0: //Header
					end = 0;
					for (unsigned char i = 0; i < 10; i++)
					{
						temp = "";
						start = end + 1;
						end = unsigned int(dat.find_first_of('/', start));
						for (unsigned short i = start; i < end; i++)
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
							startRune = (temp.at(0) == 'T') ? true : false;
							break;
						}
					}
					break;
				case 1: //stageHeader
					temp = "";
					start = unsigned int(dat.find_first_of(':') + 1);
					end = unsigned int(dat.find_first_of('[', start));
					for (unsigned int i = start; i < end; i++)
					{
						temp += dat.at(i);
					}
					stag = std::stoi(temp);

					temp = "";
					start = unsigned int(dat.find_first_of(':', end) + 1);
					end = unsigned int(dat.find_first_of(']', start));
					for (unsigned int i = start; i < end; i++)
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
						foreground[stag].loadStage(temp, tileSets, tilesetID, iconSet);
					break;
				case 4: //tileMods
					unsigned char tileModindex;
					temp = dat.substr(1, dat.find_first_of('(') - 1);
					tileModindex = std::stoi(temp);
					end = unsigned int(dat.find_first_of(')'));
					temp = dat.substr(dat.find_first_of('(') + 1, end - (dat.find_first_of('(') + 1));
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
						background[stag].loadStage(temp, tileSets, tilesetID, iconSet);
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

	COLORREF screenObject::draw(HDC* hdc, long playerCX, long playerCY, bool mouseDown)
	{
		background[(playerCX)+(playerCY * lw)].draw(hdc, false);
		foreground[(playerCX)+(playerCY * lw)].draw(hdc, false);
		return this->color;
	}

	~level()
	{
		delete[] foreground;
		delete[] background;
		this->uninit = true;
	}
};