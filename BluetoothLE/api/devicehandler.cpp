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

#include "devicehandler.h"
#include "deviceinfo.h"

DeviceHandler::DeviceHandler(QObject *parent, QTextEdit *infoTable) :
    BluetoothBaseClass(parent, infoTable)
{
    ;
}

void DeviceHandler::setAddressType(AddressType type)
{
    switch (type) {
    case DeviceHandler::AddressType::PublicAddress:
        m_addressType = QLowEnergyController::PublicAddress;
        break;
    case DeviceHandler::AddressType::RandomAddress:
        m_addressType = QLowEnergyController::RandomAddress;
        break;
    }
}

DeviceHandler::AddressType DeviceHandler::addressType() const
{
    if (m_addressType == QLowEnergyController::RandomAddress)
        return DeviceHandler::AddressType::RandomAddress;

    return DeviceHandler::AddressType::PublicAddress;
}

void DeviceHandler::searchCharacteristic()
{
    const QList<QLowEnergyCharacteristic> chars = m_service->characteristics();
    for (const QLowEnergyCharacteristic &ch : chars)
    {
        qDebug() << "found Characteristic uuid:" << ch.uuid();
    }
    // 订阅特性
    setChar = m_service->characteristic(QBluetoothUuid("00010203-0405-0607-0809-0a0b0c0d2b11"));
    getChar = m_service->characteristic(QBluetoothUuid("00010203-0405-0607-0809-0a0b0c0d2b10"));
    if (!setChar.isValid())
    {
        setError("setChar not found.");
        return;
    }
    else if (!getChar.isValid())
    {
        setError("getChar not found.");
        return;
    }

    m_notificationDesc = getChar.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration); //Descriptor : 0x2902

    if (m_notificationDesc.isValid())
    {
        //打开notification
        m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));//使能通知
        //m_service->readDescriptor(m_notificationDesc);
        //m_service->readCharacteristic(getChar);
        emit connectSuccess();
    }
    else
    {
        setError("m_notificationDesc is null.");
    }
}

void DeviceHandler::disconnectDevice()
{
    if (m_control) {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = nullptr;
    }
}

void DeviceHandler::setDevice(DeviceInfo *device)
{
    m_currentDevice = device;

    // Disconnect and delete old connection
    if (m_control) {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = nullptr;
    }

    // Create new controller and connect it if device available
    if (m_currentDevice) {

        // Make connections
        //! [Connect-Signals-1]
        m_control = QLowEnergyController::createCentral(m_currentDevice->getDevice(), this);
        //! [Connect-Signals-1]
        m_control->setRemoteAddressType(m_addressType);
        //! [Connect-Signals-2]
        connect(m_control, &QLowEnergyController::serviceDiscovered,
                this, &DeviceHandler::serviceDiscovered);
        connect(m_control, &QLowEnergyController::discoveryFinished,
                this, &DeviceHandler::serviceScanDone);

        connect(m_control, &QLowEnergyController::errorOccurred, this,
                [this](QLowEnergyController::Error error) {
            Q_UNUSED(error);
            setError("Cannot connect to remote device.");
        });
        connect(m_control, &QLowEnergyController::connected, this, [this]() {
            setInfo("Controller connected. Search services...");
            m_control->discoverServices();
        });
        connect(m_control, &QLowEnergyController::disconnected, this, [this]() {
            emit disconnectOccur();
            setError("LowEnergy controller disconnected");
        });

        // Connect
        m_control->connectToDevice();
        //! [Connect-Signals-2]
    }
}


//! [Filter HeartRate service 1]
void DeviceHandler::serviceDiscovered(const QBluetoothUuid &gatt)
{
    qDebug() << "serviceDiscovered:" << gatt;
}


//! [Filter HeartRate service 1]

void DeviceHandler::serviceScanDone()
{
    setInfo("Service scan done.");
    QList<QBluetoothUuid>  uuids = m_control->services();

    // Delete old service if available
    if (m_service) {
        delete m_service;
        m_service = nullptr;
    }

    for (int i = 0; i < uuids.count(); i++)
    {
        if (uuids.at(i).toString() == "{00010203-0405-0607-0809-0a0b0c0d1910}")
        {
            m_service = m_control->createServiceObject(QBluetoothUuid(uuids.at(i)),this);
            qDebug() << "select service:" << m_service->serviceName() << m_service->serviceUuid();
        }
    }

    if (!m_service){
        setInfo("m_service fail");
        return;
    }

    if (m_service)
    {
        connect(m_service, &QLowEnergyService::descriptorWritten, this, &DeviceHandler::confirmedDescriptorWrite);
        // 服务状态改变
        connect(m_service, &QLowEnergyService::descriptorRead, this, &DeviceHandler::descriptorRead);
        // 服务状态改变
        connect(m_service, &QLowEnergyService::stateChanged, this, &DeviceHandler::serviceStateChanged);
        // 特征改变（设备发送通知？）
        connect(m_service, &QLowEnergyService::characteristicChanged, this, &DeviceHandler::updateInfoFromDev);
        // 读信息成功
        connect(m_service, &QLowEnergyService::characteristicRead, this, &DeviceHandler::characteristicRead);
        // 写信息
        connect(m_service, &QLowEnergyService::characteristicWritten, this, &DeviceHandler::characteristicWrittenFun);

        m_service->discoverDetails();
    }
    else
    {
        setInfo("m_service is null");
    }
}

// Service functions
//! [Find HRM characteristic]
void DeviceHandler::serviceStateChanged(QLowEnergyService::ServiceState newState)
{
    switch (newState) {
    case QLowEnergyService::RemoteServiceDiscovering:
        setInfo("QLowEnergyService::RemoteServiceDiscovering");
        break;
    case QLowEnergyService::RemoteServiceDiscovered:
    {
        //discovery already done
        setInfo("QLowEnergyService::RemoteServiceDiscovered");
        searchCharacteristic();
    }
        break;
    case QLowEnergyService::RemoteService:
    {
        //未解问题： window需要再次扫描，才能进入QLowEnergyService::RemoteServiceDiscovered
        setInfo("QLowEnergyService::RemoteService");
        QTimer::singleShot(0, this, &DeviceHandler::serviceScanDone);
    }
        break;
    default:
        //nothing for now
        qDebug() << newState;
        break;
    }
}
//! [Find HRM characteristic]

//! [Reading value]
//! notify callback func
void DeviceHandler::updateInfoFromDev(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    // ignore any other characteristic change -> shouldn't really happen though
    if (c.uuid() == QBluetoothUuid(QBluetoothUuid("{00010203-0405-0607-0809-0a0b0c0d2b10}")))
    {
        emit bleMessageChange(value);
//        if (value.toHex().at(0) == 'e' && value.toHex().at(1) == 'e' && value.toHex().at(5) == '0'){
//            showMessages("config wifi sucessful");
//        }
    }
}

void DeviceHandler::descriptorRead(const QLowEnergyDescriptor &d, const QByteArray &value)
{
    qDebug() << "descriptorRead" << d.name() << value;
}

// 读特征 开启通知后 可以触发回调
void DeviceHandler::characteristicRead(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    // ignore any other characteristic change -> shouldn't really happen though
    if (c.uuid() == QBluetoothUuid(QBluetoothUuid("{00010203-0405-0607-0809-0a0b0c0d2b10}")))
    {
        qDebug() << "characteristicRead new data:" << value;
    }
}

// write特征 开启通知后 可以触发回调
void DeviceHandler::characteristicWrittenFun(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    // ignore any other characteristic change -> shouldn't really happen though
    if (c.uuid() == QBluetoothUuid(QBluetoothUuid("{00010203-0405-0607-0809-0a0b0c0d2b10}")))
    {
        qDebug() << "characteristicWrittenFun new data:" << value.toHex();
    }
}

// 写特征
void DeviceHandler::characteristicWrite(const QLowEnergyCharacteristic character,const QByteArray &value)
{
    if (character.uuid() == QBluetoothUuid(QBluetoothUuid("{00010203-0405-0607-0809-0a0b0c0d2b11}")))
    {
        //QByteArray array = text.toLocal8Bit();
        m_service->writeCharacteristic(character,value,QLowEnergyService::WriteWithoutResponse);
        return;
    }
    setError("get unright characteristicWrite uuid");
}


void DeviceHandler::confirmedDescriptorWrite(const QLowEnergyDescriptor &d, const QByteArray &value)
{
    if (d.isValid() && d == m_notificationDesc && value == QByteArray::fromHex("0000")) {
        //disabled notifications -> assume disconnect intent
        qDebug() << "confirmedDescriptorWrite" << value;
        m_control->disconnectFromDevice();
        delete m_service;
        m_service = nullptr;
    }
}

void DeviceHandler::disconnectService()
{
    //disable notifications
    if (m_notificationDesc.isValid() && m_service) {
        m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0000")); //读特征 关闭通知功能
        setInfo("disable m_notificationDesc");
    }
}

void DeviceHandler::continueConnectService()
{
    //enable notifications
    if (m_notificationDesc.isValid() && m_service) {
        m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100")); //读特征 关闭通知功能
        setInfo("enable m_notificationDesc");
    }
}