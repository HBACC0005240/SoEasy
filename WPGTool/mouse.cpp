#include "mouse.h"

//�ƶ���굽����λ�ã�X���꣬Y���꣩
void MOUSE::Move(int x, int y)
{
	this->point.x = x;
	this->point.y = y;
	::SetCursorPos(x, y);
}

//�ƶ���굽���λ�ã�Xλ�ƣ�Yλ�ƣ�
void MOUSE::RelativeMove(int cx, int cy)
{
	::GetCursorPos(&this->point);
	this->point.x += cx;
	this->point.y += cy;
	::SetCursorPos(this->point.x, this->point.y);
}

//���浱ǰλ�ã���
void MOUSE::SavePos()
{
	::GetCursorPos(&this->point);
}

//�ָ����λ�ã���
void MOUSE::RestorePos()
{
	::SetCursorPos(this->point.x, this->point.y);
}

//������꣨��
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

//������꣨��
void MOUSE::Unlock()
{
	::ClipCursor(NULL);
}

//�����������
void MOUSE::LBClick()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, this->point.x, this->point.y, 0, 0);
}

//���˫������
void MOUSE::LBDbClick()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, this->point.x, this->point.y, 0, 0);
	::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, this->point.x, this->point.y, 0, 0);
}

//������£���
void MOUSE::LBDown()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_LEFTDOWN, this->point.x, this->point.y, 0, 0);
}

//���̧�𣨣�
void MOUSE::LBUp()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_LEFTUP, this->point.x, this->point.y, 0, 0);
}

//�Ҽ���������
void MOUSE::RBClick()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, this->point.x, this->point.y, 0, 0);
}

//�Ҽ�˫������
void MOUSE::RBDbClick()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, this->point.x, this->point.y, 0, 0);
	::mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, this->point.x, this->point.y, 0, 0);
}

//�Ҽ����£���
void MOUSE::RBDown()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_RIGHTDOWN, this->point.x, this->point.y, 0, 0);
}

//�Ҽ�̧�𣨣�
void MOUSE::RBUp()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_RIGHTUP, this->point.x, this->point.y, 0, 0);
}

//�м���������
void MOUSE::MBClick()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, this->point.x, this->point.y, 0, 0);
}

//�м�˫������
void MOUSE::MBDbClick()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, this->point.x, this->point.y, 0, 0);
	::mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, this->point.x, this->point.y, 0, 0);
}

//�м����£���
void MOUSE::MBDown()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_MIDDLEDOWN, this->point.x, this->point.y, 0, 0);
}

//�м�̧�𣨣�
void MOUSE::MBUp()
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_MIDDLEUP, this->point.x, this->point.y, 0, 0);
}

//�м�����������λ�ƣ�
void MOUSE::MBRoll(int ch)
{
	this->SavePos();
	::mouse_event(MOUSEEVENTF_WHEEL, this->point.x, this->point.y, ch, 0);
}