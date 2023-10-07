#pragma once

#include <cstdlib>
#include <iostream>
#include <vector>

#include <Windows.h>
#include <psapi.h>

#pragma region // Macros.
// Debugging:
#ifdef CPW_DEBUG
#define CPW_DEBUG_LOG(x)	std::cerr << x << std::endl;
#else
#define CPW_DEBUG_LOG(x)
#endif

// Errors:
#define CPW_ENUM_PROC_FAILED			-2
#define CPW_PROC_ARRAY_ALLOC_FAILED		-3
#pragma endregion

const DWORD g_currProcId = GetCurrentProcessId();

namespace NsCascadeProcWin {
	struct WinPos;

	BOOL getWindowHandleFromPid(const DWORD p_procId, const HWND &p_winHandle);
	void cpwExit(const char errorCode);
	void appLoop(void);
};
