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
	QTimer m_updateTimer;		//定时更新定时器

};
#define g_pGameCtrl GameCtrl::getInstace()
