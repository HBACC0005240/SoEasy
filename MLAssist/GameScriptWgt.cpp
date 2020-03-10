#include "stdafx.h"
#include "GameScriptWgt.h"
#include <QFileDialog>
#include "FZParseScript.h"
#include <QTableWidgetItem>
GameScriptWgt::GameScriptWgt(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QStringList saveHeadList;
	saveHeadList << ("序号") << ("脚本内容");
	ui.tableWidget->setColumnCount(saveHeadList.size());
	ui.tableWidget->setHorizontalHeaderLabels(saveHeadList);
	ui.tableWidget->horizontalHeader()->setStyleSheet("font:bold;");
	//	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->verticalHeader()->setVisible(false);
	//	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->horizontalHeader()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	//	ui.tableWidget->horizontalHeader()->setFixedHeight(30);
	ui.tableWidget->setColumnWidth(0, 30);

}

GameScriptWgt::~GameScriptWgt()
{
}

void GameScriptWgt::initTableWidget()
{
	QStringList szScripts = FZParseScript::getInstance().GetGameScriptData();
	int rowCounts = ui.tableWidget->rowCount();
	int colCounts = ui.tableWidget->columnCount();
	int nWidgetCol = -1;
	for (int i = 0; i < rowCounts - 1; i++)
	{
		for (int j = 0; j < colCounts; j++)
		{
			QTableWidgetItem* tableitem = ui.tableWidget->item(i, j);
			if (tableitem != NULL)
				delete tableitem;
		}	
		ui.tableWidget->removeRow(0);
	}
	ui.tableWidget->clearContents();
	ui.tableWidget->setRowCount(szScripts.size());
	for (int i=0;i<szScripts.size();++i)
	{
		QTableWidgetItem* pItem = new QTableWidgetItem(QString::number(i+1));
		ui.tableWidget->setItem(i, 0, pItem);
		pItem = new QTableWidgetItem(szScripts.at(i));
		ui.tableWidget->setItem(i, 1, pItem);
	}
}

void GameScriptWgt::on_pushButton_pause_clicked()
{
	if(FZParseScript::getInstance().GetGameScriptCtrlStatus()== FZParseScript::SCRIPT_CTRL_PAUSE)
		FZParseScript::getInstance().PauseScript();
	else
		FZParseScript::getInstance().RunScript();

}

void GameScriptWgt::on_pushButton_open_clicked()
{
	QString szPath = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("打开"), "./", "*.script");
	if (szPath.isEmpty())
		return;
	FZParseScript::getInstance().ParseGameScript(szPath);
	ui.textEdit->setText(FZParseScript::getInstance().getGameDescript());
	initTableWidget();
}

void GameScriptWgt::on_pushButton_openEncrypt_clicked()
{
	QString szPath = QFileDialog::getOpenFileName(this, QString::fromLatin1("打开"), "./", "*.script");
	if (szPath.isEmpty())
		return;
	FZParseScript::getInstance().ParseGameScript(szPath);
}

void GameScriptWgt::on_pushButton_start_clicked()
{
	FZParseScript::getInstance().RunScript();
}

void GameScriptWgt::on_pushButton_stop_clicked()
{
	FZParseScript::getInstance().StopScript();
}
