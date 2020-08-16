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
	void initHotKey();
	void startMonitor();
	void stopMonitor();
public slots:
	void doComboBoxChanged(int nIndex);
	void doLineEditChanged(const QString& text);
	void dealHotKeyEvent(const QKeySequence& key);
private:
    Ui::LittleToolClass ui;
};
