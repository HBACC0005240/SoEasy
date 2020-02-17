#pragma once

#include <QtWidgets/QDialog>
#include "ui_MLAssist.h"
#include "../WPGTool/YunLai.h"
#include <QTimer>
#define MAXBUFLEN 100
#define MOVE_STEP_DIAGONAL 64//����
#define MOVE_STEP_UPDOWN 48//����
#define MOVE_STEP 32  
#define GAME_CENTER_X 320
#define GAME_CENTER_Y 240
#define STEP_VAL 16		//б�� 16ȡֵ
#define MAX_STEP 4		//һ�������4������ ��Ȼ�ͳ����߽�



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
	WCHAR name[MAXBUFLEN];	//����
	int hp;					//Ѫ��
	int mp;					//ħ��
	int totalhp;			//��Ѫ��
	int totalmp;			//��ħ��
	int level;				//�ȼ�
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
	HWND m_gameHwnd;	//��Ϸ���ھ��
	HDC m_gameHDC;
	QTimer m_getBattleTimer;
	//ս��ʱÿ��λ�õ���Ϣ״̬
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
����ֵ��EXP��Experience��
ս������CE��Combat Effectiveness)
Ѫ����HP(Health Point��
ħ��ֵ��MP(Magic Point)
���� / ������LV(Level)
��ͨ��Ǯ��Cash / Money / Game Point(����ͼ�����)
�ƽ�Gold(����ͼ�����)
��ʯ��Diamond(����ͼ�����)
������Weapon
��������Primary Weapon
��������Secondary Weapon
�˺���Damage
ɱ�У�Kill
��ҩ��AMMO / Ammunition
��ͷ��Head Shot
*/