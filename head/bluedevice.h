#ifndef BLUEDEVICE_H
#define BLUEDEVICE_H

#include <QtCore/QObject>

#include <QObject>

#include <QBluetoothLocalDevice>
#include <QTimer>
#include <QVariant>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QDateTime>
#include <QList>
#include <QTimer>
#include <QString>
#include <QObject>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>

class BlueDevice;

class BlueDevice: public QObject{
    Q_OBJECT

    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(QVariant devices READ devices NOTIFY devicesChanged)
public:

    BlueDevice();
    QString getAddress(QBluetoothDeviceInfo device) const;

    bool scanning() const;
    QVariant devices();

public slots:
    void startSearch();
    void connectToService(const QString &address);

private slots:
    void addDevice(const QBluetoothDeviceInfo&);
    void scanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void scanFinished();

signals:
    void scanningChanged();
    void devicesChanged();
private:
        QList<QBluetoothDeviceInfo> device_list;  //存放搜索到到蓝牙设备列表
        QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;  //设备搜索对象
        QLowEnergyController *m_controler;   //单个蓝牙设备控制器
        QLowEnergyService *m_service; //服务对象实例
        QList<QObject*> m_devices;
};

#endif // BLUEDEVICE_H
