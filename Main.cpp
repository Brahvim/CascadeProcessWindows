#define CPW_DEBUG

#include "CascadeProcessWindows.hpp"

namespace NsCascadeProcWin {

	struct WinPos {
		int left, right, top, bottom;
		//int left = 0, right = 0, top = 0, bottom = 0;
		// ^^^ The initializer list is invoked anyway, so I better do this!

		WinPos() : left(0), right(0), top(0), bottom(0) {};

		WinPos(int p_left, int p_right, int p_top, int p_bottom) :
			left(p_left), right(p_right), top(p_top), bottom(p_bottom) {}
	};

	static DWORD s_numProcs = 0L;
	static HWND *s_windowHandles;
	static bool s_appShouldClose = false;
	static WinPos *s_windowPositions = nullptr;

	BOOL getWindowHandleFromPid(const DWORD p_procId, HWND &p_winHandle) {
		static HWND s_winHandleRef = NULL;

		// [https://stackoverflow.com/a/20730976/13951505]
		const BOOL status = EnumWindows([](const HWND p_winHandle, const LPARAM p_lParam) {
			DWORD procId = 0L;
			GetWindowThreadProcessId(p_winHandle, &procId);

			// If-else guard clause!:
			if (procId != p_lParam)
				return TRUE;

			s_winHandleRef = p_winHandle;
			return FALSE;
		}, p_procId);

		p_winHandle = s_winHandleRef;
		return status;
	}

	void cpwExit(const char p_errorCode) {
		const char *errMsg = (const char *)nullptr;

		switch (p_errorCode) {
			case CPW_PROC_ARRAY_ALLOC_FAILED:
			{
				errMsg = "Allocating the array of processes failed!";
			} break;

			case CPW_ENUM_PROC_FAILED:
			{
				errMsg = "Enumerating through processes failed!";
			} break;
		}

		CPW_DEBUG_LOG("Error! Code:`" << p_errorCode << "`, Message: " << errMsg);
		std::exit(p_errorCode);
	}

	void appLoop(void) {
		static ULONG frame = 0L;
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
				(int) (sinf((float) (double) frame) * 50),
				(int) (cosf((float) (double) frame) * 50),
				// 400, 200,
				currentPos.right - currentPos.left,
				currentPos.bottom - currentPos.top, TRUE);
		}
	}

};

int main(void) {
	using namespace NsCascadeProcWin;

	CPW_DEBUG_LOG("Start!");

	// [https://learn.microsoft.com/en-us/windows/win32/psapi/enumerating-all-processes]
	DWORD pidsVectorRequiredSize = -1;
	std::vector<DWORD> processIds(1024, NULL);

	CPW_DEBUG_LOG("Getting current list of processes...");

	if (!EnumProcesses(processIds.data(), (DWORD) processIds.size(), &pidsVectorRequiredSize))
		exit(CPW_ENUM_PROC_FAILED);

	// Calculate the length of that list:
	s_numProcs = pidsVectorRequiredSize / sizeof(DWORD);

	printf("Found `%lu` processes.\n", s_numProcs);

	CPW_DEBUG_LOG("Getting window handles...");

	s_windowHandles = new HWND[s_numProcs];
	s_windowPositions = new WinPos[s_numProcs];

	UINT actualWindowCount = 0;
	RECT currentRect = {0};
	HWND currentHandle = nullptr;

	for (UINT i = 0; i < s_numProcs; i++) {
		if (!getWindowHandleFromPid(processIds[i], currentHandle))
			continue;

		s_windowHandles[i] = currentHandle;

		if (currentHandle != 0) {
			GetWindowRect(currentHandle, &currentRect);
			actualWindowCount++;
		} else {
			s_windowPositions[i] = WinPos(currentRect.left, currentRect.right,
				currentRect.top, currentRect.bottom);
		}
	}

	CPW_DEBUG_LOG("Succesfully scanned through `" << actualWindowCount << "` windows!");

	std::thread appLoopThread([] {
		while (true)
			appLoop();
	});

	appLoopThread.join();

	std::thread exitDetectorThread([] {
		std::cout << "Exit-detection thread started..." << std::endl;
		exit(EXIT_SUCCESS);
		std::cout << "Yo! You pressed a key!" << std::endl;
		(void) getchar();
	});

	for (UINT i = 0; i < s_numProcs; i++) {
		WinPos currentPos = s_windowPositions[i];
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
