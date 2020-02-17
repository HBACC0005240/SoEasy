////////////////////
//mouse.h
//ģ�����ĳ�������
////////////////////
#pragma once
#include <windows.h>

class MOUSE
{
private:
	//�������
	POINT point;
public:
	//�ƶ��ຯ��
	void    Move(int x, int y);
	void    RelativeMove(int cx, int cy);
	void    SavePos();
	void    RestorePos();
	//����������
	void    Lock();
	void    Unlock();
	//������
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