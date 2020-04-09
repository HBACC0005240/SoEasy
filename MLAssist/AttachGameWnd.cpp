#include "AttachGameWnd.h"
#include <QMap>
#include "YunLai.h"
#include "GameData.h"
#include <QListWidgetItem>
#include "GameCtrl.h"
AttachGameWnd::AttachGameWnd(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

AttachGameWnd::~AttachGameWnd()
{
}

void AttachGameWnd::on_pushButton_attach_clicked()
{
	QListWidgetItem* pCurItem = ui.listWidget->currentItem();
	if (pCurItem == nullptr)
		return;
	qint64 gameProcessID = pCurItem->data(Qt::UserRole).toInt();//ui.comboBox->currentData(Qt::UserRole).toInt();
	HWND gameHwnd = YunLai::FindMainWindow(gameProcessID);
	HDC gameHDC = GetDC(gameHwnd);
	GameData::getInstance().setGameProcess(gameProcessID);
	GameData::getInstance().setGameHwnd(gameHwnd);
	GameData::getInstance().setGameHDC(gameHDC);
	GameData::getInstance().initData();
	emit g_pGameCtrl->signal_activeGameFZ();
	g_pGameCtrl->StartUpdateTimer();
}

void AttachGameWnd::on_pushButton_refresh_clicked()
{
	ui.listWidget->clear();
	QMap<qint64, QString> processData;
	YunLai::GetAllProcess(processData);
	for (auto it = processData.begin(); it != processData.end(); ++it)
	{
		if (it.value().contains("qfmoli"))
		{
			QString szLoginUser = YunLai::ReadMemoryStrFromProcessID(it.key(), "00E39EE4", 100);
			QListWidgetItem* pItem = new QListWidgetItem(QString("%1 %2 %3").arg(it.key()).arg(it.value()).arg(szLoginUser));
			pItem->setData(Qt::UserRole, it.key());
			ui.listWidget->addItem(pItem);
		}
	}
}

void AttachGameWnd::on_pushButton_unAttach_clicked()
{
	GameData::getInstance().JianDongXi(MOVE_DIRECTION_LEFT);
}

void AttachGameWnd::on_pushButton_login_clicked()
{
	GameData::getInstance().JianDongXi(MOVE_DIRECTION_LEFTDOWN);
}
