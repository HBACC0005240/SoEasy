#include "ColorTool.h"
#include <QColorDialog>
#include "ITSignalProxy.h"
#include <QDebug>
#include "../include/ITPublic.h"
static HHOOK hookid = 0;
static 	HDC m_screenHDC;
ColorTool::ColorTool(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	/*sc = new XColorMaster(this);
	sc->setGeometry(10, 10, 25, 25);*/
	m_curColor = QColor(Qt::white);//白色
	m_screenHDC = GetDC(NULL);
	connect(g_pSignalProxy,SIGNAL(signal_updateColor(ulong,int, int)),this,SLOT(on_update_color(ulong, int, int)));
}

void ColorTool::setCurrentColor(QColor color)
{
	m_curColor = color;
	QString szColorHex = color.name();
	szColorHex=szColorHex.right(szColorHex.size()-1);
	QPalette pal(ui.widget_showColor->palette());
	//设置背景黑色
	pal.setColor(QPalette::Background, m_curColor);
	ui.widget_showColor->setAutoFillBackground(true);
	ui.widget_showColor->setPalette(pal);
	ui.widget_showColor->show();
	ui.lineEdit_r->setText(QString::number(m_curColor.red()));
	ui.lineEdit_g->setText(QString::number(m_curColor.green()));
	ui.lineEdit_b->setText(QString::number(m_curColor.blue()));
	ui.lineEdit_decimal->setText(QString::number(szColorHex.toInt(nullptr,16)));
	ui.lineEdit_hex->setText((szColorHex));
}

void ColorTool::on_pushButton_select_clicked()
{
	//Beep(500, 500);
	//::MessageBox(nullptr, ANSITOUNICODE1("112233"), ANSITOUNICODE1("112233"), MB_OK);// MeassgeBox();
	//return;
	if (ui.checkBox_isMouse->isChecked())
	{
		ui.checkBox_isMouse->setChecked(false);
		on_checkBox_isMouse_clicked();
	}
	//查询  
	QColor newColor = QColor(ui.lineEdit_r->text().toInt(), ui.lineEdit_g->text().toInt(), ui.lineEdit_b->text().toInt());
	if (newColor.name() != m_curColor.name())
	{
		setCurrentColor(newColor);
		ui.lineEdit_mousePos->setText("");
		return;
	}
	int nColorVal = ui.lineEdit_decimal->text().toInt();
	QString str = QString("%1").arg(nColorVal, 6, 16, QChar('0'));    //str = "0064"
	int red = nColorVal & 255;
	int green = nColorVal >> 8 & 255;
	int blue = nColorVal >> 16 & 255;
	newColor= QColor("#"+str);
//	newColor = QColor(red, green, blue);
//	newColor = QColor(GetRValue(nColorVal), GetGValue(nColorVal), GetBValue(nColorVal));
	if (newColor.name() != m_curColor.name())
	{
		setCurrentColor(newColor);
		ui.lineEdit_mousePos->setText("");
		return;
	}
	newColor = QColor("#"+ui.lineEdit_hex->text());
	qDebug() << ui.lineEdit_hex->text();
	if (newColor.name() != m_curColor.name())
	{
		setCurrentColor(newColor);
		ui.lineEdit_mousePos->setText("");
		return;
	}
}

void ColorTool::on_checkBox_isMouse_clicked()
{
	if (ui.checkBox_isMouse->isChecked())
	{
		int nLastError = 0;
		if (hookid == 0)
		{
			HMODULE curModle = NULL;
			curModle = GetModuleHandle(NULL);
			hookid = ::SetWindowsHookEx(WH_MOUSE_LL/*WH_JOURNALRECORD*/, HOOKPROC(&MouseHookProc), (HMODULE)curModle, 0);		
			nLastError = ::GetLastError();
		}
	}
	else
	{
		UnhookWindowsHookEx(hookid);
		hookid = 0;	
	}
}

void ColorTool::on_update_color(ulong color, int x, int y)
{
	setCurrentColor(QColor(color));
	ui.lineEdit_mousePos->setText(QString("%1,%2").arg(x).arg(y));
}

LRESULT ColorTool::MouseHookProc(DWORD _dwCode, DWORD _wParam, DWORD _lParam)
{
	if (_dwCode != HC_ACTION)
		return 	CallNextHookEx(hookid, _dwCode, _wParam, _lParam);
	LPMSLLHOOKSTRUCT pMouseHook = (MSLLHOOKSTRUCT FAR*)_lParam;
	POINT pi;
	::GetCursorPos(&pi);	
	if (_wParam >= WM_MOUSEMOVE && _wParam <= WM_MBUTTONDBLCLK)
	{
		emit g_pSignalProxy->signal_updateColor(GetPixel(m_screenHDC, pi.x, pi.y),pi.x,pi.y);
	}
	return 	CallNextHookEx(hookid, _dwCode, _wParam, _lParam);
}