#ifndef WPGTOOL_H
#define WPGTOOL_H

#include <QtWidgets/QDialog>
#include "ui_WPGTool.h"

class WPGTool : public QDialog
{
	Q_OBJECT

public:
	WPGTool(QWidget *parent = 0);
	~WPGTool();

	bool GetProcessList();
	void createStatusBar();
	void clearTableItems();
public slots:
	void doBtnClicked();
protected:
	bool eventFilter(QObject *obj, QEvent *event);

private:
	Ui::WPGToolClass ui;
};

#endif // WPGTOOL_H
