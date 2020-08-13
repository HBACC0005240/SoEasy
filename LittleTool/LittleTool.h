#pragma once

#include <QtWidgets/QDialog>
#include "ui_LittleTool.h"

class LittleTool : public QDialog
{
    Q_OBJECT

public:
    LittleTool(QWidget *parent = Q_NULLPTR);
	~LittleTool();
	void init();
	void startMonitor();
	void stopMonitor();
public slots:
	void doComboBoxChanged(int nIndex);
	void doLineEditChanged(const QString& text);

private:
    Ui::LittleToolClass ui;
};
