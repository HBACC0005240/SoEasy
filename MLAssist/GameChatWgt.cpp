#include "GameChatWgt.h"
#include "GameCtrl.h"
#include "../include/ITPublic.h"
#include "YunLai.h"
#include "GameData.h"
#include <QDateTime>
#include <QScrollBar>
GameChatWgt::GameChatWgt(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(g_pGameCtrl, SIGNAL(signal_updateGameStatus()), this, SLOT(doUpdateGameChat()));
	m_nLastIndex = 0;

	QStringList saveHeadList;
	saveHeadList << "时间" << "内容";
	ui.tableWidget->setColumnCount(2);
	ui.tableWidget->setHorizontalHeaderLabels(saveHeadList);
//	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->verticalHeader()->setVisible(false);
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->horizontalHeader()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget->horizontalHeader()->setFixedHeight(30);
	ui.tableWidget->setColumnWidth(0, 60);
	//ui.tableWidget->setColumnWidth(3, 1 * iWidth);
	ui.tableWidget->verticalHeader()->setDefaultSectionSize(15);
}

GameChatWgt::~GameChatWgt()
{
}

void GameChatWgt::addOneChatData(const QString& chatData)
{
	int nRowCount = ui.tableWidget->rowCount();
	ui.tableWidget->setRowCount(nRowCount+1);
	QTableWidgetItem* item1 = new QTableWidgetItem(QDateTime::currentDateTime().toString("hh:mm:ss"));
	item1->setTextAlignment(Qt::AlignLeft);
	ui.tableWidget->setItem(nRowCount, 0, item1);

	QTableWidgetItem* item2 = new QTableWidgetItem(chatData);
	item2->setTextAlignment(Qt::AlignLeft);
	ui.tableWidget->setItem(nRowCount, 1, item2);

}

void GameChatWgt::doUpdateGameChat()
{
	//聊天基址005A91C4  差值 134
	//005A7014基址 005D218C 数组索引
	int nCurChatIndex = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "005D218C");
	//比对索引 当前是2说明有1条数据  3有2条
	if (m_nLastIndex != (nCurChatIndex-1))//说明有新的聊天记录 
	{
		for (int i = m_nLastIndex;i < (nCurChatIndex-1);++i)
		{
			DWORD pAddress = 0x005A7014;
			DWORD offset = i * 0x134;
			pAddress += offset;
			QString chatData = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), pAddress, 300)));
			if (!chatData.isEmpty())//不为空 记录时间  2秒定时器 只能这样来记录大概时间了
			{
			//	ui.textEdit->append(QString("%1 %2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd")).arg(chatData));
				addOneChatData(chatData);
			}
		}
		ui.tableWidget->verticalScrollBar()->setSliderPosition(ui.tableWidget->verticalScrollBar()->maximum());
	}
	m_nLastIndex = (nCurChatIndex-1);
}
