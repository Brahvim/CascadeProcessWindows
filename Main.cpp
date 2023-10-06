#include "WinPos.cpp"
#include "PidToWindowHandle.hpp"
#include "CascadeProcessWindows.hpp"

#define CPW_DEBUG

namespace NsCascadeProcWin {
	// [https://learn.microsoft.com/en-us/windows/win32/psapi/enumerating-all-processes]

	static HWND* s_windowHandles;
	// Handles are basically pointers, but the API you're
	// using (WinAPI here!) allocates and frees them.
	// ...so this allocation is just a bunch of `ull`s :)

	// "Count" of processes:
	static DWORD s_numProcs = 0;
	static bool g_appShouldClose = false;
	static WinPos* s_windowPositions = nullptr;

	void appLoop(void) {
		static ULONG frame = 0;
		static WinPos currentPos;
		static HWND currentHandle;

		frame++;

		for (UINT i = 0; i < s_numProcs - 1; i++) {
			currentHandle = s_windowHandles[i];
			currentPos = s_windowPositions[i];

			if (currentHandle == NULL) {
				CPW_DEBUG_LOG("Skipped a window!");
				continue;
			}

			MoveWindow(currentHandle,
				(int)(sinf((float)(double)frame) * 50),
				(int)(cosf((float)(double)frame) * 50),
				//400, 200,
				currentPos.right - currentPos.left,
				currentPos.bottom - currentPos.top, TRUE);
		}
	}

	void cpwExit(const char p_errorCode) {
		const char* errMsg = nullptr;

		switch (p_errorCode) {
		case CPW_PROC_ARRAY_ALLOC_FAILED: {
			errMsg = "Allocating the array of processes failed!";
		} break;

		case CPW_ENUM_PROC_FAILED: {
			errMsg = "Enumerating through processes failed!";
		} break;
		}

		CPW_DEBUG_LOG("Error! Code:`" << p_errorCode << "`, Message: " << errMsg);
		std::exit(p_errorCode);
	}
};

int main(void) {
	using namespace NsCascadeProcWin;

	CPW_DEBUG_LOG("Start!");

	DWORD pidsVectorRequiredSize = -1;
	std::vector<DWORD> processIds(1024, NULL);

	CPW_DEBUG_LOG("Getting current list of processes...");

	if (!EnumProcesses(processIds.data(), processIds.size(), &pidsVectorRequiredSize))
		exit(CPW_ENUM_PROC_FAILED);

	// Calculate the length of that list:
	s_numProcs = pidsVectorRequiredSize / sizeof(DWORD);

	printf("Found `%lu` processes.\n", s_numProcs);

	CPW_DEBUG_LOG("Getting window handles...");

	s_windowHandles = new HWND[s_numProcs];
	s_windowPositions = new WinPos[s_numProcs];

	UINT actualWindows = 0;
	RECT currentRect = { 0 };
	HWND currentHandle = nullptr;

	for (UINT i = 0; i < s_numProcs; i++) {
		if (!getWindowHandleFromPid(processIds[i], currentHandle))
			continue;

		s_windowHandles[i] = currentHandle;

		if (currentHandle != 0) {
			GetWindowRect(currentHandle, &currentRect);
			actualWindows++;
		}
		else {
			s_windowPositions[i] = WinPos(
				currentRect.left, currentRect.right,
				currentRect.top, currentRect.bottom);
		}
	}

	CPW_DEBUG_LOG("Succesfully scanned through `%d` windows!\n", actualWindows);

	std::thread appLoopThread([] {
		while (true)
			appLoop();
	});

	appLoopThread.join();

	std::thread exitDetectorThread([] {
		(void)getchar();
		exit(EXIT_SUCCESS);
	});

	WinPos currentPos;
	for (UINT i = 0; i < s_numProcs; i++) {
		currentPos = s_windowPositions[i];
		currentHandle = s_windowHandles[i];

		if (currentHandle != NULL)
			MoveWindow(currentHandle,
				currentPos.left, currentPos.top,
				currentPos.right - currentPos.left,
				currentPos.bottom - currentPos.top, TRUE);
	}

	CPW_DEBUG_LOG("Everything should be normal now - bye!~");

	delete[] s_windowHandles;
	delete[] s_windowPositions;
}
