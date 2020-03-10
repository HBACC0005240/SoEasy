#pragma once

#include <QWidget>
#include "ui_GameWorkWgt.h"

class GameWorkWgt : public QWidget
{
	Q_OBJECT

public:
	GameWorkWgt(QWidget *parent = Q_NULLPTR);
	~GameWorkWgt();

private:
	Ui::GameWorkWgt ui;
};
