#pragma once

#include <Windows.h>

BOOL _WindowEnumProc(HWND p_winHandle, LPARAM p_lParam);
BOOL GetWindowHandleFromPid(DWORD p_procId, HWND& p_winHandle);
