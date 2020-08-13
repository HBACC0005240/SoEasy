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
	void doComboBoxChanged2(int nIndex);
	void doComboBoxChanged3(int nIndex);
	void doComboBoxChanged4(int nIndex);
	void doComboBoxChanged5(int nIndex);
	void doComboBoxChanged6(int nIndex);

private:
    Ui::LittleToolClass ui;
};
