#pragma once

#include "stdafx.h"
#include "GlobalDef.h"
class ITObject;
class ITFileGroup;
class ITFile;
typedef QList<ITObject*> ITObjectList;
typedef QList<ITFileGroup*> ITFileGroupList;
typedef QList<ITFile*> ITFileList;

//设备状态
enum ITDevStatus				
{
	ITDEV_STATUS_NORMAL = 0,	//正常
	ITDEV_STATUS_ADD = 1,       //增加
	ITDEV_STATUS_EDIT = 2,     	//修改
	ITDEV_STATUS_DELETE = 3,    //删除
};

//基类
class ITObject
{
public:
	ITObject();
	ITObject(QString strDeviceName, int nDeviceType, QString strDataCode);
	virtual ~ITObject();

	//设置状态值
	int getStatus(void) { return m_nStatus; };
	void setAddStatus(void) { m_nStatus = ITDEV_STATUS_ADD; }
	virtual void setDelStatus(void) { m_nStatus = ITDEV_STATUS_DELETE; }
	void setEditStatus(void) { if (m_nStatus == ITDEV_STATUS_NORMAL)m_nStatus = ITDEV_STATUS_EDIT; }
	void setNomalStatus(void) { m_nStatus = ITDEV_STATUS_NORMAL; }

	void setDataType(int nType) { m_nDataType = nType; }
	int getDataType() { return m_nDataType; }

	void setDataCode(const QString& strCode) { m_strDataCode = strCode; }
	QString getDataCode() { return m_strDataCode; }

	void setDataName(const QString& strName) { m_strDevName = strName; }
	QString getDataName() { return m_strDevName; }

	void setDataDesc(const QString& strDesc) { m_strDesc = strDesc; }
	QString getDataDesc() { return m_strDesc; }

	void setDeviceOwner(ITObject *pDevice) { m_pOwner = pDevice; }
	ITObject* getDeviceOwner(void) { return m_pOwner; }
private:
	QString m_strDataCode;		//唯一编码
	QString m_strDevName;		//名称
	QString m_strRemark;		//备注
	QString m_strDesc;			//描述
	int m_nStatus;				//设备修改状态, 有修改状态的设备有部门、主机和通道
	ITObject*	m_pOwner;		//父节点
	int m_nDataType;				//类型
};
class ITFileGroup :public ITObject
{
public:
	ITFileGroup();
	ITFileGroup(QString strName, int nDataType, QString strDataCode);
	virtual ~ITFileGroup();

	bool addChildGroup(ITFileGroup* pGroup);
	bool removeChildGroup(ITFileGroup* pGroup);
	void clearAllSubGroup() { m_pSubGroupList.clear(); }

	bool addChildFile(ITFile* pFile);
	bool removeChildFile(ITFile* pFile);
	void clearAllSubFile() { m_pSubFileList.clear(); }
private:
	ITFileList m_pSubFileList;	//目录下文件列表
	ITFileGroupList m_pSubGroupList;	//目录下的目录

};
//文件类
class ITFile :public ITObject
{
public:
	ITFile();
	ITFile(QString strName, int nDataType, QString strDataeCode);
	virtual ~ITFile();

	void setFileType(const int& nType) { m_nFileType = nType; }
	int getFileType() { return m_nFileType; }

	void setFileBaseDir(const QString& strDir) { m_strBaseDir = strDir; }
	QString getFileBaseDir() { return m_strBaseDir; }

	void setFilePath(const QString& strPath) { m_strFilePath = strPath; }
	QString getFilePath() { return m_strFilePath; }

	int getOrdinal(void) { return m_nOrdinal; }
	void setOrdial(int nOrdinal) { m_nOrdinal = nOrdinal; }
private:
	int m_nFileType;		//文件类型
	QString m_strBaseDir;	//文件主目录
	QString m_strFilePath;	//文件全路径
	QIcon m_fileIcon;		//文件图标
	int m_nOrdinal;	        //用于排序

};