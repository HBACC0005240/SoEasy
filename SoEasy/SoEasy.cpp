#include "stdafx.h"
#include "SoEasy.h"

SoEasy::SoEasy(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setContextMenuPolicy(Qt::NoContextMenu);
	ui.mainToolBar->hide();
	ui.centralWidget->setLayout(&m_centerWidgetLayout);
	m_centerWidgetLayout.setContentsMargins(0,0,0,0);
	m_centerWidgetLayout.setHorizontalSpacing(0);
	m_centerWidgetLayout.setVerticalSpacing(0);
	m_centerWidgetLayout.setSpacing(0);

	//m_pPlayerWidget = new QPlayerWidget();
	//m_centerWidgetLayout.addWidget(m_pPlayerWidget);
	//m_pPlayerWidget->show();
	//ui.statusBar->hide();

//	connect(m_pPlayerWidget,SIGNAL(closeCurrentWidget()),this,SLOT(doCloseDstWidget()));
//	setWindowFlags( this->windowFlags() | Qt::Popup);	

	setWindowFlags( Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint | Qt::Popup);	
	QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
	animation->setDuration(1000);
	animation->setStartValue(0);
	animation->setEndValue(1);
	animation->start();
	createSysTrayAction();
	initStatusBar();
	ui.statusBar->setFixedHeight(35);
	ui.statusBar->setStyleSheet(QString("QStatusBar{background-color:rgb(240,255,253);}QStatusBar::item {border:0px;}"));
	////��ҳ�ϵ�ͬ����widgetͳһ���: tableViewΪ�̵�,����,��������;��ť��ɫΪ����ɫ
	//setStyleSheet(
	//	"QTableView { color: rgb(255, 255, 255); gridline-color: rgb(255, 255, 255);}"
	//	"QPushButton { color: rgb(255,255,255); background-color: rgb(12,134,174); }"
	//	"QLabel { color: rgb(255,255,255); }"
	//	"QTableWidget{background-color:rgb(0,71,67);}");
}

SoEasy::~SoEasy()
{

}
//************************************
//������:  closeEvent
//������qt�ر��¼�API��ֻ�����ؽ��棬���������˳�
//�������ر��¼�
//����ֵ����
//ʱ�䣺2016/3/23 WZQ
//************************************
void SoEasy::closeEvent(QCloseEvent *event)
{	
	hide();
//	QApplication::quit();
}
//************************************
//������:  quitAndDeleteAllInfo
//�����������˳�������
//��������
//����ֵ����
//ʱ�䣺2016/3/23 WZQ
//************************************
void SoEasy::quitAndDeleteAllInfo()
{	
	qApp->quit();	
}


void SoEasy::initStatusBar()
{
	m_updateNoteLabel = new QLabel(this);
	m_updateNoteLabel->setStyleSheet("QLabel{color:rgb(0,119,110);font-size:15px;}");
	m_updateNoteLabel->setText(QString("   ���ԣ�"));
	ui.statusBar->addPermanentWidget(m_updateNoteLabel);

	QLabel* pLabel3 = new QLabel(this);
	pLabel3->setMinimumWidth(20);
	ui.statusBar->addPermanentWidget(pLabel3);


	QLabel* pCurUserLabel = new QLabel(this);
	pCurUserLabel->setStyleSheet("QLabel{color:rgb(0,119,110);font-size:15px;}");
	pCurUserLabel->setText(QString("   ��ǰ�û���"));
	ui.statusBar->addWidget(pCurUserLabel);

	QLabel* pTempLabel = new QLabel(this);
	pTempLabel->setStyleSheet("QLabel{color:rgb(0,119,110);font-size:15px;}");
	pTempLabel->setMinimumWidth(20);
	ui.statusBar->addWidget(pTempLabel);

	m_pLabelUpdateState = new QLabel(this);
	m_pLabelUpdateState->setStyleSheet("QLabel{color:rgb(0,119,110);font-size:15px;}");
	ui.statusBar->addWidget(m_pLabelUpdateState);
}
//************************************
//������:  enterEvent
//����������������¼�
//�������¼�
//����ֵ����
//ʱ�䣺2016/3/23 WZQ
//************************************
void SoEasy::enterEvent(QEvent *)
{
	QRect rc;
	QRect rect;
	rect = this->geometry();
	rc.setRect(rect.x(),rect.y(),rect.width(),rect.height());
	if(rect.top()<0)
	{
		rect.setX(rc.x());
		rect.setY(0);
		move(rc.x(),-2);
	}
}

void SoEasy::leaveEvent(QEvent *)
{
	QRect rc;
	QRect rect;
	rect = this->geometry();
	rc.setRect(rect.x(),rect.y(),rect.width(),rect.height());
	if(rect.top()<0)
	{
		move(rc.x(),-rc.height()+2);
	}
}

//�ޱ߿��ƶ�  ͨ��������϶�
void SoEasy::mousePressEvent(QMouseEvent* event)
{
	if(event->button()==Qt::LeftButton)
	{
		m_bmove=true;
		m_pressedpoint = event->globalPos();	//���������λ��
		m_windowspoint = this->pos();			//������������λ��
		m_oldmousepoint = m_pressedpoint-m_windowspoint;	//�����λ�� �ڽ�����ʼ��ıȽ�
	}
}

void SoEasy::mouseMoveEvent(QMouseEvent *event)
{
	if(m_bmove)
	{	
		move(event->globalPos()-m_oldmousepoint);		//move�ƶ����ǽ������� �µ����λ�ü�ȥ��ǰ����ڽ����λ�ò� ����Ҫ�ƶ���Ŀ��� �ƶ����ǽ������ʼ����
	}
}
void SoEasy::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button()==Qt::LeftButton)
	{
		m_bmove=false;
	}
}
//************************************
//������:  createSysTrayAction
//����������ϵͳ�����ļ�
//��������
//����ֵ����
//ʱ�䣺2016/3/23 WZQ
//************************************
void SoEasy::createSysTrayAction()
{
	m_pChangeFlagsAction = new QAction("����",this);
	connect(m_pChangeFlagsAction, SIGNAL(triggered()), this, SLOT(changeWidgetFlags()));

	m_pMinimizeAction = new QAction("��С��", this);		///���̲˵���
	connect(m_pMinimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

	m_pMaximizeAction = new QAction("���", this);
	connect(m_pMaximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

	m_pRestoreAction = new QAction("�ָ�", this);
	connect(m_pRestoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

	m_pQuitAction = new QAction("�˳�", this);
	connect(m_pQuitAction, SIGNAL(triggered()), this, SLOT(quitAndDeleteAllInfo()));
	m_pTrayIconMenu = new QMenu(this);				///���̲˵�
	m_pTrayIconMenu->addAction(m_pChangeFlagsAction);
	m_pTrayIconMenu->addAction(m_pMinimizeAction);
	m_pTrayIconMenu->addAction(m_pMaximizeAction);
	m_pTrayIconMenu->addAction(m_pRestoreAction);
	m_pTrayIconMenu->addSeparator();
	m_pTrayIconMenu->addAction(m_pQuitAction);

	m_pTrayIcon = new QSystemTrayIcon(this);			///����ͼ�� 
	connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(doIconActivated(QSystemTrayIcon::ActivationReason)));
	m_pTrayIcon->setContextMenu(m_pTrayIconMenu);
	m_pTrayIcon->setToolTip("SoEasy");
	m_pTrayIcon->setIcon(QIcon(":/SoEasy/Resources/soeasy.png"));
	m_pTrayIcon->show();
	//	m_pTrayIcon->showMessage(QString("��̨���£�"), QString("���ڼ�����°汾��Ϣ�����Ժ�"));	//��ʾ�����ݣ��Զ��������ʧ

}
//************************************
//������:  doIconActivated
//��������������ͼ��������ź� �ۺ���
//��������
//����ֵ����
//ʱ�䣺2016/3/23 WZQ
//************************************
void SoEasy::doIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) 
	{
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick:	
		showNormal();				///�ָ�
		break;
	case QSystemTrayIcon::MiddleClick:
		break;
	default:
		;
	}
}


void SoEasy::doBtnClicked()
{
	QObject* obj = sender();
	/*if(obj == ui.pushButton_close)
	{
		QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
		animation->setDuration(1000);
		animation->setStartValue(1);
		animation->setEndValue(0);
		animation->start();
		connect(animation, SIGNAL(finished()), this, SLOT(close()));
	}else if(obj == ui.pushButton_min)
	{
		hide();
	}else if(obj == ui.pushButton_max)
	{
		if(isMaximized())
			showNormal();
		else
			showMaximized();
	}*/
}
void SoEasy::doCloseDstWidget()
{
	QObject* obj=sender();
	
}
void SoEasy::changeWidgetFlags()
{
	if((this->windowFlags() & Qt::WindowType_Mask) == Qt::Tool)
	{
		setWindowFlags( Qt::Window);	
	}else
		setWindowFlags( Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint | Qt::Popup);	

}