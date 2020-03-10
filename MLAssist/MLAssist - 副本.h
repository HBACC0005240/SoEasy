#pragma once

#include <QtWidgets/QDialog>
#include "ui_MLAssist.h"
#include "YunLai.h"
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
	MOVE_DIRECTION_DOWN =5,
	MOVE_DIRECTION_LEFT =7,
	MOVE_DIRECTION_RIGHT =3,
	MOVE_DIRECTION_LEFTUP=0 ,
	MOVE_DIRECTION_RIGHTUP=2 ,
	MOVE_DIRECTION_LEFTDOWN=6 ,
	MOVE_DIRECTION_RIGHTDOWN=4,
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
class MLAssist : public QWidget
{
	Q_OBJECT

public:
	MLAssist(QWidget *parent = Q_NULLPTR);
	~MLAssist();

	void initComboBox();
	void initPersonPos();
	void SetPersonInfo(PERSONINFO& info, char* name, int hp, int totalhp, int mp, int totalmp, int level);
	void refreshBattleUI();
	void readBattleInfo();
	void setLoginUser(const QString& szText);
	void setItemText(int row,int col,const QString& szText);
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
	bool WaitToMove(int x,int y);
	static void AttackThread(MLAssist* pThis);
	void moveToPoint(int x,int y);//�� ��
	void moveToPointEast();
	void AutoEny();
	static void RemoteMoveCall();
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
	void dealHotKeyEvent(const QKeySequence& key);
private:
	Ui::MLAssistClass ui;
	POINT m_personPos[20];
	DWORD m_gameProcessID;
	HWND m_gameHwnd;	//��Ϸ���ھ��
	HDC m_gameHDC;
	HDC m_screenHDC;
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
	POINT pointYes;
	POINT pointNo;
	uint m_KeyStart;
	uint m_KeyStop;
	POINT m_lastEnyPoint;
	POINT m_recordPoint;
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