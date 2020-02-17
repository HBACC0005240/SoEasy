#include "ITHotKeyProxy.h"
#include <windows.h>
#include <string>
using namespace std;
ITHotKeyProxy::ITHotKeyProxy()
{
	m_pWnd=NULL;
}

ITHotKeyProxy::~ITHotKeyProxy()
{

}

ITHotKeyProxy& ITHotKeyProxy::getInstance()
{
	static ITHotKeyProxy g_pHotKeyProxy;
	return g_pHotKeyProxy;
}
//***************************************
//函数名：readShortcuts
//描述：加载热键配置文件
//参数：无
//返回值：bool
//时间：2017/12/26 WZQ
//***************************************
void ITHotKeyProxy::readShortcuts()
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL,szPath,MAX_PATH);
	QString szTempPath = QString::fromWCharArray(szPath);
//	std::string szTemp = szPath;
//	int nPos = szTemp.rfind('\\');
//	QString strPath = szTemp.substr(0,nPos)+"/mits.ini";
	QString strPath = szTempPath.mid(0,szTempPath.lastIndexOf("\\")) +"//config//shortcuts.ini";
	strPath = QDir::toNativeSeparators(strPath);
	QSettings iniFile(strPath, QSettings::IniFormat); 
	iniFile.setIniCodec(QTextCodec::codecForName("GB2312"));
	iniFile.beginGroup("HotKey");
	QStringList  szKeyList = iniFile.childKeys();
	foreach(QString szKey,szKeyList)
	{
		QString szTempVal = iniFile.value(szKey,"").toString();
		QxtGlobalShortcut *shortcut = new QxtGlobalShortcut(this);
		if(shortcut)
		{
			shortcut->setShortcut(QKeySequence(szTempVal),m_pWnd);
			connect(shortcut, SIGNAL(activated(int,int)),this,SIGNAL(signal_keyReleaseEvent(int,int)));
			connect(shortcut, SIGNAL(activated(const QKeySequence&)), this,SIGNAL(signal_HotKeyEvent(const QKeySequence&)));
		}		
		m_szKeyDefMapVal.insert(szKey,QKeySequence(szTempVal));
	}
	iniFile.endGroup();
}
QKeySequence ITHotKeyProxy::TransIniHotKey(const QString& szKey)
{
	if(szKey.isEmpty())
		return NULL;
	return m_szKeyDefMapVal.value(szKey);
}

bool ITHotKeyProxy::isAlreadyRegister(QKeySequence keys)
{
	qDebug()<<"按键："<<keys.toString(); 
	for(auto it=m_szKeyDefMapVal.begin();it!=m_szKeyDefMapVal.end();++it)
	{
		qDebug()<<"搜索项："<< it.key()<<QKeySequence(it.value()).toString(); 
		if(keys.matches(QKeySequence(it.value())))
			return true;
	}
	return false;
}