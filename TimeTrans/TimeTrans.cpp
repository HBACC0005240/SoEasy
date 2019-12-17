#include "stdafx.h"
#include "TimeTrans.h"
#include <QDateTime>

TimeTrans::TimeTrans(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(doBtnClicked()));
	connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(doBtnClicked()));
	connect(ui.pushButton_curTime,SIGNAL(clicked()),this,SLOT(doBtnClicked()));

}

TimeTrans::~TimeTrans()
{

}
void TimeTrans::doBtnClicked()
{
	QObject* obj=sender();
	if(obj == ui.pushButton)
	{
		QString strSecond=ui.lineEdit_s->text();
		QDateTime dt = QDateTime::fromTime_t(strSecond.toUInt());
		QString strDate = dt.toString(Qt::SystemLocaleLongDate);//日期格
		ui.lineEdit_ymd->setText(strDate);
	}else if(obj == ui.pushButton_2)
	{
		QString strYMD=ui.lineEdit_ymd->text();
		QDateTime dt = QDateTime::fromString(strYMD,Qt::SystemLocaleLongDate);
		QString strDate = QString::number(dt.toTime_t());//日期格
		ui.lineEdit_s->setText(strDate);
	}else if(obj == ui.pushButton_curTime)
	{		
		QString strDate = QDateTime::currentDateTime().toString(Qt::SystemLocaleLongDate);//日期格
		ui.lineEdit_ymd->setText(strDate);
		strDate = QString::number(QDateTime::currentDateTime().toTime_t());//日期格
		ui.lineEdit_s->setText(strDate);
	}
}