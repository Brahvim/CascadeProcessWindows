#define CPW_DEBUG

#include "CascadeProcessWindows.hpp"

namespace NsCascadeProcWin {

	struct WinPos {
	public:
		int left, right, top, bottom;

		WinPos()
			: left(0), right(0), top(0), bottom(0) {};

		WinPos(const int p_left, const int p_right, const int p_top, const int p_bottom)
			: left(p_left), right(p_right), top(p_top), bottom(p_bottom) {}
	};

	static DWORD s_numProcs = 0L;
	static bool s_appShouldClose = false;
	static HWND *s_windowHandles = nullptr;
	static WinPos *s_windowPositions = nullptr;

	BOOL getWindowHandleFromPid(const DWORD p_procId, HWND &p_winHandle) {
		static HWND s_winHandleRef = NULL;

		// [https://stackoverflow.com/a/20730976/13951505]
		const BOOL status = EnumWindows([](const HWND p_winHandle, const LPARAM p_lParam) {
			DWORD procId = 0L;
			GetWindowThreadProcessId(p_winHandle, &procId);

			if (procId == g_currProcId)
				return FALSE;

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
		const char *errMsg = "";

		switch (p_errorCode) {
			case EXIT_SUCCESS:
			errMsg = "Successful exit.";
			break;

			case EXIT_FAILURE:
			errMsg = "Some failure...";
			break;

			case CPW_PROC_ARRAY_ALLOC_FAILED:
			errMsg = "Allocating the array of processes failed!";
			break;

			case CPW_ENUM_PROC_FAILED:
			errMsg = "Enumerating through processes failed!";
			break;
		}

		std::cout
			<< "Exiting! Code: `" << p_errorCode
			<< "`, Message: " << errMsg
			<< std::endl;

		std::exit(p_errorCode);
	}

	void appLoop(void) {
		static ULONG frame = 0L;
		static WinPos currentPos;
		static HWND currentHandle;

		Sleep(10);
		frame++;

		for (UINT i = 0; i < s_numProcs - 1; i++) {
			currentHandle = s_windowHandles[i];
			currentPos = s_windowPositions[i];

			if (currentHandle == NULL) {
				//CPW_DEBUG_LOG("Skipped a window!");
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

	// Getting a list of all processes:
	// [https://learn.microsoft.com/en-us/windows/win32/psapi/enumerating-all-processes]

	// Make an `std::vector` to store all PIDs:
	std::vector<DWORD> processIds(1024L, NULL);
	DWORD pidsVectorRequiredSize = -1L; // We do not yet know how many there are.

	std::cout << "Getting current list of processes..." << std::endl;
	if (!EnumProcesses(processIds.data(), (DWORD) processIds.size(), &pidsVectorRequiredSize))
		cpwExit(CPW_ENUM_PROC_FAILED);

	// Calculate the length of that list:
	s_numProcs = pidsVectorRequiredSize / sizeof(DWORD);

	std::cout << "Found `" << s_numProcs << "` processes." << std::endl;
	std::cout << "Getting window handles..." << std::endl;

	s_windowHandles = new HWND[s_numProcs];
	s_windowPositions = new WinPos[s_numProcs];
	std::cout << "(Just finished allocating for them.)" << std::endl;

	RECT currentRect = {0};
	UINT actualWindowCount = 0;
	HWND currentHandle = nullptr;

	// Get handles to windows of all processes:
	for (UINT i = 0; i < s_numProcs; i++) {
		if (!getWindowHandleFromPid(processIds[i], currentHandle))
			continue;

		if (currentHandle != 0) {
			s_windowHandles[i] = currentHandle;

			if (GetWindowRect(currentHandle, &currentRect))
				actualWindowCount++;

			s_windowPositions[i] = WinPos(
				currentRect.left, currentRect.right,
				currentRect.top, currentRect.bottom);
		}
	}

	std::cout
		<< "Succesfully found `" << actualWindowCount << "` windows!"
		<< std::endl;

	HANDLE appLoopThreadId = NULL;

	while (appLoopThreadId == NULL)
		appLoopThreadId = CreateThread(NULL, 0, [](LPVOID p_lParam) -> DWORD {
		while (true)
			appLoop();
	}, nullptr, 0, NULL);


	//auto b = [] {
	//std::cout << "Exit-detection thread started..." << std::endl;
	//cpwExit(EXIT_SUCCESS);
	//std::cout << "Yo! You pressed a key!" << std::endl;
	//(void) getchar();
	//};

	//appLoopThread.join();

	// Move every window to the top-left:
	for (UINT i = 0; i < s_numProcs; i++) {
		WinPos currentPos = s_windowPositions[i];
		currentHandle = s_windowHandles[i];

		if (currentHandle != NULL)
			MoveWindow(currentHandle,
			currentPos.left, currentPos.top,
			currentPos.right - currentPos.left,
			currentPos.bottom - currentPos.top, TRUE);
	}

	CPW_DEBUG_LOG("De-allocating stuff...");
	CloseHandle(appLoopThreadId);
	delete[] s_windowHandles;
	delete[] s_windowPositions;
	CPW_DEBUG_LOG("De-allo*cated* stuff!");

	cpwExit(EXIT_SUCCESS);
}
