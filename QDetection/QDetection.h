#ifndef QDETECTION_H
#define QDETECTION_H

#include "stdafx.h"
#include <QtWidgets/QMainWindow>
#include "ui_QDetection.h"
#include "ITDevice.h"

class QDetection : public QMainWindow
{
	Q_OBJECT

public:
	QDetection(QWidget *parent = 0);
	~QDetection();

	void openDstPicFile(const QString& strFile);
	void setVideoFrameImage(const QImage& image);
	void clearTableItems();
	void showTableItems(ITDeviceList pVehicles);
	void refreshTable();
public slots:
	void doBtnClicked();
	void doDisplayImage();
protected:
	Mat CharsChuli(Mat in);
	Mat histeq(Mat in);
	std::string InttoChars(int q);
	Mat ReturnMat(Mat img, int t);
	int CharsCharacter(Mat in);
	int CharsChinese(Mat f);	
//	void resizeEvent(QResizeEvent *eve);
	bool eventFilter(QObject *, QEvent *);
private:
	Ui::QDetectionClass ui;
	CvANN_MLP m_annMlp;

	QImage m_curDisImage;
	QMap<int,ITDevice*> m_rowForVehicle;	///行与指针映射
	QMap<int,QPushButton*>	m_nRowForBtn;		//行和checkBox对应
};

#endif // QDETECTION_H
