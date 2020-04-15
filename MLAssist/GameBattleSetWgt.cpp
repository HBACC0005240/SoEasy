#include "GameBattleSetWgt.h"
#include "GameCtrl.h"
#include "GameData.h"
GameBattleSetWgt::GameBattleSetWgt(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(g_pGameCtrl,SIGNAL(signal_loadUserConfig()),this,SLOT(doLoadUserConfig()));
	connect(g_pGameCtrl, SIGNAL(signal_updateGameStatus()), this, SLOT(doUpdateGameData()));
	connect(g_pGameCtrl, SIGNAL(signal_activeGameFZ()), this, SLOT(Active()));
	bool bRet=connect(ui.listWidget_ren,SIGNAL(itemChanged(QListWidgetItem *)),this,SLOT(doRenItemChanged(QListWidgetItem *)));
	bRet = false;
}

GameBattleSetWgt::~GameBattleSetWgt()
{

}


void GameBattleSetWgt::initRenItems(QHash<QString, int> itemHash)
{
	ui.listWidget_ren->clear();
	for (auto it=itemHash.begin();it!=itemHash.end();++it)
	{
		QListWidgetItem* pItem = new QListWidgetItem(it.key());
		if(it.value()==1)
			pItem->setCheckState(Qt::Checked);
		else
			pItem->setCheckState(Qt::Unchecked);
		ui.listWidget_ren->addItem(pItem);
	}
}
void GameBattleSetWgt::initDieItems(QHash<QString, int> itemHash)
{
	ui.listWidget_die->clear();
	for (auto it = itemHash.begin();it != itemHash.end();++it)
	{
		QListWidgetItem* pItem = new QListWidgetItem(it.key());
		if (it.value() == 1)
			pItem->setCheckState(Qt::Checked);
		else
			pItem->setCheckState(Qt::Unchecked);
		ui.listWidget_die->addItem(pItem);
	}
}
void GameBattleSetWgt::on_pushButton_renAdd_clicked()
{
	QString renText = ui.lineEdit_renAdd->text();
	QListWidgetItem* pItem = new QListWidgetItem(renText);
	pItem->setCheckState(Qt::Unchecked);
	ui.listWidget_ren->addItem(pItem);
}

void GameBattleSetWgt::on_pushButton_dieAdd_clicked()
{
	QString dieText = ui.lineEdit_dieAdd->text();
	QListWidgetItem* pItem = new QListWidgetItem(dieText);
	pItem->setCheckState(Qt::Unchecked);
	ui.listWidget_die->addItem(pItem);
}

void GameBattleSetWgt::doLoadUserConfig()
{
	initRenItems(g_pGameCtrl->GetRenItemHash());
	initDieItems(g_pGameCtrl->GetDieItemHash());

}

void GameBattleSetWgt::doUpdateGameData()
{
	g_pGameCtrl->RenItems();
	g_pGameCtrl->dieItems();
}

void GameBattleSetWgt::doRenItemChanged(QListWidgetItem *item)
{
	if (item == nullptr)
		return;
	QString name = item->text();
	if(item->checkState() == Qt::Checked)
		g_pGameCtrl->setRenItemIsChecked(name,1);
	else
		g_pGameCtrl->setRenItemIsChecked(name, 0);
}
