#include "GameCtrl.h"
#include <QSettings>
#include <QTextCodec>
#include "YunLai.h"
#include "../include/ITPublic.h"

GameCtrl::GameCtrl( )
{
	connect(&m_updateTimer, SIGNAL(timeout()), this, SIGNAL(signal_updateGameStatus()));
	for (size_t i = 0; i < 28; i++)
	{
		GameItem* pItem = new GameItem;
		m_gameItems.append(pItem);
	}
}

GameCtrl::~GameCtrl()
{
}

GameCtrl* GameCtrl::getInstace()
{
	static GameCtrl pGameCtrl;
	return &pGameCtrl;
}

void GameCtrl::LoadConfig(const QString& path)
{
	if (path.isEmpty())
		return;
	QSettings iniFile(path, QSettings::IniFormat);
	iniFile.setIniCodec(QTextCodec::codecForName("GB2312"));     //这样分组下的键值可以读取中文  下面的是读取段的中文

	m_renItemHash.clear();
	int renCount = iniFile.value("ren/count", 0).toInt();
	for (int i=1;i<=renCount;++i)
	{
		QString itemKeyName = QString("item%1").arg(i);
		QString itemKeyVal = QString("checked%1").arg(i);
		QString itemName = iniFile.value(QString("ren/%1").arg(itemKeyName), "").toString();
		int itemVal = iniFile.value(QString("ren/%1").arg(itemKeyVal), "").toInt();
		m_renItemHash.insert(itemName, itemVal);
	}

	m_dieItemHash.clear();
	int dieCount = iniFile.value("die/count", 0).toInt();
	for (int i = 1;i <= dieCount;++i)
	{
		QString itemKeyName = QString("item%1").arg(i);
		QString itemKeyVal = QString("checked%1").arg(i);
		QString itemName = iniFile.value(QString("die/%1").arg(itemKeyName), "").toString();
		int itemVal = iniFile.value(QString("die/%1").arg(itemKeyVal), "").toInt();
		m_dieItemHash.insert(itemName, itemVal);
	}

	emit signal_loadUserConfig();
}

void GameCtrl::SaveConfig(const QString& path)
{

}

void GameCtrl::StartUpdateTimer()
{
	m_updateTimer.start(2000);	//2秒获取一次战斗信息

}

void GameCtrl::StopUpdateTimer()
{
	m_updateTimer.stop();
}

void GameCtrl::setRenItemIsChecked(const QString& name, int nVal)
{
	if (m_renItemHash.contains(name))
		m_renItemHash[name] = nVal;
	else
		m_renItemHash[name] = nVal;
}

void GameCtrl::setDieItemIsChecked(const QString& name, int nVal)
{
	if (m_dieItemHash.contains(name))
		m_dieItemHash[name] = nVal;
	else
		m_dieItemHash[name] = nVal;
}

void GameCtrl::RenItems()
{
	for (auto it = m_renItemHash.begin();it != m_renItemHash.end();++it)
	{
		if (it.value() == 1)
		{
			GameItem* pItem = m_itemHash.value(it.key());
			if(pItem)	
				GameData::getInstance().Ren(pItem->index);
		}
	}
}

void GameCtrl::dieItems()
{

}

void GameCtrl::ReadItemData()
{
	m_itemHash.clear();
	for (size_t i = 0; i < 28; i++)
	{
		GameItem* pItem = m_gameItems.at(i);

		DWORD pAddress = 0x00E2A566;
		DWORD offset = i * 0x65C;
		pAddress += offset;
		DWORD pExistItemAdd = pAddress - 0x2;
		DWORD pItemCodeAddr = pAddress + 0x63E;
		DWORD pItemItemCount = pItemCodeAddr + 0x4;
		int isExist = YunLai::ReadMemoryWordFromProcessID(GameData::getInstance().getGamePID(), pExistItemAdd);
		QString itemName;
		int nCount = 0;
		int itemCode = 0;
		if (isExist)
		{
			itemName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), pAddress, 100)));
			nCount=YunLai::ReadMemoryWordFromProcessID(GameData::getInstance().getGamePID(), pItemItemCount);
			itemCode = YunLai::ReadMemoryWordFromProcessID(GameData::getInstance().getGamePID(), pItemCodeAddr);
			m_itemHash.insert(itemName, pItem);

		}
		else
			itemName = "";
		pItem->itemName = itemName;
		pItem->itemId = QString::number(itemCode);
		pItem->itemCount = nCount;
		pItem->index = i;
	}
}
