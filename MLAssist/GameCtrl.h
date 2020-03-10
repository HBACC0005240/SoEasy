#pragma once

#include <QObject>
#include <QTimer>
class GameCtrl : public QObject
{
	Q_OBJECT

public:
	GameCtrl( );
	~GameCtrl();
	static GameCtrl* getInstace();

	void StartUpdateTimer();
	void StopUpdateTimer();
signals:
	void signal_activeGameFZ();
	void signal_updateGameStatus();
private:
	QTimer m_updateTimer;		//��ʱ���¶�ʱ��

};
#define g_pGameCtrl GameCtrl::getInstace()
