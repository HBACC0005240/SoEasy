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
class GameItem
{
public:
	QString itemName;//名称
	QString itemId;	//id
	int itemCount;	//数量
	int index;		//位置
};
//size = 148
typedef struct skill_sub_s
{
	char name[25];//0
	char info[99];//25
	int cost;//124
	int unk1;//128
	int flags;//132
	int unk3;//136
	int available;//140
	int level;//144
}skill_sub_t;


typedef struct skill_craft_mat_s
{
	int itemid;//-1 if none
	int count;//-1 if none
	char name[32];
}skill_craft_mat_t;

static_assert(sizeof(skill_craft_mat_t) == 40, "Size check");

typedef struct skill_craft_s
{
	int subskill_id;
	int cost;
	int level;
	int available;
	int itemid;
	char name[29];
	char info[59];
	skill_craft_mat_t mats[5];
}skill_craft_t;

//size=0x49FC
typedef struct skill_s
{
	char name[25];
	char slotsize;
	char padding[2];
	int level;
	int maxlevel;
	int xp;
	int maxxp;
	int type;//1=制造 2=鉴定 3=治疗 4=采集
	int skill_id;//255 =???, 225=伐木
	int unk3;
	int pos;
	//offset=60
	skill_sub_t sub[11];
	;//offset=1688
	skill_craft_t craft[50];
	char padding2[1852];
}skill_t;
typedef struct cga_playerpet_detail_info_s
{
	cga_playerpet_detail_info_s()
	{
		points_remain = 0;
		points_endurance = 0;
		points_strength = 0;
		points_defense = 0;
		points_agility = 0;
		points_magical = 0;
		value_attack = 0;
		value_defensive = 0;
		value_agility = 0;
		value_spirit = 0;
		value_recovery = 0;
		resist_poison = 0;
		resist_sleep = 0;
		resist_medusa = 0;
		resist_drunk = 0;
		resist_chaos = 0;
		resist_forget = 0;
		fix_critical = 0;
		fix_strikeback = 0;
		fix_accurancy = 0;
		fix_dodge = 0;
		element_earth = 0;
		element_water = 0;
		element_fire = 0;
		element_wind = 0;
	}
	cga_playerpet_detail_info_s(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10,
		int a11, int a12, int a13, int a14, int a15, int a16, int a17, int a18, int a19, int a20,
		int a21, int a22, int a23, int a24, int a25) :
		points_remain(a1), points_endurance(a2), points_strength(a3), points_defense(a4), points_agility(a5), points_magical(a6),
		value_attack(a7), value_defensive(a8), value_agility(a9), value_spirit(a10), value_recovery(a11),
		resist_poison(a12), resist_sleep(a13), resist_medusa(a14), resist_drunk(a15), resist_chaos(a16), resist_forget(a17),
		fix_critical(a18), fix_strikeback(a19), fix_accurancy(a20), fix_dodge(a21),
		element_earth(a22), element_water(a23), element_fire(a24), element_wind(a25)
	{

	}
	int points_remain;
	int points_endurance;
	int points_strength;
	int points_defense;
	int points_agility;
	int points_magical;
	int value_attack;
	int value_defensive;
	int value_agility;
	int value_spirit;
	int value_recovery;
	int resist_poison;
	int resist_sleep;
	int resist_medusa;
	int resist_drunk;
	int resist_chaos;
	int resist_forget;
	int fix_critical;
	int fix_strikeback;
	int fix_accurancy;
	int fix_dodge;
	int element_earth;
	int element_water;
	int element_fire;
	int element_wind;
}cga_playerpet_detail_info_t;
typedef struct cga_player_info_s
{
	cga_player_info_s()
	{
		hp = 0;
		maxhp = 0;
		mp = 0;
		maxmp = 0;
		xp = 0;
		maxxp = 0;
		health = 0;
		souls = 0;
		level = 0;
		gold = 0;
		unitid = 0;
		petid = -1;
		punchclock = 0;
		usingpunchclock = false;
		manu_endurance = 0;
		manu_skillful = 0;
		manu_intelligence = 0;
		value_charisma = 0;
	}

	cga_player_info_s(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13, int a14, bool a15, std::string &a16, std::string &a17, const cga_playerpet_detail_info_t &a18, int a19, int a20, int a21, int a22)
		: hp(a1), maxhp(a2), mp(a3), maxmp(a4), xp(a5), maxxp(a6), health(a7), souls(a8), level(a9), gold(a10), unitid(a11), petid(a12), direction(a13), punchclock(a14), usingpunchclock(a15), name(a16), job(a17), detail(a18), manu_endurance(a19), manu_skillful(a20), manu_intelligence(a21), value_charisma(a22)
	{

	}

	int hp;
	int maxhp;
	int mp;
	int maxmp;
	int xp;
	int maxxp;
	int health;
	int souls;
	int level;
	int gold;
	int unitid;
	int petid;
	int direction;
	int punchclock;
	bool usingpunchclock;
	std::string name;
	std::string job;
	std::vector<std::string> titles;
	cga_playerpet_detail_info_t detail;
	int manu_endurance;
	int manu_skillful;
	int manu_intelligence;
	int value_charisma;
}cga_player_info_t;
typedef QList<GameItem*> GameItemList;
#define GetGameBaseAddr GameData::getInstance().getGameBaseAddr()
class GameData
{
public:
	static GameData& getInstance();

	void initData();
	void setGameProcess(DWORD pid) { m_gameProcessID = pid; }
	void setGameHwnd(HWND pHwnd) { m_gameHwnd = pHwnd; }
	void setGameHDC(HDC pHdc) { m_gameHDC = pHdc; }
	void setGameBaseAddr(ULONG addr) { m_gameBaseAddr = addr; }

	DWORD getGamePID() { return m_gameProcessID; }
	HWND getGameHwnd() { return m_gameHwnd; }
	HDC getGameHDC() { return m_gameHDC; }
	ULONG getGameBaseAddr() { return m_gameBaseAddr; }
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
	void Ren(const QString& itemName);
	void Ren(int nIndex);
	void Chat(const QString& szText);
	void Renew();
	bool NeedHPSupply(cga_player_info_t &pl);
	bool NeedMPSupply(cga_player_info_t &pl);
	//bool NeedPetSupply(cga_pets_info_t &pets);
	virtual bool ClickNPCDialog(int option, int index);
	cga_player_info_t GetPlayerInfo();
	void(__cdecl *UI_OpenGatherDialog)(int skill_index, int sub_index);
	void(__cdecl *Move_Player)();

private:
	DWORD m_gameProcessID;
	HWND m_gameHwnd;	//游戏窗口句柄
	HDC m_gameHDC;
	HDC m_screenHDC;
	ULONG m_gameBaseAddr;
	//战斗时每个位置的信息状态
	PERSONINFO m_Infos[20];
	QMap<int, int>  m_skillCodeForIndex;	//自定义技能指令代码和实际下标映射
	QMap<int, QString> m_indexForSkillName;	//数组下标对应技能名称
	QStringList m_szPickSkillList;
	XASM m_asm;
	QMap<int, int> m_skillCodeForMyCode;//游戏定义的code和自定义code对应
	QMap<QString, int> m_characterForVal;	//游戏人物状态名称和实际内存值
	int* m_world_status;	//世界状态
	int* m_game_status;//游戏状态
	skill_t *g_skill_base;

};


