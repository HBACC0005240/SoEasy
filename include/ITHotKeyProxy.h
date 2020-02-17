#ifndef ITHOTKEYPROXY_H
#define ITHOTKEYPROXY_H

#include <QObject>
#include <QMap>
#include <QtGui>
#include <QCoreApplication>
#include "qxtglobalshortcut.h"

class ITHotKeyProxy : public QObject
{
	Q_OBJECT

public:
	ITHotKeyProxy();
	~ITHotKeyProxy();
	static ITHotKeyProxy& getInstance();
	static void initHotKeyProxy();

	void setHotKeyWnd(HWND pHwnd){m_pWnd=pHwnd;}
	void readShortcuts();										///���������ļ� ��ȡ�ȼ��б�
	QKeySequence TransIniHotKey(const QString& szKey);			///���������ļ������keyֵ��ȡת��ΪQt�ȼ�ֵ
	bool isAlreadyRegister(QKeySequence keys);
signals:
	void signal_HotKeyEvent(const QKeySequence&);
public slots:

private:
	QMap<QString, QKeySequence> m_szKeyDefMapVal;		///�����ļ���keyֵ �� qt�ȼ���ӳ��
	HWND m_pWnd;	///���ھ��
};
#endif // ITHOTKEYPROXY_H
