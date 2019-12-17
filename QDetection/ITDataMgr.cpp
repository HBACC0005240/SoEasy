#include "stdafx.h"
#include "ITDataMgr.h"

ITDataMgr::ITDataMgr()
{
}

ITDataMgr::~ITDataMgr()
{

}

ITDataMgr& ITDataMgr::getInstance()
{
	static ITDataMgr pInstance;
	return pInstance;
}
ITDevice* ITDataMgr::createNewDevice()
{
	int nMaxCode=0;
	for(int i=0;i<m_pDeviceList.size();++i)
	{
		if(nMaxCode < m_pDeviceList[i]->getDeviceCode().toInt())
			nMaxCode =  m_pDeviceList[i]->getDeviceCode().toInt();
	}
	++nMaxCode;
	ITDevice* pNewDev=new ITDevice;
	pNewDev->setDeviceCode(QString(nMaxCode));
	m_pDeviceList.append(pNewDev);
	return pNewDev;
}