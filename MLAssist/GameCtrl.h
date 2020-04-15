#pragma once

#include <QObject>
#include <QTimer>
#include <QHash>
#include "GameData.h"
class GameCtrl : public QObject
{
	Q_OBJECT

public:
	GameCtrl( );
	~GameCtrl();
	static GameCtrl* getInstace();

	void LoadConfig(const QString& path);
	void SaveConfig(const QString& path);
	void StartUpdateTimer();
	void StopUpdateTimer();

	QHash<QString, int> GetRenItemHash() { return m_renItemHash; }
	QHash<QString, int> GetDieItemHash() { return m_dieItemHash; }
	void setRenItemIsChecked(const QString& name, int nVal);
	void setDieItemIsChecked(const QString& name, int nVal);
	void RenItems();
	void dieItems();
	void ReadItemData();
signals:
	void signal_activeGameFZ();
	void signal_updateGameStatus();
	void signal_loadUserConfig();
private:
	QTimer m_updateTimer;		//定时更新定时器
	QHash<QString, int> m_renItemHash;//扔物品
	QHash<QString, int> m_dieItemHash;//叠物品

	GameItemList m_gameItems;	//人物物品信息  0-7人物佩戴 8-27包裹 5左饰 0头 6右饰 2手 3手 1身 4脚 7水晶
	QHash<QString, GameItem*> m_itemHash;

};
#define g_pGameCtrl GameCtrl::getInstace()
