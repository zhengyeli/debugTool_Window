/***************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the QtBluetooth module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "devicefinder.h"
#include "devicehandler.h"
#include "deviceinfo.h"

DeviceFinder::DeviceFinder(QObject *parent, DeviceHandler *handler, QTextEdit *infoTable):
    BluetoothBaseClass(parent, infoTable)
{
    m_deviceHandler = handler;
    //! [devicediscovery-1]
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
#ifdef Q_OS_LINUX
    m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(5000);
#else
    m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(0);
#endif
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &DeviceFinder::addDevice);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred, this, &DeviceFinder::scanError);

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &DeviceFinder::scanFinished);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &DeviceFinder::scanFinished);
    //! [devicediscovery-1]
}

DeviceFinder::~DeviceFinder()
{
    qDeleteAll(m_devices);
    m_devices.clear();
    setInfo( "Scanning exit.");
}

void DeviceFinder::startSearch()
{
    m_deviceHandler->setDevice(nullptr);
    qDeleteAll(m_devices);
    m_devices.clear();

    setInfo("Scanning for devices...");
    qDebug() << m_deviceDiscoveryAgent;
    m_deviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    setInfo("Scanning start.");
}

void DeviceFinder::stopSearch()
{
    setInfo("Stop scan");
    m_deviceDiscoveryAgent->stop();
}

//! [devicediscovery-3]
void DeviceFinder::addDevice(const QBluetoothDeviceInfo &device)
{
   if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        if (sku.length() == 0)
        {
            m_devices.append(new DeviceInfo(device));
        }
        else if (QString(device.name()).contains(sku, Qt::CaseInsensitive)) // 匹配不区分大小写
        {
            m_devices.append(new DeviceInfo(device));
        }
        emit scanDeviceResult(device);
        setInfo("found :  " + device.name());
    }
}

//! [devicediscovery-4]

void DeviceFinder::scanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError){
        setInfo("The Bluetooth adaptor is powered off.") ;
    }
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError){
        setInfo("Writing or reading from the device resulted in an error.") ;
    }
    else{
        setInfo("An unknown error has occurred.") ;
    }
}

void DeviceFinder::scanFinished()
{

    if (m_devices.isEmpty())
    {
        setInfo("No Need Low Energy devices found.");
    }
    else
    {
        setInfo("Low Energy device Scan finish...");
    }
}

void DeviceFinder::connectToService(const QString &name)
{
    m_deviceDiscoveryAgent->stop();
    DeviceInfo *currentDevice = nullptr;
    for (QObject *entry : qAsConst(m_devices)) {
        auto device = qobject_cast<DeviceInfo *>(entry);
        //if (device && device->getAddress() == address ) {
        if (device && device->getName() == name ) {
            currentDevice = device;
            setInfo("connect to:");
            setInfo(name);
            break;
        }
    }

    if (currentDevice)
       m_deviceHandler->setDevice(currentDevice);
}

bool DeviceFinder::scanning() const
{
   return m_deviceDiscoveryAgent->isActive();
}

QVariant DeviceFinder::devices()
{
   return QVariant::fromValue(m_devices);
}