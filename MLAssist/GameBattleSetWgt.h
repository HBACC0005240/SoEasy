#pragma once

#include <QWidget>
#include "ui_GameBattleSetWgt.h"

class GameBattleSetWgt : public QWidget
{
	Q_OBJECT

public:
	GameBattleSetWgt(QWidget *parent = Q_NULLPTR);
	~GameBattleSetWgt();

private:
	Ui::GameBattleSetWgt ui;
};
