#include "mouse.h"

//移动鼠标到绝对位置（X坐标，Y坐标）
void MOUSE::Move(int x, int y)
{
	this->point.x = x;
	this->point.y = y;
	::SetCursorPos(x, y);
}

//移动鼠标到相对位置（X位移，Y位移）
void MOUSE::RelativeMove(int cx, int cy)
{
	::GetCursorPos(&this->point);
	this->point.x += cx;
	this->point.y += cy;
	::SetCursorPos(this->point.x, this->point.y);
}

//保存当前位置（）
void MOUSE::SavePos()
{
	::GetCursorPos(&this->point);
}

//恢复鼠标位置（）
void MOUSE::RestorePos()
{
	::SetCursorPos(this->point.x, this->point.y);
}

//锁定鼠标（）
void MOUSE::Lock()
{
	POINT    pt;
	RECT    rt;

	::GetCursorPos(&pt);
	rt.left = rt.right = pt.x;
	rt.top = rt.bottom = pt.y;
	rt.right++;
	rt.bottom++;
	::ClipCursor(&rt);
}

//解锁鼠标（）
void MOUSE::Unlock()
{
	::ClipCursor(NULL);
}

//左键单击（）
void MOUSE::LBClick()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, this->point.x, this->point.y, 0, 0);
}

//左键双击（）
void MOUSE::LBDbClick()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, this->point.x, this->point.y, 0, 0);
	::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, this->point.x, this->point.y, 0, 0);
}

//左键按下（）
void MOUSE::LBDown()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_LEFTDOWN, this->point.x, this->point.y, 0, 0);
}

//左键抬起（）
void MOUSE::LBUp()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_LEFTUP, this->point.x, this->point.y, 0, 0);
}

//右键单击（）
void MOUSE::RBClick()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, this->point.x, this->point.y, 0, 0);
}

//右键双击（）
void MOUSE::RBDbClick()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, this->point.x, this->point.y, 0, 0);
	::mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, this->point.x, this->point.y, 0, 0);
}

//右键按下（）
void MOUSE::RBDown()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_RIGHTDOWN, this->point.x, this->point.y, 0, 0);
}

//右键抬起（）
void MOUSE::RBUp()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_RIGHTUP, this->point.x, this->point.y, 0, 0);
}

//中键单击（）
void MOUSE::MBClick()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, this->point.x, this->point.y, 0, 0);
}

//中键双击（）
void MOUSE::MBDbClick()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, this->point.x, this->point.y, 0, 0);
	::mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, this->point.x, this->point.y, 0, 0);
}

//中键按下（）
void MOUSE::MBDown()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_MIDDLEDOWN, this->point.x, this->point.y, 0, 0);
}

//中键抬起（）
void MOUSE::MBUp()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_MIDDLEUP, this->point.x, this->point.y, 0, 0);
}

//中键滚动（滚动位移）
void MOUSE::MBRoll(int ch)
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_WHEEL, this->point.x, this->point.y, ch, 0);
}