#include "PidToWindowHandle.h"

static HWND s_WinHandleRef = NULL;
// ^^^ Could have made this a variable for JUST this file using `static`, 
// but I liked the idea of portability for this code!~

// [https://stackoverflow.com/a/20730976/13951505]
BOOL CALLBACK _WindowEnumProc(HWND p_winHandle, LPARAM p_lParam) {
	DWORD procId;
	GetWindowThreadProcessId(p_winHandle, &procId);

	// If-else guard clause!:
	if (procId != p_lParam)
		return TRUE;

	s_WinHandleRef = p_winHandle;
	return FALSE;
}

BOOL GetWindowHandleFromPid(DWORD p_procId, HWND& p_winHandle) {
	BOOL status = EnumWindows(_WindowEnumProc, p_procId);
	p_winHandle = s_WinHandleRef;
	return status;
}
