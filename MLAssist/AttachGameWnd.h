#pragma once

#include <QWidget>
#include "ui_AttachGameWnd.h"
//╧р╫ссно╥
class AttachGameWnd : public QWidget
{
	Q_OBJECT

public:
	AttachGameWnd(QWidget *parent = Q_NULLPTR);
	~AttachGameWnd();

public slots:
	void on_pushButton_attach_clicked();
	void on_pushButton_refresh_clicked();
	void on_pushButton_unAttach_clicked();
	void on_pushButton_login_clicked();
private:
	Ui::AttachGameWnd ui;
};
