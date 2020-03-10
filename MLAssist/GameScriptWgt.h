#pragma once

#include <QWidget>
#include "ui_GameScriptWgt.h"

class GameScriptWgt : public QWidget
{
	Q_OBJECT

public:
	GameScriptWgt(QWidget *parent = Q_NULLPTR);
	~GameScriptWgt();

	void initTableWidget();
public slots:
	void on_pushButton_pause_clicked();
	void on_pushButton_open_clicked();
	void on_pushButton_openEncrypt_clicked();
	void on_pushButton_start_clicked();
	void on_pushButton_stop_clicked();
private:
	Ui::GameScriptWgt ui;
};
