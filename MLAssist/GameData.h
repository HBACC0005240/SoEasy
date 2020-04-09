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
#define MOVE_STEP_DIAGONAL 64//左右
#define MOVE_STEP_UPDOWN 48//上下
#define MOVE_STEP 32  
#define GAME_CENTER_X 320
#define GAME_CENTER_Y 240
#define STEP_VAL 16		//斜边 16取值
#define MAX_STEP 4		//一次最多走4个坐标 不然就超出边界



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
	CHARACTER_Troop = 1,		//队
	CHARACTER_Battle = 4,		//战
	CHARACTER_Chat = 8,		//聊
	CHARACTER_Card = 16,		//名
	CHARACTER_Exchange = 32,	//易
	CHARACTER_Home = 64,		//家
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
	void moveToPoint(int x, int y);//东 南
	void turn_about(int nDirection);
	void readBattleInfo();
	void getPersonInfo(PERSONINFO* pPerson);
	static void RemoteMoveCall();;

	bool WaitToMove(int x, int y);
	void JianDongXi(int nDirection);
	void Work(int code,const QString itemName="");//使用技能
	void Work(const QString& skillName, const QString itemName = "");
	bool isWorking();	//是否采集中
//	static void RemoteWorkCall();
//	void Work(int code);		//打开开关
	bool transAssemble(void* pAddr, BYTE* buf,int nLen,QStringList szCmdList);
	void selectRenew();
	void SetCharacterState(int state);
	void SetCharacterSwitch(int v1,int v2,int v3,int v4,int v5,int v6);

private:
	DWORD m_gameProcessID;
	HWND m_gameHwnd;	//游戏窗口句柄
	HDC m_gameHDC;
	HDC m_screenHDC;
	//战斗时每个位置的信息状态
	PERSONINFO m_Infos[20];
	QMap<int, int>  m_skillCodeForIndex;	//自定义技能指令代码和实际下标映射
	QMap<int, QString> m_indexForSkillName;	//数组下标对应技能名称
	QStringList m_szPickSkillList;
	XASM m_asm;
	QMap<int, int> m_skillCodeForMyCode;//游戏定义的code和自定义code对应
	QMap<QString, int> m_characterForVal;	//游戏人物状态名称和实际内存值
};

