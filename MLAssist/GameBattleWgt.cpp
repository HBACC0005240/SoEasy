#include "GameBattleWgt.h"
#include <QFileDialog>
#include "GameCtrl.h"
GameBattleWgt::GameBattleWgt(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

GameBattleWgt::~GameBattleWgt()
{
}

void GameBattleWgt::on_pushButton_loadCfg_clicked()
{
	QString path = QFileDialog::getOpenFileName(this, "ѡ������", "./", "*.save");
	if (path.isEmpty())
		return;
	g_pGameCtrl->LoadConfig(path);
}

void GameBattleWgt::on_pushButton_saveCfg_clicked()
{
	QString path = QFileDialog::getOpenFileName(this, "��������", "./", "*.save");
	if (path.isEmpty())
		return;
	g_pGameCtrl->SaveConfig(path);
}
