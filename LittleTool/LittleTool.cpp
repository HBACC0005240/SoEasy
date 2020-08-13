#include "LittleTool.h"
#include <windows.h>
#include <QDebug>
static HHOOK hookid = 0;
static HHOOK g_hHook = 0;
static int g_curOneKey;
static int g_userSetKey[5];

static QHash<void*, int> g_comboBoxIndex;

LittleTool::LittleTool(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() &~Qt::WindowContextHelpButtonHint);     //ȥ������
	connect(ui.comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(doComboBoxChanged(int)));
	init();
	startMonitor();
}

LittleTool::~LittleTool()
{
	stopMonitor();
}

void LittleTool::init()
{
	ui.comboBox->clear();
	ui.comboBox->addItem(QStringLiteral("Esc"),VK_ESCAPE);
	for (int i = 0; i < 8; ++i)
	{
		QString itemName = QStringLiteral("F%1").arg(i + 1);
		int nVal = VK_F1 + i;
		ui.comboBox->addItem(itemName, nVal);
	}
	connect(ui.comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(doComboBoxChanged(int)));
	connect(ui.comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(doComboBoxChanged(int)));
	connect(ui.comboBox_4, SIGNAL(currentIndexChanged(int)), this, SLOT(doComboBoxChanged(int)));
	connect(ui.comboBox_5, SIGNAL(currentIndexChanged(int)), this, SLOT(doComboBoxChanged(int)));
	connect(ui.comboBox_6, SIGNAL(currentIndexChanged(int)), this, SLOT(doComboBoxChanged(int)));


	ui.comboBox_2->addItem(QStringLiteral("��"), 0);
	ui.comboBox_3->addItem(QStringLiteral("��"), 0);
	ui.comboBox_4->addItem(QStringLiteral("��"), 0);
	ui.comboBox_5->addItem(QStringLiteral("��"), 0);
	ui.comboBox_6->addItem(QStringLiteral("��"), 0);
	for (int i = 0; i < 8; ++i)
	{
		QString itemName = QStringLiteral("F%1").arg(i + 1);
		int nVal = VK_F1 + i;
		ui.comboBox_2->addItem(itemName, nVal);
		ui.comboBox_3->addItem(itemName, nVal);
		ui.comboBox_4->addItem(itemName, nVal);
		ui.comboBox_5->addItem(itemName, nVal);
		ui.comboBox_6->addItem(itemName, nVal);
	}
	g_curOneKey = ui.comboBox->currentData().toInt();
	ui.comboBox_2->setCurrentIndex(6);
	ui.comboBox_3->setCurrentIndex(7);
	ui.comboBox_4->setCurrentIndex(8);

}

// Hook callback   ���ӻص�����
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode != HC_ACTION)//if (nCode < 0 || nCode == HC_NOREMOVE)
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	LPKBDLLHOOKSTRUCT pKeyBoardHook = (KBDLLHOOKSTRUCT  FAR*)lParam;
	if (wParam == WM_KEYDOWN /*&& wParam <= WM_MBUTTONDBLCLK*/)
	{
		if ((pKeyBoardHook->vkCode == g_curOneKey))
		{
			//if (GetKeyState(VK_F4) < 0)	// if (pkbhs.vkCode=VK_ESCAPE) and  (GetAsyncKeyState(VK_CONTROL)<0) then  ͬʱ�ж�ctrl
			//{

			//}
		/*	mouse_event(MOUSEEVENTF_RIGHTDOWN, lpPoint.x, lpPoint.y, 0, 0);
			mouse_event(MOUSEEVENTF_RIGHTUP, lpPoint.x, lpPoint.y, 0, 0);*///���
			int nUserKey = 0;
			for (auto it= g_comboBoxIndex.begin();it!= g_comboBoxIndex.end();++it)
			{
				nUserKey = it.value();
				if (nUserKey != 0)
				{
					keybd_event(nUserKey, 0, 0, 0);
					keybd_event(nUserKey, 0, KEYEVENTF_KEYUP, 0);
					qDebug() << nUserKey;
				}
			}
		/*	{
				if (g_userSetKey[i] != 0)
				{
					keybd_event(g_userSetKey[i], 0, 0, 0);
					keybd_event(g_userSetKey[i], 0, KEYEVENTF_KEYUP, 0);
				}
			}	*/		

		}
	}
	
	return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

void LittleTool::startMonitor()
{
	HMODULE curModle = NULL;
	curModle = GetModuleHandle(NULL);
	g_hHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, (HMODULE)curModle, 0);
}

void LittleTool::stopMonitor()
{
	UnhookWindowsHookEx(g_hHook);
	g_hHook = nullptr;
}

void LittleTool::doComboBoxChanged(int nIndex)
{
	QObject* pObj = sender();
	if (pObj == ui.comboBox)
	{
		g_curOneKey = ui.comboBox->currentData().toInt();
		return;
	}

	QComboBox* pComboBox = (QComboBox*)pObj;
	g_comboBoxIndex.insert(pObj, pComboBox->currentData().toInt());
}

void LittleTool::doComboBoxChanged2(int nIndex)
{
	g_userSetKey[0] = ui.comboBox_2->currentData().toInt();
}

void LittleTool::doComboBoxChanged3(int nIndex)
{

}

void LittleTool::doComboBoxChanged4(int nIndex)
{

}

void LittleTool::doComboBoxChanged5(int nIndex)
{

}

void LittleTool::doComboBoxChanged6(int nIndex)
{

}
