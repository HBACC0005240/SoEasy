#include "GameWorkWgt.h"
#include "stdafx.h"
GameWorkWgt::GameWorkWgt(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(g_pGameCtrl, SIGNAL(signal_activeGameFZ()), this, SLOT(Active()));
	m_szPickSkillList <<"����"<< "�ڿ�" << "��ľ" << "����" <<"��������"<<"�������"<< "�ڿ�����" << "��ľ����" << "��������";
	
}

GameWorkWgt::~GameWorkWgt()
{
}

void GameWorkWgt::initSkillComboBox()
{
	ui.comboBox_skill->clear();
	//ɸѡ �ɼ����ڿ󡢷�ľ�����ԡ��ڿ����顢��ľ���顢��������
	QMap<int, QString> indexForName;
	for (int i=0;i<15;++i)//��10��  15��������
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
	GameData::getInstance().Work(skillName);
}

void GameWorkWgt::Active()
{
	initSkillComboBox();
}
