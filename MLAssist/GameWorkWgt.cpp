#include "GameWorkWgt.h"
#include "stdafx.h"
//#include "gameservice.h"
GameWorkWgt::GameWorkWgt(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(g_pGameCtrl, SIGNAL(signal_activeGameFZ()), this, SLOT(Active()));
	connect(g_pGameCtrl, SIGNAL(signal_updateGameStatus()), this, SLOT(doUpdateGameData()));

	m_szPickSkillList <<"鉴定"<< "挖矿" << "伐木" << "狩猎" <<"修理武器"<<"修理防具"<< "挖矿体验" << "伐木体验" << "狩猎体验";
	
}

GameWorkWgt::~GameWorkWgt()
{
}

void GameWorkWgt::initSkillComboBox()
{
	ui.comboBox_skill->clear();
	//筛选 采集：挖矿、伐木、狩猎、挖矿体验、伐木体验、狩猎体验
	QMap<int, QString> indexForName;
	for (int i=0;i<15;++i)//先10个  15个技能栏
	{
		DWORD pAddress = 0x00D84FEC;
		DWORD offset = i * 0x49FC;
		pAddress += offset;
		QString skillName = QString::fromWCharArray(ANSITOUNICODE1(YunLai::ReadMemoryStrFromProcessID(GameData::getInstance().getGamePID(), pAddress, 100)));
		pAddress += 0x38;
		int nShowIndex = i;//YunLai::ReadMemoryIntFromProcessID(GameData::getInstance().getGamePID(), pAddress);
		if (m_szPickSkillList.contains(skillName))
		{
			indexForName.insert(nShowIndex, skillName);
		}
	}
	for (auto it=indexForName.begin();it!=indexForName.end();++it )
	{
		ui.comboBox_skill->addItem(it.value(),it.key());
	}

}

void GameWorkWgt::on_pushButton_useSkill_clicked()
{
	QString skillName = ui.comboBox_skill->currentText();
	int nCurIndex = ui.comboBox_skill->itemData(ui.comboBox_skill->currentIndex()).toInt();
//	SendMessageA(GameData::getInstance().getGameHwnd(), WM_CGA_START_WORK, nCurIndex, 0);
//	GameData::getInstance().UI_OpenGatherDialog(nCurIndex, 0);
//
//	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GameData::getInstance().getGamePID());	
//	int nTestArg[2];
//	nTestArg[0] = nCurIndex;
//	nTestArg[1] = 0;
//	LPVOID callArg = VirtualAllocEx(hProcess, nullptr, sizeof(nTestArg), MEM_COMMIT /*| MEM_RESERVE*/, PAGE_READWRITE);
//	DWORD byWrite;
//	bool bRet = WriteProcessMemory(hProcess, callArg, nTestArg, sizeof(nTestArg), &byWrite);
//	if (bRet == false)
//	{
//	//	std::cout << "erorCode:" << GetLastError() << "提示：写入代码失败\n";
//		VirtualFreeEx(hProcess, callArg, sizeof(nTestArg), MEM_DECOMMIT);
//		CloseHandle(hProcess);
//		return;
//	}
//	DWORD tId;
//	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GameData::getInstance().UI_OpenGatherDialog, callArg, 0, &tId);
//	if (hRemoteThread == nullptr)
//	{
//		printf("提示：远程调用代码失败\n");
//	//	VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
//		CloseHandle(hRemoteThread);
//		CloseHandle(hProcess);
//		return;
//	}
//	WaitForSingleObject(hRemoteThread, 0xFFFFFFF);//等待 ...
////	VirtualFreeEx(hProcess, callBase, nThreadSize, MEM_DECOMMIT);
//	CloseHandle(hRemoteThread);
//	CloseHandle(hProcess);

	GameData::getInstance().Work(skillName);
}

void GameWorkWgt::Active()
{
	initSkillComboBox();
}

void GameWorkWgt::doUpdateGameData()
{
	g_pGameCtrl->ReadItemData();
}
