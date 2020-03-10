#pragma once

#include <QWidget>
#include "ui_GameDataWgt.h"
#include "GameData.h"
class GameDataWgt : public QWidget
{
	Q_OBJECT

public:
	GameDataWgt(QWidget *parent = Q_NULLPTR);
	~GameDataWgt();

	void setItemText(int row, int col, const QString& szText);
	void refreshBattleUI();
public slots:
	void doUpdateGameData();
	void Active();

private:
	Ui::GameDataWgt ui;
	PERSONINFO m_Infos[20];

};
