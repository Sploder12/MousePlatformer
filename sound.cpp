#include "sound.h"

DWORD openAlias(std::wstring fileName, std::wstring alias)
{
	fileName = L"Open \"snds/" + fileName + L"\" alias " + alias;
	return mciSendString(fileName.c_str(), NULL, 0, NULL);
}

DWORD playAlias(std::wstring alias)
{
	alias = L"play " + alias + L" from 1"; //alias becomes the send string
	return mciSendString(alias.c_str(), NULL, 0, NULL);
}

DWORD closeAlias(std::wstring alias)
{
	alias = L"close " + alias;
	return mciSendString(alias.c_str(), NULL, 0, NULL);
}

DWORD playFile(std::wstring fileName, bool loop) //.wav files can't be looped
{
	fileName = L"play \"snds/" + fileName + L"\"" + ((loop) ? L" repeat" : L""); //fileName becomes the send string
	return mciSendString(fileName.c_str(), NULL, 0, NULL);
}

DWORD stopSound()
{
	return NULL;
}
