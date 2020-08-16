#include "GameDataWgt.h"
#include "GameCtrl.h"
#include "GameData.h"
#include "YunLai.h"
#include <QTableWidgetItem>
#include <QLabel>
#include <QDebug>
#include "../include/ITPublic.h"
GameDataWgt::GameDataWgt(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(g_pGameCtrl,SIGNAL(signal_updateGameStatus()),this,SLOT(doUpdateGameData()));
	connect(g_pGameCtrl, SIGNAL(signal_activeGameFZ()), this, SLOT(Active()));

	ui.tableWidget->setRowCount(16);
	for (int i = 0; i < 16; ++i)
	{
		for (size_t n = 0; n < 5; n++)
		{
			QTableWidgetItem* pItem = new QTableWidgetItem();
			ui.tableWidget->setItem(i, n, pItem);
		}
	}
	ui.tableWidget->horizontalHeader()->setStyleSheet("font:bold;");
	//	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->verticalHeader()->setVisible(false);
	//	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->horizontalHeader()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	//	ui.tableWidget->horizontalHeader()->setFixedHeight(30);
	ui.tableWidget->setColumnWidth(1, 130);
	ui.tableWidget->verticalHeader()->setDefaultSectionSize(15);

	//	ui.tableWidget->resizeColumnsToContents();//根据内容调整列宽 但每次都变 太麻烦 修改下
	memset(m_Infos, 0, sizeof(PERSONINFO) * 20);
}

GameDataWgt::~GameDataWgt()
{
}

void GameDataWgt::Active()
{
	QString szLoginUser = YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "00E39EE4", 100);
	QString szGameUserName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "00E35878", 100)));
	QString szMapName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "009181C0", 100)));


	int nEast = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "0092BD28");
	int nSouth = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "0092BD30");
	setItemText(0, 0, szLoginUser);
	setItemText(1, 0, szGameUserName);
	setItemText(2, 0, szMapName);
	setItemText(3, 0, QString("东%1 南%2").arg(nEast).arg(nSouth));
	setItemText(4, 0, QString("连接有效"));
	int nHP = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A418");
	int nTotHP = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A41C");
	int nMP = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A420");
	int nTotMP = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A424");
	QString szHp = QString("HP:%1/%2").arg(nHP).arg(nTotHP);
	QString szMp = QString("MP:%1/%2").arg(nMP).arg(nTotMP);
	qDebug() << szHp << szMp;
	setItemText(0, 1, szHp);
	setItemText(1, 1, szMp);
	//	qDebug() << m_gameProcessID << m_gameHwnd << szEast;
	//获取升级经验 
	int nExperience = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A438");
	int nNextExperience = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A43C");
	setItemText(2, 1, QString("升:%1").arg(nNextExperience - nExperience));
	int nAttack = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A460");
	setItemText(3, 1, QString("力:%1").arg(nAttack));
	int nDefence = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A464");
	setItemText(4, 1, QString("防:%1").arg(nDefence));
	int nAgility = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A468");
	setItemText(5, 1, QString("敏:%1").arg(nAgility));
	int nMind = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A46C");
	setItemText(6, 1, QString("精:%1").arg(nMind));
	int nCharm = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A474");
	setItemText(7, 1, QString("魅:%1").arg(nCharm));

	int nEarth = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A4A4");
	int nWater = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A4A8");
	int nFire = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A4AC");
	int nWind = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A4B0");
	QString szAttribute = "属:";
	if (nEarth > 0)
		szAttribute += QString(" 地%1").arg(nEarth);
	if (nWater > 0)
		szAttribute += QString(" 水%1").arg(nWater);
	if (nFire > 0)
		szAttribute += QString(" 火%1").arg(nFire);
	if (nWind > 0)
		szAttribute += QString(" 风%1").arg(nWind);
	setItemText(8, 1, szAttribute);
	int nGold = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), "00E2A4BC");
	setItemText(9, 1, QString("钱:%1").arg(nGold));

	QMap<int, QString> indexForName;
	for (int i = 0;i < 10;++i)//10个技能栏
	{
		DWORD pAddress = 0x00D84FEC;
		DWORD offset = i * 0x49FC;
		pAddress += offset;
		QString skillName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), pAddress, 100)));
		pAddress += 0x38;
		int nShowIndex = YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), pAddress);
		indexForName.insert(nShowIndex, skillName);
	}
	for (auto it = indexForName.begin();it != indexForName.end();++it)
	{
		setItemText(it.key()-1,2,it.value());
	}
	/*char* skill = YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "00D84FEC", 100);
	QString qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(0, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "00D899E8", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(1, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "D8E3E4", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(2, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "D92DE0", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(3, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "D977DC", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(4, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "D9C1D8", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(5, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "DA0BD4", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(6, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "DA55D0", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(7, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "DA9FCC", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(8, 2, qSkill);
	skill = YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), "DAE9C8", 100);
	qSkill = QString::fromWCharArray(ANSITOUNICODE1(skill));
	setItemText(9, 2, qSkill);*/
}

void GameDataWgt::doUpdateGameData()
{
	POINT gamePoint = GameData::getInstance().GetGamePersonCoordinate();
	setItemText(3, 0, QString("东%1 南%2").arg(gamePoint.x).arg(gamePoint.y));
	//	qDebug() << "更新战斗";
	memset(m_Infos, 0, sizeof(PERSONINFO) * 20);
	GameData::getInstance().readBattleInfo();
	GameData::getInstance().getPersonInfo(m_Infos);
	refreshBattleUI();
	//int nGameStatus = GameData::getInstance().GetGameStatus();
	//int nWordStatus = GameData::getInstance().GetWorldStatus();
	//if(nGameStatus == 1 && nWordStatus  == 2)
	//	setItemText(4, 0, QString("连接断开"));
	//else
	//	setItemText(4, 0, QString("连接有效"));

//	return;
	POINT pi;
	::GetCursorPos(&pi);

	//DWORD srcColorVal = GetPixel(m_screenHDC, pi.x, pi.y);
//	qDebug() << "屏幕坐标 颜色" << pi.x << pi.y << srcColorVal;
	::ScreenToClient(GameData::getInstance().getGameHwnd(), &pi);
	DWORD colorVal = GetPixel(GameData::getInstance().getGameHDC(), 6, 21);//pi.x, pi.y);
	QColor newColor = QColor(GetRValue(colorVal), GetGValue(colorVal), GetBValue(colorVal));

	qDebug() << "游戏坐标 颜色" << pi.x << pi.y << colorVal << GetRValue(colorVal) << GetGValue(colorVal) << GetBValue(colorVal)<< newColor.name();
	/*DWORD capColor = YunLai::GetScreenColorCapture(pi.x,pi.y,m_gameHwnd);
	qDebug() << "截图取色 颜色" << pi.x << pi.y << capColor;*/
	
}
void GameDataWgt::setItemText(int row, int col, const QString& szText)
{
	QTableWidgetItem* pItem = ui.tableWidget->item(row, col);
	if(pItem)
		pItem->setText(szText);
}

void GameDataWgt::refreshBattleUI()
{
	for (int i = 0; i < 20; i++)
	{
		PERSONINFO info = m_Infos[i];
		QString szLabelName = QString("label_%1").arg(i);
		QLabel* pLabel = this->findChild<QLabel *>(szLabelName);
		if (pLabel)
		{
			WCHAR buf[MAXBUFLEN] = { 0 };
			if (wcslen(info.name) > 0)//
			{
				QString szShowText = QString("%1\n%2/%3\n%4").arg(QString::fromWCharArray(info.name)).arg(info.hp).arg(info.totalhp).arg(info.level);
				//			qDebug() << szShowText << info.name << info.hp << info.totalhp << info.level;
				pLabel->setText(szShowText);
			}
			else
			{
				pLabel->setText("");
			}
		}
	}
}
