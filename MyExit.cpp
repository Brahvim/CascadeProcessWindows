#include <stdio.h>
#include <stdlib.h>

#include "CascadeProcessWindowsErrors.h"

void MyExit(char p_errCode) {
	const char* errMsg = nullptr;

	switch (p_errCode) {
	case PROC_ARRAY_ALLOC_FAILED:
		errMsg = "Allocating the array of processes failed!";
		break;
	case ENUM_PROC_FAILED:
		errMsg = "Enumerating through processes failed!";
		break;
	}

	fprintf(stderr, "ERROR `%d`.\n %s\n", p_errCode, errMsg);
	exit(p_errCode);
}