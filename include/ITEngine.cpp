#include "ITEngine.h"
#include <QMessageBox>
#include <QDebug>
ITEngine::ITEngine()
{

}

ITEngine::~ITEngine()
{
}

ITEngine& ITEngine::GetInstance(void)
{
	static ITEngine engineInstance;
	return engineInstance;
}

bool ITEngine::connectToDataBase(const QString& dbName, const QString& dbType, const QString& dbIP, const QString& loginUser, const QString& logingPwd, const int port/*=0*/)
{
	if (m_pDBConn)
	{
		delete m_pDBConn;
		m_pDBConn = NULL;
		m_strDBName.clear();
		m_strDBIp.clear();
		m_strDBType.clear();
		m_strDBUser.clear();
		m_strDBPwd.clear();
	}
	m_pDBConn = new ITDataBaseConn(dbType);
	if (m_pDBConn->openDataBase(dbName, dbIP, loginUser, logingPwd))
	{
		m_strDBName = dbName;
		m_strDBIp = dbIP;
		m_strDBType = dbType;
		m_strDBUser = loginUser;
		m_strDBPwd = logingPwd;		
		loadDataBaseData();
		return true;
	}
	//! 连接失败  释放指针
	if (m_pDBConn)
	{
		delete m_pDBConn;
		m_pDBConn = NULL;
		m_strDBName.clear();
		m_strDBIp.clear();
		m_strDBType.clear();
		m_strDBUser.clear();
		m_strDBPwd.clear();
	}
	return false;
}

bool ITEngine::loadDataBaseData()
{
	if (m_pDBConn == nullptr)
		return false;
	QString szSql;
	switch (m_pDBConn->getCurrentDBType())
	{
	case DB_TYPE_SQLITE:szSql = "SELECT name FROM sqlite_master WHERE type = 'table' ORDER BY name;"; break;
	case DB_TYPE_MYSQL:szSql = "SELECT name FROM sqlite_master WHERE type = 'table' ORDER BY name;"; break;
	case DB_TYPE_ORACLE:szSql = "SELECT name FROM sqlite_master WHERE type = 'table' ORDER BY name;"; break;
	default:break;
	}
	if (szSql.isEmpty())
		return false;
	QStringList tableNameList;
	ITRecordSet* recordset = m_pDBConn->execQuerySql(szSql);
	if (recordset != NULL)
	{
		while (recordset->next())
		{
			QString tableName = recordset->getStrValue("name");
			tableNameList << tableName;
		}
		delete recordset;
	}
	if (tableNameList.size() < 1)
		return false;
	switch (m_pDBConn->getCurrentDBType())
	{
	case DB_TYPE_SQLITE:szSql = "SELECT name FROM sqlite_master WHERE type = 'table' ORDER BY name;"; break;
	case DB_TYPE_MYSQL:szSql = "SELECT name FROM sqlite_master WHERE type = 'table' ORDER BY name;"; break;
	case DB_TYPE_ORACLE:szSql = "SELECT name FROM sqlite_master WHERE type = 'table' ORDER BY name;"; break;
	default:break;
	}
	foreach(QString szTableName, tableNameList)
	{
		/*szSql = QString("PRAGMA table_info([%1])").arg(szTableName);
		recordset = m_pDBConn->execQuerySql(szSql);
		if (recordset != NULL)
		{
			while (recordset->next())
			{
				QString tableName = recordset->getStrValue("name");
				tableNameList << tableName;
			}
			delete recordset;
		}*/
		ITSqlTable *model = new ITSqlTable(nullptr, m_pDBConn->getDataBase());
		model->setTable(szTableName);
		model->setEditStrategy(QSqlTableModel::OnManualSubmit);
		model->select();
		m_sqlTableModels.append(model);
		m_tableForModel.insert(szTableName, model);
	}
	
	return true;
}

bool ITEngine::saveTableData(QSqlTableModel* pTable)
{
	if (pTable == nullptr)
		return false;
	QSqlDatabase sqlDatabase = m_pDBConn->getDataBase();
	sqlDatabase.transaction();//开始事务操作
	if (pTable->submitAll())
	{
		sqlDatabase.commit();//提交
	}
	else
	{
		sqlDatabase.rollback();//回滚
		QMessageBox::information(nullptr,tr("tableModel"),tr("db error: %1").arg(pTable->lastError().text()));
	}
	return true;
}

bool ITEngine::saveToDataBase()
{	
	QSqlDatabase sqlDatabase = m_pDBConn->getDataBase();
	sqlDatabase.transaction();//开始事务操作
	for (auto it = m_tableForModel.begin(); it != m_tableForModel.end(); ++it)
	{
		if (it.value()->submitAll())
		{
			sqlDatabase.commit();//提交
		}
		else
		{
			sqlDatabase.rollback();//回滚
			qDebug() << it.value()->lastError().text();
			return false;
		}
	}
	return true;
}

ITSqlTable::ITSqlTable(QObject *parent /*= nullptr*/, QSqlDatabase db /*= QSqlDatabase()*/)
	:QSqlTableModel(parent,db)
{

}
ITSqlTable::~ITSqlTable()
{

}
void ITSqlTable::setTable(const QString &tableName)
{
	QSqlTableModel::setTable(tableName);
	setEditStrategy(QSqlTableModel::OnManualSubmit);
	select();

	int nFieldCount = this->record().count();
	for (size_t i = 0; i < nFieldCount; i++)
	{
		QSqlField field = this->record().field(i);		
		QString fieldName = this->record().fieldName(i);
		if (field.isAutoValue())
			m_primaryFieldName = fieldName;
		m_indexForField.insert(i, fieldName);
	}
	//查询加载所有 
	QSqlIndex primaryIndex = primaryKey();
	QString primaryName = primaryIndex.name();
	if (primaryName.isEmpty())
		primaryName = "id";	//先用这个吧 
	for (int i = 0;i < this->rowCount();++i)
	{
		QSqlRecord record = this->record(i);
		quint64 idVal = record.value(primaryName).toULongLong();
		m_idForRecord.insert(idVal, record);
	}
}

QSqlRecord ITSqlTable::findById64(quint64 recordID)
{
	return m_idForRecord.value(recordID);
}
