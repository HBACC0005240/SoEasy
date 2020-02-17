#pragma once

#include <QtWidgets/QDialog>
#include "ui_MLAssist.h"
#include "../WPGTool/YunLai.h"
#include <QTimer>
#define MAXBUFLEN 100
#define MOVE_STEP_DIAGONAL 64//左右
#define MOVE_STEP_UPDOWN 48//上下
#define MOVE_STEP 32  
#define GAME_CENTER_X 320
#define GAME_CENTER_Y 240
#define STEP_VAL 16		//斜边 16取值
#define MAX_STEP 4		//一次最多走4个坐标 不然就超出边界



enum MOVE_DIRECTION
{
	MOVE_DIRECTION_UP=1,
	MOVE_DIRECTION_DOWN ,
	MOVE_DIRECTION_LEFT ,
	MOVE_DIRECTION_RIGHT ,
	MOVE_DIRECTION_LEFTUP ,
	MOVE_DIRECTION_RIGHTUP ,
	MOVE_DIRECTION_LEFTDOWN ,
	MOVE_DIRECTION_RIGHTDOWN,
};
struct PERSONINFO
{
	WCHAR name[MAXBUFLEN];	//名称
	int hp;					//血量
	int mp;					//魔力
	int totalhp;			//总血量
	int totalmp;			//总魔力
	int level;				//等级
};
class MLAssist : public QDialog
{
	Q_OBJECT

public:
	MLAssist(QWidget *parent = Q_NULLPTR);
	~MLAssist();

	void initPersonPos();
	void SetPersonInfo(PERSONINFO& info, char* name, int hp, int totalhp, int mp, int totalmp, int level);
	void refreshBattleUI();
	void readBattleInfo();
	void setLoginUser(const QString& szText);
	void setItemText(int row,int col,const QString& szText);
	void MoveToGamePoint(int gameX,int gameY,long stampTime=0,long nTryTime =0);
	void MoveToGamePoint2(int gameX, int gameY, long stampTime = 0, long nTryTime = 0);

	int GetGameStatus();
	POINT GetGamePersonPos(int gamePos);

	bool WaitSkillDlg();
	bool WaitActDlg();
	void useFirstSkill();
	void attackAnyEnemy();
	static void AttackThread(MLAssist* pThis);
signals:
	void signal_autoBattle();
public slots:
	void on_pushButton_clicked();
	void on_pushButton_hook_clicked();
	void doUpdateBattleInfo();
	void on_checkBox_autoBattle_clicked();
	void on_autoBattle();
	void on_pushButton_move_clicked();
	void on_pushButton_up_clicked();
	void on_pushButton_down_clicked();
	void on_pushButton_left_clicked();
	void on_pushButton_right_clicked();
	void on_pushButton_color_clicked();
private:
	Ui::MLAssistClass ui;
	POINT m_personPos[20];
	DWORD m_gameProcessID;
	HWND m_gameHwnd;	//游戏窗口句柄
	HDC m_gameHDC;
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