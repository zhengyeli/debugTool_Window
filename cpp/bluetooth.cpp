#include <bluedevice.h>

//#include <devicefinder.h>
#include <QDebug>
#include <QTimer>

// blue
BlueDevice::BlueDevice() {
    qDebug() << "enter bludedevice constructor....";

    //! [devicediscovery-1]
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(5000);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BlueDevice::addDevice);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred, this,
            &BlueDevice::scanError);

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BlueDevice::scanFinished);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &BlueDevice::scanFinished);

        qDebug() << "1";
        m_deviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);

}


void BlueDevice::startSearch()
{
    m_deviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    qDebug() << "2";
}

void BlueDevice::addDevice(const QBluetoothDeviceInfo &device)
{
        qDebug() << "3";
}

void BlueDevice::scanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        //setError(tr("The Bluetooth adaptor is powered off."));
        ;
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        //setError(tr("Writing or reading from the device resulted in an error."));
        ;
    else
        //setError(tr("An unknown error has occurred."));
        ;
}

void BlueDevice::scanFinished()
{
    qDebug() << "scan done";
}

void BlueDevice::connectToService(const QString &address)
{
;
}

bool BlueDevice::scanning() const
{
    return 0;
}

QVariant BlueDevice::devices()
{
return QVariant::fromValue(m_devices);
}
