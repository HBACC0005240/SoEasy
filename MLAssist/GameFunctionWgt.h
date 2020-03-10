#pragma once

#include <QWidget>
#include "ui_GameFunctionWgt.h"

class GameFunctionWgt : public QWidget
{
	Q_OBJECT

public:
	GameFunctionWgt(QWidget *parent = Q_NULLPTR);
	~GameFunctionWgt();

private:
	Ui::GameFunctionWgt ui;
};
