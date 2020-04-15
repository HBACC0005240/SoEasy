#pragma once

#include <QWidget>
#include "ui_GameBattleSetWgt.h"

class GameBattleSetWgt : public QWidget
{
	Q_OBJECT

public:
	GameBattleSetWgt(QWidget *parent = Q_NULLPTR);
	~GameBattleSetWgt();
	
	void initRenItems(QHash<QString,int> itemHash);
	void initDieItems(QHash<QString, int> itemHash);
public slots:
	void on_pushButton_renAdd_clicked();
	void on_pushButton_dieAdd_clicked();
	void doLoadUserConfig();
	void doUpdateGameData();
	void doRenItemChanged(QListWidgetItem *item);
private:
	Ui::GameBattleSetWgt ui;

	
};
