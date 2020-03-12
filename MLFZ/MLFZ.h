#pragma once

#include <QtWidgets/QWidget>
#include "ui_MLFZ.h"

class MLFZ : public QWidget
{
	Q_OBJECT

public:
	MLFZ(QWidget *parent = Q_NULLPTR);

private:
	Ui::MLFZClass ui;
};
