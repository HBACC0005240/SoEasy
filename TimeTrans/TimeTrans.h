#ifndef TIMETRANS_H
#define TIMETRANS_H

#include <QtWidgets/QMainWindow>
#include "ui_TimeTrans.h"

class TimeTrans : public QMainWindow
{
	Q_OBJECT

public:
	TimeTrans(QWidget *parent = 0);
	~TimeTrans();

public slots:
	void doBtnClicked();
private:
	Ui::TimeTransClass ui;
};

#endif // TIMETRANS_H
