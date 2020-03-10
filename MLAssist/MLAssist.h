#pragma once

#include <QtWidgets/QDialog>
#include "ui_MLAssist.h"
#include "YunLai.h"
#include <QTimer>

class MLAssist : public QWidget
{
	Q_OBJECT

public:
	MLAssist(QWidget *parent = Q_NULLPTR);
	~MLAssist();

//	void initComboBox();
	void initPersonPos();
	void SetPersonInfo(PERSONINFO& info, char* name, int hp, int totalhp, int mp, int totalmp, int level);
	void refreshBattleUI();
	void readBattleInfo();
	void MoveToGamePoint(int gameX,int gameY,long stampTime=0,long nTryTime =0);
	void MoveToGamePoint2(int gameX, int gameY, long stampTime = 0, long nTryTime = 0);

	void faceToDirection(int direction);
	int GetGameStatus();
	POINT GetGamePersonPos(int gamePos);
	POINT GetGamePersonCoordinate();
	void clickedYes();
	void clickedNo();
	void clickedSkillBtn();
	bool WaitSkillDlg();
	bool WaitAndWakeSkillDlg();

	void nowhile(QString szMap,int status=1);
	bool WaitActDlg();
	bool WaitDialog();
	bool WaitSelectOpe();
	bool WaitAutoEny();
	void useFirstSkill();
	void attackAnyEnemy();
	int getPersionHP();
	int getPersionMP();
//	static void AttackThread(MLAssist* pThis);
	void moveToPointEast();
	void AutoEny();
	static void RemoteMoveCall();
signals:
	void signal_autoBattle();
public slots:
	void on_checkBox_autoBattle_clicked();
	void on_autoBattle();
	void on_pushButton_up_clicked();
	void on_pushButton_down_clicked();
	void on_pushButton_left_clicked();
	void on_pushButton_right_clicked();
//	void dealHotKeyEvent(const QKeySequence& key);
private:
	Ui::MLAssistClass ui;
	POINT m_personPos[20];
	DWORD m_gameProcessID;
	HWND m_gameHwnd;	//游戏窗口句柄
	HDC m_gameHDC;
	HDC m_screenHDC;
	QTimer m_getBattleTimer;
	//战斗时每个位置的信息状态
	PERSONINFO m_Infos[20];
	bool m_autoBattle;
	POINT m_lastPoint;
	POINT pointEscape;
	POINT pointAttack;
	POINT pointDefense;
	POINT pointSkill;
	POINT pointThings;
	POINT m_skillPos[10];
	POINT pointYes;
	POINT pointNo;
	uint m_KeyStart;
	uint m_KeyStop;
	POINT m_lastEnyPoint;
	POINT m_recordPoint;
};
/*
经验值：EXP（Experience）
战斗力：CE（Combat Effectiveness)
血量：HP(Health Point）
魔力值：MP(Magic Point)
级别 / 级数：LV(Level)
普通金钱：Cash / Money / Game Point(常用图标代替)
黄金：Gold(常用图标代替)
钻石：Diamond(常用图标代替)
武器：Weapon
主武器：Primary Weapon
副武器：Secondary Weapon
伤害：Damage
杀敌：Kill
弹药：AMMO / Ammunition
爆头：Head Shot
*/