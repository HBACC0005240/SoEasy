#pragma once

#include <QWidget>
#include "ui_GameBattleWgt.h"

class GameBattleWgt : public QWidget
{
	Q_OBJECT

public:
	GameBattleWgt(QWidget *parent = Q_NULLPTR);
	~GameBattleWgt();

private:
	Ui::GameBattleWgt ui;
};
