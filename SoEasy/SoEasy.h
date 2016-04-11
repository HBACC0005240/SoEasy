#ifndef SOEASY_H
#define SOEASY_H

#include <QtWidgets/QMainWindow>
#include "ui_SoEasy.h"
#include <QGridLayout>
//#include "QPlayerWidget.h"

class SoEasy : public QMainWindow
{
	Q_OBJECT

public:
	SoEasy(QWidget *parent = 0);
	~SoEasy();

	void createSysTrayAction();
	void initStatusBar();
protected:
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void closeEvent(QCloseEvent *event);
	
public slots:
	void doBtnClicked();
	void doIconActivated(QSystemTrayIcon::ActivationReason reason);
	void quitAndDeleteAllInfo();
	void doCloseDstWidget();
	void changeWidgetFlags();
private:
	Ui::SoEasyClass ui;
	QGridLayout			m_centerWidgetLayout;
//	QPlayerWidget*		m_pPlayerWidget;
	bool				m_bmove;
	QPoint				m_pressedpoint;
	QPoint				m_windowspoint;
	QPoint				m_oldmousepoint;
	QAction*			m_pMinimizeAction;		//��С�Ű�ť
	QAction*			m_pMaximizeAction;		//��󻯰�ť
	QAction*			m_pRestoreAction;		//�ָ���ť
	QAction*			m_pChangeFlagsAction;	//����ť
	QAction*			m_pQuitAction;			//�˳���ť
	QSystemTrayIcon*	m_pTrayIcon;			//ϵͳ����ͼ��
	QMenu *				m_pTrayIconMenu;		//ϵͳ���̲˵�

	QLabel* m_updateNoteLabel;	
	QLabel* m_pLabelUpdateState;
};

#endif // SOEASY_H
