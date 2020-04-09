#pragma once
#include <windows.h>
#include <QString>
#include <QMap>
#include <QStringList>
#include "XASM.h"

#define GAME_X "0092BD28"
#define GAME_Y "0092BD30"
#define GAME_MOVE_X "008E543C"
#define GAME_MOVE_Y "008E5440"
#define GAME_MOVE_CALL "0044D000"

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
	MOVE_DIRECTION_UP = 1,
	MOVE_DIRECTION_DOWN = 5,
	MOVE_DIRECTION_LEFT = 7,
	MOVE_DIRECTION_RIGHT = 3,
	MOVE_DIRECTION_LEFTUP = 0,
	MOVE_DIRECTION_RIGHTUP = 2,
	MOVE_DIRECTION_LEFTDOWN = 6,
	MOVE_DIRECTION_RIGHTDOWN = 4,
};
enum CHARACTER_STATE {
	CHARACTER_Troop = 1,		//��
	CHARACTER_Battle = 4,		//ս
	CHARACTER_Chat = 8,		//��
	CHARACTER_Card = 16,		//��
	CHARACTER_Exchange = 32,	//��
	CHARACTER_Home = 64,		//��
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
class GameData
{
public:
	static GameData& getInstance();

	void initData();
	void setGameProcess(DWORD pid) { m_gameProcessID = pid; }
	void setGameHwnd(HWND pHwnd) { m_gameHwnd = pHwnd; }
	void setGameHDC(HDC pHdc) { m_gameHDC = pHdc; }
	DWORD getGamePID() { return m_gameProcessID; }
	HWND getGameHwnd() { return m_gameHwnd; }
	HDC getGameHDC() { return m_gameHDC; }
	POINT GetGamePersonCoordinate();
	QString GetGameMapName();
	int getPersionHP();
	int getPersionMP();
	int IsBattering();
	void moveToPoint(int x, int y);//�� ��
	void turn_about(int nDirection);
	void readBattleInfo();
	void getPersonInfo(PERSONINFO* pPerson);
	static void RemoteMoveCall();;

	bool WaitToMove(int x, int y);
	void JianDongXi(int nDirection);
	void Work(int code,const QString itemName="");//ʹ�ü���
	void Work(const QString& skillName, const QString itemName = "");
	bool isWorking();	//�Ƿ�ɼ���
//	static void RemoteWorkCall();
//	void Work(int code);		//�򿪿���
	bool transAssemble(void* pAddr, BYTE* buf,int nLen,QStringList szCmdList);
	void selectRenew();
	void SetCharacterState(int state);
	void SetCharacterSwitch(int v1,int v2,int v3,int v4,int v5,int v6);

private:
	DWORD m_gameProcessID;
	HWND m_gameHwnd;	//��Ϸ���ھ��
	HDC m_gameHDC;
	HDC m_screenHDC;
	//ս��ʱÿ��λ�õ���Ϣ״̬
	PERSONINFO m_Infos[20];
	QMap<int, int>  m_skillCodeForIndex;	//�Զ��弼��ָ������ʵ���±�ӳ��
	QMap<int, QString> m_indexForSkillName;	//�����±��Ӧ��������
	QStringList m_szPickSkillList;
	XASM m_asm;
	QMap<int, int> m_skillCodeForMyCode;//��Ϸ�����code���Զ���code��Ӧ
	QMap<QString, int> m_characterForVal;	//��Ϸ����״̬���ƺ�ʵ���ڴ�ֵ
};

