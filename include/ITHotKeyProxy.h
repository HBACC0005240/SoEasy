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
	void readShortcuts();										///加载配置文件 读取热键列表
	QKeySequence TransIniHotKey(const QString& szKey);			///根据配置文件定义的key值来取转换为Qt热键值
	bool isAlreadyRegister(QKeySequence keys);
signals:
	void signal_HotKeyEvent(const QKeySequence&);
public slots:

private:
	QMap<QString, QKeySequence> m_szKeyDefMapVal;		///配置文件中key值 和 qt热键的映射
	HWND m_pWnd;	///窗口句柄
};
#endif // ITHOTKEYPROXY_H
