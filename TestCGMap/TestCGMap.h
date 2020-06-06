#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TestCGMap.h"
#include "global_def.h"
class TestCGMap : public QMainWindow
{
	Q_OBJECT

public:
	TestCGMap(QWidget *parent = Q_NULLPTR);
	~TestCGMap();
	void readMap();
	void loadTSPlug();
	void unLoadTSPlug();
protected:
	void paintEvent(QPaintEvent *event);
public slots:
	void on_pushButton_clicked();
private:
	Ui::TestCGMapClass ui;
	cga_map_cells_t * m_collision;
	QString m_runPath;
};
