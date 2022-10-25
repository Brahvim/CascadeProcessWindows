#include <stdio.h>
#include <stdlib.h>
#include <thread>

#include <Windows.h>
#include <psapi.h>

#include "WinPos.cpp"
#include "PidToWindowHandle.h"
#include "CascadeProcessWindowsErrors.h"

#pragma region Forward declarations.
void AppLoop(void);
#pragma endregion

// [https://learn.microsoft.com/en-us/windows/win32/psapi/enumerating-all-processes]

HWND* windowHandles;
// Handles are basically pointers, but the API you're
// using (Windows here!) allocates and frees them.
// ...so this allocation is just a bunch of `ulong`s :)

// "Count" of processes.
DWORD numProcs = -1;
WinPos* windowPositions;

bool appShouldClose = false;

int main(void) {


	puts("Started execution!~");

	// Pointer-array of processes
	DWORD allProcs[1024] = {};
	// For some reason, it works ONLY if allocated on the stack.

	// ("cb" stands for "count of bytes").
	DWORD cbAllProcs = -1;

	puts("Getting current process list...");

	// Get a list of currently active processes:
	if (allProcs != NULL)
		if (!EnumProcesses(allProcs, sizeof(allProcs), &cbAllProcs)) {
			MyExit(ENUM_PROC_FAILED);
		}

	// Calculate the length of that list:
	numProcs = cbAllProcs / sizeof(DWORD);

	printf("Found `%d` processes.\n", numProcs);

	puts("Getting window handles...");

	windowHandles = new HWND[numProcs];
	// Handles are basically pointers, but the API you're
	// using (Windows here!) allocates and frees them.
	// ...so this allocation is just a bunch of `ulong`s :)

	windowPositions = new WinPos[numProcs];

	HWND currentHandle = nullptr;
	LPRECT currentRect = nullptr;

	UINT actualWindows = 0;
	for (UINT i = 0; i < numProcs; i++) {
		if (!GetWindowHandleFromPid(allProcs[i], currentHandle))
			continue;
		windowHandles[i] = currentHandle;

		//#pragma warning(suppress:6387) // "`cuRrEnTrEcT` cOuLD bE `0`".
		//currentRect.

		if (currentHandle != 0) {
			GetWindowRect(currentHandle, currentRect);
			actualWindows++;
			if (currentRect == 0)
				continue;
			else {
				windowPositions[i] = WinPos(
					currentRect->left, currentRect->right,
					currentRect->top, currentRect->bottom);
			}
		}
	}

	printf("Succesfully scanned through `%d` windows!\n", actualWindows);

	std::thread appLoopThread(AppLoop);
	appLoopThread.join();

	appShouldClose = false;
	Sleep(1000);
	appShouldClose = true;

	WinPos currentPos;
	for (UINT i = 0; i < numProcs; i++) {
		currentHandle = windowHandles[i];
		currentPos = (windowPositions[i]);

		MoveWindow(currentHandle,
			currentPos.left,
			currentPos.top,
			currentPos.right - currentPos.left,
			currentPos.bottom - currentPos.top,
			TRUE);
	}

	puts("Everything should be normal now - bye!~");

	delete[] windowHandles;
	delete[] windowPositions;

#pragma warning(suppress:6031)
	//getchar();
	return 0;
}

void AppLoop(void) {
	HWND currentHandle;
	WinPos currentPos;

	ULONG frame = 0;

	while (!appShouldClose) {
		++frame;
		Sleep(10);

		for (UINT i = 0; i < numProcs; i++) {
			currentHandle = windowHandles[i];
			currentPos = windowPositions[i];

			MoveWindow(currentHandle,
				sinf(frame) * 50, cosf(frame) * 50,
				400, 200,
				//currentPos.right - currentPos.left,
				//currentPos.bottom - currentPos.top,
				TRUE);
		}
	}
}
