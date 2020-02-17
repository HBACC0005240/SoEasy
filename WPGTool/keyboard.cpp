#include "keyboard.h"
// 按键（虚拟键值）
void KEYBOARD::PressKey(BYTE bVk)
{
	::keybd_event(bVk, 0, 0, 0);
	::keybd_event(bVk, 0, KEYEVENTF_KEYUP, 0);
}

//按下（虚拟键值）
void KEYBOARD::KeyDown(BYTE bVk)
{
	::keybd_event(bVk, 0, 0, 0);
}

//抬起（虚拟键值）
void KEYBOARD::KeyUp(BYTE bVk)
{
	::keybd_event(bVk, 0, KEYEVENTF_KEYUP, 0);
}

//发送字符串（字符串）
void KEYBOARD::PressStr(char *str)
{
	for (unsigned i = 0; i < strlen(str); i++)
	{
		if (str[i] > 0x60 && str[i] < 0x7B)
			this->PressKey(str[i] - 0x20);
		else
			this->PressKey(str[i]);
	}
}