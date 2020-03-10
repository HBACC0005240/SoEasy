#pragma once

#include <QObject>
#include <QMap>
#include <QTimer>
class FZParseScript : public QObject
{
	Q_OBJECT
public:
	FZParseScript();
	~FZParseScript();
	enum SCRIPT_CTRL
	{
		SCRIPT_CTRL_STOP = 0,	//ֹͣ
		SCRIPT_CTRL_RUN = 1,	//ִ��
		SCRIPT_CTRL_PAUSE = 2,//��ͣ
	};
	enum SCRIPT_TIMER
	{
		SCRIPT_TIMER_RUN = 1,	//�ű���ʱ��
		SCRIPT_TIMER_SYS = 2,	//ϵͳָ�ʱ��
	};

	QString getGameDescript() { return m_szScriptDesc; }
	QStringList GetGameScriptData() { return m_scriptDataList; }
	int GetGameScriptRunRow() { return m_scriptRunRow; }
	int GetGameScriptCtrlStatus() { return m_scriptCtrl; }
	static FZParseScript& getInstance();
	void ParseGameScript(const QString& szPath);
	void SetScriptRunRow(int nRow);
	bool RunScript();
	bool StopScript();
	bool PauseScript();
	void ParseSet(const QString& szData);
	void setTimerCmd(int nType,int nTimer);
	void setAutoActionCmd(int nType, int nVal);

	bool nop();
	bool nowhile(const QString& szData);
	bool moveto(const QString& szData);
	bool turn_about(const QString& szData);
	bool high(const QString& szData);
	QString MidBrackets(const QString& szData);
	void ParseInputVal(const QString& szData);

	void ParseIFCmd(const QString& szData);
	static void RunScriptThread(FZParseScript* pThis);
private:
//	QMap<int, QString> m_script;
	QString m_szScriptDesc;	//�ű�������Ϣ
	int m_scriptCtrl;		//�ű�����
	int m_scriptRunRow;		//�ű���ǰ����λ��
	QStringList m_scriptDataList;
	QMap<int,QString > m_mark;//goto ��־���ƶ�Ӧ�Ľű���
	QTimer m_scriptRunTimer;		//�ű����ж�ʱ�� ��λ�Ǻ��룬Ĭ��ֵΪ320��һ�㲻��Ҫ���иĶ�����ֵԽС�ű����е��ٶ�Խ�죬��������ζ��Ч�ʻ����
	int m_tScriptTimerInterval;	//�ű����м��
	int m_tSysCmdTimerInterval;	//ϵͳָ�ʱ�����
	QMap<QString, int> m_userDefVariable;//�û��Զ������
};
