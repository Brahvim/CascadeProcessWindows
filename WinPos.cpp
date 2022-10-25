#include "WinPos.h"

struct WinPos {
	int left, right, top, bottom;
	//int left = 0, right = 0, top = 0, bottom = 0;

	WinPos() : left(0), right(0), top(0), bottom(0) {};

	WinPos(int p_left, int p_right, int p_top, int p_bottom) :
		left(p_left), right(p_right), top(p_top), bottom(p_bottom) {}
};
