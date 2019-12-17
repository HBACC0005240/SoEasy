#ifndef ITDATAMGR_H
#define ITDATAMGR_H

#include <QObject>
#include "ITMutex.h"
#include "ITDevice.h"
class ITDataMgr : public QObject
{
	Q_OBJECT

public:
	ITDataMgr();
	~ITDataMgr();

	static ITDataMgr& getInstance();
	

	ITDevice* createNewDevice();
	ITDeviceList getAllDevices(){return m_pDeviceList;}

	static ITDataMgr m_pInstance;
private:	
	ITDeviceList m_pDeviceList;

};


#endif // ITDATAMGR_H
