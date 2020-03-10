#pragma once

#include <QtWidgets/QWidget>
#include "ui_ColorTool.h"
#include "XColorMaster.h"
#include <windows.h>
class ColorTool : public QWidget
{
	Q_OBJECT

public:
	ColorTool(QWidget *parent = Q_NULLPTR);
	enum CheckedColor
	{
		H, S, V, R, G, B
	}checkedColor;

	void setCurrentColor(QColor color);
	static LRESULT MouseHookProc(DWORD _dwCode, DWORD _wParam, DWORD _lParam);	
public slots:
	void on_pushButton_select_clicked();
	void on_checkBox_isMouse_clicked();
	void on_update_color(ulong color, int x, int y);
private:
	Ui::ColorToolClass ui;
	XColorMaster *sc;
	QColor m_curColor;
};
