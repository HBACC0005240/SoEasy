#include "LittleTool.h"
#include <windows.h>
#include <QDebug>
#include <QMessageBox>
#include "QxtGlobalShortcut.h"

static HHOOK hookid = 0;
static HHOOK g_hHook = 0;
static int g_curOneKey;
static int g_userSetKey[5];

static QHash<void*, int> g_comboBoxIndex;
static QList<QComboBox*> g_comboBoxs;
static QHash<QLineEdit*,QComboBox*> g_lineEditsHash;
static QHash<void*, int> g_comboBoxMs;
LittleTool::LittleTool(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() &~Qt::WindowContextHelpButtonHint);     //È¥µô°ïÖú
	connect(ui.comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(doComboBoxChanged(int)));
	init();
//	startMonitor();
	initHotKey();
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
	connect(ui.lineEdit_2,SIGNAL(textChanged(const QString &)),this,SLOT(doLineEditChanged(const QString&)));
	connect(ui.lineEdit_3, SIGNAL(textChanged(const QString &)), this, SLOT(doLineEditChanged(const QString&)));
	connect(ui.lineEdit_4, SIGNAL(textChanged(const QString &)), this, SLOT(doLineEditChanged(const QString&)));
	connect(ui.lineEdit_5, SIGNAL(textChanged(const QString &)), this, SLOT(doLineEditChanged(const QString&)));
	connect(ui.lineEdit_6, SIGNAL(textChanged(const QString &)), this, SLOT(doLineEditChanged(const QString&)));

	ui.comboBox_2->addItem(QStringLiteral("¿Õ"), 0);
	ui.comboBox_3->addItem(QStringLiteral("¿Õ"), 0);
	ui.comboBox_4->addItem(QStringLiteral("¿Õ"), 0);
	ui.comboBox_5->addItem(QStringLiteral("¿Õ"), 0);
	ui.comboBox_6->addItem(QStringLiteral("¿Õ"), 0);
	g_comboBoxs.append(ui.comboBox_2);
	g_comboBoxs.append(ui.comboBox_3);
	g_comboBoxs.append(ui.comboBox_4);
	g_comboBoxs.append(ui.comboBox_5);
	g_comboBoxs.append(ui.comboBox_6);
	g_lineEditsHash.insert(ui.lineEdit_2, ui.comboBox_2);
	g_lineEditsHash.insert(ui.lineEdit_3, ui.comboBox_3);
	g_lineEditsHash.insert(ui.lineEdit_4, ui.comboBox_4);
	g_lineEditsHash.insert(ui.lineEdit_5, ui.comboBox_5);
	g_lineEditsHash.insert(ui.lineEdit_6, ui.comboBox_6);
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

	ui.lineEdit_2->setText("500");
	ui.lineEdit_3->setText("500");
	ui.lineEdit_4->setText("500");
	ui.lineEdit_5->setText("500");
	ui.lineEdit_6->setText("500");

}

void LittleTool::initHotKey()
{
	QxtGlobalShortcut *shortcut = new QxtGlobalShortcut(this);
	if (shortcut)
	{
		if (shortcut->setShortcut(QKeySequence(Qt::Key_Escape), (HWND)NULL) == FALSE)
		{
			QMessageBox::information(this, "ÌáÊ¾", "×¢²áÆô¶¯ÈÈ¼üÊ§°Ü", QMessageBox::Ok);
		}
		//connect(shortcut, SIGNAL(activated(int, int)), this, SIGNAL(signal_keyReleaseEvent(int, int)));
		connect(shortcut, SIGNAL(activated(const QKeySequence&)), this, SLOT(dealHotKeyEvent(const QKeySequence&)));
	}
	//QxtGlobalShortcut *shortcut2 = new QxtGlobalShortcut(this);
	//if (shortcut2)
	//{
	//	if (shortcut2->setShortcut(QKeySequence(ui.comboBox_2->currentText()), (HWND)NULL) == FALSE)
	//	{
	//		QMessageBox::information(this, "ÌáÊ¾", "×¢²áÍ£Ö¹ÈÈ¼üÊ§°Ü", QMessageBox::Ok);
	//	}
	//	//connect(shortcut, SIGNAL(activated(int, int)), this, SIGNAL(signal_keyReleaseEvent(int, int)));
	//	connect(shortcut2, SIGNAL(activated(const QKeySequence&)), this, SLOT(dealHotKeyEvent(const QKeySequence&)));
	//}

}

// Hook callback   ¹³×Ó»Øµ÷º¯Êý
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode != HC_ACTION)//if (nCode < 0 || nCode == HC_NOREMOVE)
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	LPKBDLLHOOKSTRUCT pKeyBoardHook = (KBDLLHOOKSTRUCT  FAR*)lParam;
	if (wParam == WM_KEYDOWN /*&& wParam <= WM_MBUTTONDBLCLK*/)
	{
		if ((pKeyBoardHook->vkCode == g_curOneKey))
		{
			//if (GetKeyState(VK_F4) < 0)	// if (pkbhs.vkCode=VK_ESCAPE) and  (GetAsyncKeyState(VK_CONTROL)<0) then  Í¬Ê±ÅÐ¶Ïctrl
			//{

			//}
		/*	mouse_event(MOUSEEVENTF_RIGHTDOWN, lpPoint.x, lpPoint.y, 0, 0);
			mouse_event(MOUSEEVENTF_RIGHTUP, lpPoint.x, lpPoint.y, 0, 0);*///Êó±ê
			int nUserKey = 0;
			for (auto it= g_comboBoxIndex.begin();it!= g_comboBoxIndex.end();++it)
			{
				nUserKey = it.value();
				if (nUserKey != 0)
				{
					keybd_event(nUserKey, 0, 0, 0);
					keybd_event(nUserKey, 0, KEYEVENTF_KEYUP, 0);
					int nVal = g_comboBoxMs.value(it.key());
					Sleep(nVal);
					qDebug() << nUserKey << nVal;
				}
			}
			return 1;
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

void LittleTool::doLineEditChanged(const QString& text)
{
	QObject* pObj = sender();
	
	QComboBox* pComboBox = g_lineEditsHash.value((QLineEdit*)pObj);
	g_comboBoxMs.insert(pComboBox, text.toInt());
}

void LittleTool::dealHotKeyEvent(const QKeySequence& key)
{
	int nUserKey = 0;
	for (auto it = g_comboBoxIndex.begin(); it != g_comboBoxIndex.end(); ++it)
	{
		nUserKey = it.value();
		if (nUserKey != 0)
		{
			keybd_event(nUserKey, MapVirtualKey(nUserKey, 0), 0, 0);
			keybd_event(nUserKey, MapVirtualKey(nUserKey, 0), KEYEVENTF_KEYUP, 0);
			/*INPUT   keyin;
			WORD wScan = MapVirtualKeyW(nUserKey, 0);
			keyin.type = INPUT_KEYBOARD;
			keyin.ki.wVk = nUserKey;
			keyin.ki.wScan = wScan;
			keyin.ki.time = 100;
			keyin.ki.dwFlags = KEYEVENTF_UNICODE;
			keyin.ki.dwExtraInfo = GetMessageExtraInfo();
			::SendInput(1, &keyin, sizeof(INPUT));*/
			int nVal = g_comboBoxMs.value(it.key());
			Sleep(nVal);
			qDebug() << nUserKey << nVal;
		}
	}
}

