/////////////////////////
//keyboard.h
//模拟键盘的常见操作
/////////////////////////
#pragma once
#include <windows.h>

class KEYBOARD
{
public:
	void    PressStr(char *str);
	void    PressKey(BYTE bVk);
	void    KeyDown(BYTE bVk);
	void    KeyUp(BYTE bVk);
};