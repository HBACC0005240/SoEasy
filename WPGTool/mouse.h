////////////////////
//mouse.h
//模拟鼠标的常见操作
////////////////////
#pragma once
#include <windows.h>

class MOUSE
{
private:
	//坐标变量
	POINT point;
public:
	//移动类函数
	void    Move(int x, int y);
	void    RelativeMove(int cx, int cy);
	void    SavePos();
	void    RestorePos();
	//锁定启用类
	void    Lock();
	void    Unlock();
	//动作类
	void    LBClick();
	void    LBDbClick();
	void    LBDown();
	void    LBUp();
	void    RBClick();
	void    RBDbClick();
	void    RBDown();
	void    RBUp();
	void    MBClick();
	void    MBDbClick();
	void    MBDown();
	void    MBUp();
	void    MBRoll(int ch);
};