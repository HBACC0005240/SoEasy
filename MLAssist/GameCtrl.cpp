#include "GameCtrl.h"
GameCtrl::GameCtrl( )
{
	connect(&m_updateTimer, SIGNAL(timeout()), this, SIGNAL(signal_updateGameStatus()));
}

GameCtrl::~GameCtrl()
{
}

GameCtrl* GameCtrl::getInstace()
{
	static GameCtrl pGameCtrl;
	return &pGameCtrl;
}

void GameCtrl::StartUpdateTimer()
{
	m_updateTimer.start(2000);	//2秒获取一次战斗信息
}

void GameCtrl::StopUpdateTimer()
{
	m_updateTimer.stop();
}
