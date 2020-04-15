#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_AsmManual.h"

class AsmManual : public QMainWindow
{
	Q_OBJECT

public:
	AsmManual(QWidget *parent = Q_NULLPTR);

	void init();
public slots:
	void on_pushButton_clicked();
	void on_pushButton_add_clicked();
	void on_pushButton_edit_clicked();
	void on_listWidget_customContextMenuRequested(const QPoint & pos);
	void doDelItem();
	void doLineEditEnter();
	void doAsmFilter(const QString& szFilter);
	void doListWidgetItemClicked(QListWidgetItem* pItem);
private:
	Ui::AsmManualClass ui;
	QHash<QString, QListWidgetItem*> m_asmMapItem;	//指令名称和item对应
};
