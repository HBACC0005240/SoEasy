#include "stdafx.h"
#include "ITObject.h"


ITObject::ITObject()
{
	m_nStatus = ITDEV_STATUS_NORMAL;
}
ITObject::ITObject(QString strDataName, int nDataType, QString strDataCode)
{
	m_nDataType = nDataType;
	m_strDataCode = strDataCode;
	m_strDevName = strDataName;
	m_pOwner = NULL;
	m_nStatus = ITDEV_STATUS_NORMAL;
}

ITObject::~ITObject()
{
}

ITFileGroup::ITFileGroup()
{
	
}
ITFileGroup::ITFileGroup(QString strName, int nDataType, QString strDataeCode)
	:ITObject(strName, nDataType, strDataeCode)
{

}

ITFileGroup::~ITFileGroup()
{
}
bool ITFileGroup::addChildGroup(ITFileGroup* pDir)
{
	if (!pDir)
		return false;
	for (int i = 0; i < m_pSubGroupList.size(); ++i)
	{
		if (m_pSubGroupList[i] == pDir)
		{
			return true;
		}
	}
	m_pSubGroupList.push_back(pDir);
	return true;
}
bool ITFileGroup::removeChildGroup(ITFileGroup* pDir)
{
	if (!pDir)
		return false;
	m_pSubGroupList.removeOne(pDir);
	return true;
}
bool ITFileGroup::addChildFile(ITFile* pFile)
{
	if (!pFile)
		return false;
	for (int i = 0; i < m_pSubFileList.size(); ++i)
	{
		if (m_pSubFileList[i] == pFile)
		{
			return true;
		}
	}
	m_pSubFileList.push_back(pFile);
	return true;
}
bool ITFileGroup::removeChildFile(ITFile* pFile)
{
	if (!pFile)
		return false;
	m_pSubFileList.removeOne(pFile);
	return true;
}

ITFile::ITFile()
{
	m_nOrdinal = 0;
	m_nFileType = 0;
}
ITFile::ITFile(QString strName, int nDataType, QString strDataeCode)
	:ITObject(strName, nDataType, strDataeCode)
{

}

ITFile::~ITFile()
{
}
