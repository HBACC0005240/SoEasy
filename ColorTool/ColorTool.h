#pragma once

#include <QtWidgets/QWidget>
#include "ui_ColorTool.h"

class ColorTool : public QWidget
{
	Q_OBJECT

public:
	ColorTool(QWidget *parent = Q_NULLPTR);

private:
	Ui::ColorToolClass ui;
};
