#pragma once

#include <QWidget>
#include "ui_GameWorkWgt.h"

class GameWorkWgt : public QWidget
{
	Q_OBJECT

public:
	GameWorkWgt(QWidget *parent = Q_NULLPTR);
	~GameWorkWgt();
	
	void initSkillComboBox();
public slots:
	void on_pushButton_useSkill_clicked();
	void Active();
private:
	Ui::GameWorkWgt ui;
	QStringList m_szPickSkillList;

};
