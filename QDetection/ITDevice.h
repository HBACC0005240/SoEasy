#ifndef ITDEVICE_H
#define ITDEVICE_H

#include "stdafx.h"
#include <QObject>
class ITDevice;
typedef QList<ITDevice*> ITDeviceList;
class ITDevice : public QObject
{
	Q_OBJECT

public:
	ITDevice();
	~ITDevice();

	void setDeviceCode(const QString& strCode){m_strDeviceCode=strCode;}
	QString getDeviceCode(){return m_strDeviceCode;}
	void setVehicleNum(const QString& strNum){m_strVehicleNum=strNum;}
	QString getVehicleNum(){return m_strVehicleNum;}
	void setVehicleImage(const QImage& image){m_vehicleImage=image;}
	QImage getVehicleImage(){return m_vehicleImage;}
private:
	QImage m_vehicleImage;
	QString	m_strVehicleNum;
	QString m_strDeviceCode;

};

#endif // ITDEVICE_H
