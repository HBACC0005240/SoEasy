#pragma once

#include <QObject>
#include "ITDatabaseConn.h"
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlIndex>
#include <QSqlField>
#include "ITObjectID.h"
typedef QList<QSqlTableModel*> QSqlTableModelList;
class ITSqlTable :public QSqlTableModel
{
public:
	ITSqlTable(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());
	virtual ~ITSqlTable();

	void setTable(const QString &tableName);
	QSqlRecord findById64(quint64 recordID);
private:
	QString m_primaryFieldName;
	QHash<quint64, QSqlRecord> m_idForRecord;
	QHash<int, QString> m_indexForField;	//
};
class ITEngine : public QObject
{
	Q_OBJECT

public:
	ITEngine();
	~ITEngine();
	static ITEngine& GetInstance(void);

	bool connectToDataBase(const QString& dbName,const QString& dbType,const QString& dbIP,\
		const QString& loginUser,const QString& logingPwd,const int port=0);
	bool loadDataBaseData();
	static ITEngine engineInstance;
	ITSqlTable* getTable(const QString& szTableName) { return m_tableForModel.value(szTableName); }
	bool saveTableData(QSqlTableModel* pTable);
	bool saveToDataBase();
private:
	ITDataBaseConn* m_pDBConn;
	QString						m_strDBName;			//当前数据库名称
	QString						m_strDBIp;				//当前数据库连接地址
	QString						m_strDBType;			//当前数据库类型
	QString						m_strDBUser;			//当前数据库用户名
	QString						m_strDBPwd;				//当前数据库密码
	QSqlTableModelList			m_sqlTableModels;		//所有表模型
	QHash<QString, ITSqlTable*> m_tableForModel;
};
