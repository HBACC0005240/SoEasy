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
	QTimer m_updateTimer;		//��ʱ���¶�ʱ��
	QHash<QString, int> m_renItemHash;//����Ʒ
	QHash<QString, int> m_dieItemHash;//����Ʒ

	GameItemList m_gameItems;	//������Ʒ��Ϣ  0-7������� 8-27���� 5���� 0ͷ 6���� 2�� 3�� 1�� 4�� 7ˮ��
	QHash<QString, GameItem*> m_itemHash;

};
#define g_pGameCtrl GameCtrl::getInstace()
