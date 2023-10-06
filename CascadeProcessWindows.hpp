#pragma once

#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>

#include <Windows.h>
#include <psapi.h>

#pragma region // Macros.
#ifdef CPW_DEBUG
#define CPW_DEBUG_LOG(x)	std::cerr << x << std::endl;
#else
#define CPW_DEBUG_LOG(x)
#endif

#define CPW_ENUM_PROC_FAILED			1
#define CPW_PROC_ARRAY_ALLOC_FAILED		2
#pragma endregion

namespace NsCascadeProcWin {
	void cpwExit(char errorCode);
	void appLoop(void);
};
