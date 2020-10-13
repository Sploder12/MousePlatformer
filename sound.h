#include <windows.h>
#include <string>

DWORD openAlias(std::wstring fileName, std::wstring alias);

DWORD playAlias(std::wstring alias);

DWORD closeAlias(std::wstring alias);

DWORD playFile(std::wstring fileName, bool loop = false);

DWORD stopSound();