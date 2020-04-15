#pragma once

#include "stdafx.h"
#include "GlobalDef.h"
class ITObject;
class ITFileGroup;
class ITFile;
typedef QList<ITObject*> ITObjectList;
typedef QList<ITFileGroup*> ITFileGroupList;
typedef QList<ITFile*> ITFileList;

//�豸״̬
enum ITDevStatus				
{
	ITDEV_STATUS_NORMAL = 0,	//����
	ITDEV_STATUS_ADD = 1,       //����
	ITDEV_STATUS_EDIT = 2,     	//�޸�
	ITDEV_STATUS_DELETE = 3,    //ɾ��
};

//����
class ITObject
{
public:
	ITObject();
	ITObject(QString strDeviceName, int nDeviceType, QString strDataCode);
	virtual ~ITObject();

	//����״ֵ̬
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
	QString m_strDataCode;		//Ψһ����
	QString m_strDevName;		//����
	QString m_strRemark;		//��ע
	QString m_strDesc;			//����
	int m_nStatus;				//�豸�޸�״̬, ���޸�״̬���豸�в��š�������ͨ��
	ITObject*	m_pOwner;		//���ڵ�
	int m_nDataType;				//����
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
	ITFileList m_pSubFileList;	//Ŀ¼���ļ��б�
	ITFileGroupList m_pSubGroupList;	//Ŀ¼�µ�Ŀ¼

};
//�ļ���
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
	int m_nFileType;		//�ļ�����
	QString m_strBaseDir;	//�ļ���Ŀ¼
	QString m_strFilePath;	//�ļ�ȫ·��
	QIcon m_fileIcon;		//�ļ�ͼ��
	int m_nOrdinal;	        //��������

};