#pragma once

#include <QWidget>
#include "ui_GameChatWgt.h"

class GameChatWgt : public QWidget
{
	Q_OBJECT

public:
	GameChatWgt(QWidget *parent = Q_NULLPTR);
	~GameChatWgt();

private:
	Ui::GameChatWgt ui;
};
