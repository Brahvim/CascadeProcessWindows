#pragma once

#include "CascadeProcessWindows.hpp"

namespace NsCascadeProcWin {
	BOOL windowEnumProc(HWND p_winHandle, LPARAM p_lParam);
	BOOL getWindowHandleFromPid(DWORD p_procId, HWND& p_winHandle);
};
