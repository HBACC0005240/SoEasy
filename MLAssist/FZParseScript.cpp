#include "FZParseScript.h"
#include <QFile>
#include <windows.h>
#include <QtConcurrent>
#include "GameData.h"
#include "MLAssist.h"
FZParseScript::FZParseScript( )
{
	m_tScriptTimerInterval = 320;
	m_scriptCtrl = 0;
	m_scriptRunRow = 0;
}

FZParseScript::~FZParseScript()
{
}

FZParseScript& FZParseScript::getInstance()
{
	static FZParseScript g_pFZParseScript;
	return g_pFZParseScript;
}

void FZParseScript::ParseGameScript(const QString& szPath)
{
	if (!QFile::exists(szPath))
		return;
	QFile file(szPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	m_scriptDataList.clear();
	int nRowIndex = 0;
	//先处理下脚本 空行  还有注释
	QTextCodec *tc = QTextCodec::codecForName("GBK");
	m_szScriptDesc = tc->toUnicode(file.readLine());
	while (!file.atEnd())
	{
		QByteArray line = file.readLine();
		QString szData = tc->toUnicode(line);
//		QString szData = QString::fromLocal8Bit(line);
		szData = szData.simplified();
		szData=szData.remove(QRegExp("\\s"));
		int nIndex = szData.indexOf("//");
		szData = szData.mid(0, nIndex);
		nIndex = szData.indexOf(";");
		szData = szData.mid(0, nIndex);
		if (szData.isEmpty())//空行不处理
			continue;
//		m_script.insert(++nRowIndex, szData);
		m_scriptDataList.append(szData);
	}
	file.close();
	//拆分脚本 设置部分  还有循环部分 分开
	for (int i=0;i< m_scriptDataList.size();++i)
	{
		QString szData = m_scriptDataList.at(i);
		//开始 解析脚本
		if (szData.contains(":"))//goto标志
		{
			int nIndex = szData.indexOf(":");
			m_mark.insert(i,szData.mid(0, nIndex)); //记录每个脚本循环点
		}
	}

}

void FZParseScript::SetScriptRunRow(int nRow)
{
	m_scriptRunRow = nRow;
}

bool FZParseScript::RunScript()
{
	//解析指令
	if (m_scriptDataList.size() < 1)
		return false;
	for (int i = 0; i < m_scriptDataList.size(); ++i)
	{
		QString szData = m_scriptDataList.at(i);
		szData=szData.simplified();//去掉所有空格
		if (szData.startsWith("set("))
		{
			ParseSet(szData);
		}else if (szData.startsWith("InputVal("))
		{
			ParseInputVal(szData);
		}
		//开始 解析脚本
		if (szData.contains(":"))//goto标志
		{
			m_scriptRunRow = i;
			break;//第一个goto语句  跳到线程 
		}
	}
	m_scriptCtrl = SCRIPT_CTRL_RUN;
	QtConcurrent::run(RunScriptThread, this);
	return true;
}

bool FZParseScript::StopScript()
{
	m_scriptCtrl = SCRIPT_CTRL_STOP;
	return true;
}

bool FZParseScript::PauseScript()
{
	m_scriptCtrl = SCRIPT_CTRL_PAUSE;
	return true;
}

void FZParseScript::ParseSet(const QString& szData)
{
	QString szScript = MidBrackets(szData);
	QStringList args = szScript.split(",");
	if (args.size() == 2)
	{
		if (args[0] == "timer")
		{
			setTimerCmd(SCRIPT_TIMER_RUN,args.at(1).toInt());
		}else if (args[0] == "timer1")
		{
			setTimerCmd(SCRIPT_TIMER_SYS, args.at(1).toInt());
		}
	}else if (args.size() == 3)
	{

	}
}

void FZParseScript::setTimerCmd(int nType, int nTimer)
{
	switch (nType)
	{
	case SCRIPT_TIMER_RUN:m_tScriptTimerInterval = nTimer; break;
	case SCRIPT_TIMER_SYS:m_tSysCmdTimerInterval = nTimer; break;
	default:break;
	}
}
//遇敌设置
void FZParseScript::setAutoActionCmd(int nType, int nVal)
{

}

bool FZParseScript::nop()
{
	while (GameData::getInstance().IsBattering())//战斗中 等待
	{
		Sleep(m_tScriptTimerInterval);
	}
	return true;
}

bool FZParseScript::nowhile(const QString& szData)
{	
	QString szScript = MidBrackets(szData);
	szScript = szScript.remove("\"");
	QStringList args = szScript.split(",");
	if (args.size() == 3)
	{
		while (1)//战斗中 等待
		{
			QString szMap = GameData::getInstance().GetGameMapName();
			if (szMap == args[0])
			{
				POINT gamePoint = GameData::getInstance().GetGamePersonCoordinate();
				qDebug() << gamePoint.x << gamePoint.y;
				if(gamePoint.x == args[1].toInt() && gamePoint.y == args[2].toInt())
					break;
			}
			Sleep(m_tScriptTimerInterval);
		}
	}
	else if (args.size() == 2)
	{
		while (1)//战斗中 等待
		{
			QString szMap = GameData::getInstance().GetGameMapName();
			qDebug() << szMap;
			if (szMap == args[0])
			{			
				break;
			}
			Sleep(m_tScriptTimerInterval);
		}
	}
	return true;
}

bool FZParseScript::moveto(const QString& szData)
{
	QString szScript = MidBrackets(szData);
	QStringList args = szScript.split(",");
	if (args.size() != 2)
		return false;
	int nTimeOut = 60000;//1分钟走不到 退出
	POINT lastPoint;	//监测上次坐标点 如果调用后和上次一样，则继续调用 否则等待
	lastPoint = GameData::getInstance().GetGamePersonCoordinate();
	while (m_scriptCtrl!= SCRIPT_CTRL_STOP)
	{
		if (m_scriptCtrl == SCRIPT_CTRL_PAUSE)
		{
			Sleep(m_tScriptTimerInterval);
			continue;
		}
	//	GameData::getInstance().moveToPoint(args[0].toInt(), args[1].toInt());
		POINT gamePoint = GameData::getInstance().GetGamePersonCoordinate();
		qDebug() << szData << "当前坐标："<< gamePoint.x << gamePoint.y;
		if (gamePoint.x == args[0].toInt() && gamePoint.y == args[1].toInt())
			break;
		if (lastPoint.x == gamePoint.x && lastPoint.y == gamePoint.y)//上次调用后 没走动 需要再次调用
		{//如果是遇敌状态 不调用
			if (GameData::getInstance().IsBattering()==false)		
				GameData::getInstance().moveToPoint(args[0].toInt(), args[1].toInt());
		}
		lastPoint = gamePoint;
		Sleep(m_tScriptTimerInterval);
	}
	Sleep(m_tScriptTimerInterval);//成功后 新坐标 等待一下
	return true;
}

bool FZParseScript::turn_about(const QString& szData)
{
	QString szScript = MidBrackets(szData);
	QStringList args = szScript.split(",");
	if (args.size() != 2)
		return false;
	int direction = args[0].toInt();
	GameData::getInstance().turn_about(direction);
	return true;
}

bool FZParseScript::high(const QString& szData)
{
	QString szScript = MidBrackets(szData);
	QStringList args = szScript.split(",");
	if (args.size() != 3)
		return false;
	args.removeAt(0);//第一个删除
	int nTimeOut = 60000;//1分钟走不到 退出
	POINT lastPoint;	//监测上次坐标点 如果调用后和上次一样，则继续调用 否则等待
	lastPoint = GameData::getInstance().GetGamePersonCoordinate();
	while (m_scriptCtrl != SCRIPT_CTRL_STOP)
	{
		if (m_scriptCtrl == SCRIPT_CTRL_PAUSE)
		{
			Sleep(m_tScriptTimerInterval);
			continue;
		}
		//	GameData::getInstance().moveToPoint(args[0].toInt(), args[1].toInt());
		POINT gamePoint = GameData::getInstance().GetGamePersonCoordinate();
		qDebug() << szData << "当前坐标：" << gamePoint.x << gamePoint.y;
		if (gamePoint.x == args[0].toInt() && gamePoint.y == args[1].toInt())
			break;
		if (lastPoint.x == gamePoint.x && lastPoint.y == gamePoint.y)//上次调用后 没走动 需要再次调用
			GameData::getInstance().moveToPoint(args[0].toInt(), args[1].toInt());
		lastPoint = gamePoint;
		Sleep(m_tScriptTimerInterval);
	}
	Sleep(m_tScriptTimerInterval);//成功后 新坐标 等待一下
	return true;
}

QString FZParseScript::MidBrackets(const QString& szData)
{
	QString szScript = szData.mid(szData.indexOf("(") + 1);
	szScript = szScript.mid(0, szScript.indexOf(")"));
	return szScript;
}

//输入设置
void FZParseScript::ParseInputVal(const QString& szData)
{
	QString szScript = MidBrackets(szData);
	QStringList args = szScript.split(",");//参数
	if (args.size() != 3)
		return;
	m_userDefVariable.insert(args[0], args[2].remove("\"").toInt());
}

void FZParseScript::ParseIFCmd(const QString& szData)
{
	QString szScript = MidBrackets(szData);
	szScript = szScript.remove("\"");//去掉双引号
	QStringList args = szScript.split(",");
	if (args.size() < 5)
		return;
	if (args[0].contains("状态"))
	{
		if (args[1].contains("GameMap.x.y"))//判断人物当前位置
		{
			if (args[2]== "=")
			{//判断当前坐标
				POINT curPoint = GameData::getInstance().GetGamePersonCoordinate();
				//如果是判断坐标 则x y是两个参数 拆分根据,逗号拆分 是6个数据
				if (curPoint.x == args[3].toInt() && curPoint.y == args[4].toInt())
				{//跳转
					int nRow = m_mark.key(args[5]);
					m_scriptRunRow = nRow;
				}
			}
		}
	}
}

void FZParseScript::RunScriptThread(FZParseScript* pThis)
{
	while (pThis->m_scriptCtrl != SCRIPT_CTRL_STOP)//暂停和执行 继续
	{
		if (pThis->m_scriptCtrl==SCRIPT_CTRL_PAUSE)
		{
			Sleep(pThis->m_tScriptTimerInterval);
			continue;
		}
		QString szScriptData = pThis->m_scriptDataList.at(pThis->m_scriptRunRow);		
		szScriptData = szScriptData.simplified();//去掉所有空格
		qDebug() << szScriptData;
		//开始解析指令
		if (szScriptData=="nop")
		{
			pThis->nop();
		}
		else if (szScriptData == "getout")
		{

		}
		else if (szScriptData.startsWith("WaitTime("))
		{
			int nVal = pThis->MidBrackets(szScriptData).toInt();
			Sleep(nVal);
		}
		else if (szScriptData.startsWith("if("))
		{
			pThis->ParseIFCmd(szScriptData);
		}
		else if (szScriptData.startsWith("goto("))
		{
			pThis->m_scriptRunRow = pThis->m_mark.key(pThis->MidBrackets(szScriptData));
		}
		else if (szScriptData.startsWith("moveto("))
		{
			pThis->moveto(szScriptData);
		}
		else if (szScriptData.startsWith("turn_about("))
		{
			pThis->turn_about(szScriptData);
		}
		else if (szScriptData.startsWith("high("))
		{
			pThis->high(szScriptData);
		}
		else if (szScriptData.startsWith("nowhile("))
		{
			pThis->nowhile(szScriptData);
		}
		pThis->m_scriptRunRow = pThis->m_scriptRunRow + 1;
		Sleep(pThis->m_tScriptTimerInterval);

	}
}
