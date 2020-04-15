#pragma once

#include <QWidget>
#include "ui_GameBattleWgt.h"

class GameBattleWgt : public QWidget
{
	Q_OBJECT

public:
	GameBattleWgt(QWidget *parent = Q_NULLPTR);
	~GameBattleWgt();

public slots:
	void on_pushButton_loadCfg_clicked();
	void on_pushButton_saveCfg_clicked();
private:
	Ui::GameBattleWgt ui;
};
