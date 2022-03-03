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


QTimer *timer = nullptr;

DeviceHandler::DeviceHandler(QObject *parent) :
    BluetoothBaseClass(parent)
{

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
    for (const QLowEnergyCharacteristic &ch : chars) {
        qDebug() << "found Characteristic uuid:" << ch.uuid();
    }
    // 订阅特性
    setChar = m_service->characteristic(QBluetoothUuid("00010203-0405-0607-0809-0a0b0c0d2b11"));
    getChar = m_service->characteristic(QBluetoothUuid("00010203-0405-0607-0809-0a0b0c0d2b10"));
    if (!setChar.isValid()) {
        setError("setChar not found.");
        return;
    }else if (!getChar.isValid()) {
        setError("getChar not found.");
        return;
    }

    //const QLowEnergyCharacteristic hrChar = m_service->characteristic(QBluetoothUuid::CharacteristicType::ServiceChanged); //character : 0x2a05
    m_notificationDesc = getChar.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration); //Descriptor : 0x2902
    if (m_notificationDesc.isValid())
    {
        //打开notification
        m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));//使能通知
        m_service->readDescriptor(m_notificationDesc);
        m_service->readCharacteristic(getChar);

        // 心跳定时器
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(keepalive()));
        timer->start(3000);

        showMessages("connect sucess");
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
    if (timer)
    {
        timer->stop();
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

    if (m_service_bledebug) {
        delete m_service_bledebug;
        m_service_bledebug = nullptr;
    }

    for (int i = 0; i < uuids.count(); i++)
    {
        if (uuids.at(i).toString() == "{00010203-0405-0607-0809-0a0b0c0d1910}")
        {
            m_service = m_control->createServiceObject(QBluetoothUuid(uuids.at(i)),this);
            qDebug() << "select service:" << m_service->serviceName() << m_service->serviceUuid();
        }
        else if (uuids.at(i).toString() == "{00010203-0405-0607-0809-0a0b0c0d2c00}")
        {
            m_service_bledebug = m_control->createServiceObject(QBluetoothUuid(uuids.at(i)),this);
            qDebug() << "select m_service_bledebug:" << m_service_bledebug->serviceName() << m_service_bledebug->serviceUuid();
        }
    }

    if (!m_service && !m_service_bledebug){
        setInfo("m_control->createServiceObject all fail");
        return;
    }

    if (m_service)
    {
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

    if (m_service_bledebug)
    {
        connect(m_service_bledebug, &QLowEnergyService::descriptorRead, this, &DeviceHandler::bledebugdescriptorRead);
        // 服务状态改变
        connect(m_service_bledebug, &QLowEnergyService::stateChanged, this, &DeviceHandler::bledebugserviceStateChanged);
        // 特征改变（设备发送通知？）
        connect(m_service_bledebug, &QLowEnergyService::characteristicChanged, this, &DeviceHandler::bledebugupdateInfoFromDev);

        m_service_bledebug->discoverDetails();
    }
    else
    {
        setInfo("m_service_bledebug is null");
    }
}

// Service functions
//! [Find HRM characteristic]
void DeviceHandler::serviceStateChanged(QLowEnergyService::ServiceState newState)
{
    switch (newState) {
    case QLowEnergyService::RemoteServiceDiscovering:
        setInfo("ble : QLowEnergyService::RemoteServiceDiscovering");
        break;
    case QLowEnergyService::RemoteServiceDiscovered:
    {
        //discovery already done
        setInfo("ble : QLowEnergyService::RemoteServiceDiscovered");
        searchCharacteristic();
    }
        break;
    case QLowEnergyService::RemoteService:
    {
        //未解问题： window需要再次扫描，才能进入QLowEnergyService::RemoteServiceDiscovered
        setInfo("ble : QLowEnergyService::RemoteService");
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
        setInfo("recive :" + value.toHex());
        if (value.toHex().at(2) == '0' && (value.toHex().at(3) == '1' || value.toHex().at(3) == '0')){
        }
        else{
            blelinkwindow::receive(value.toHex());
        }
        return;
    }
}

void DeviceHandler::descriptorRead(const QLowEnergyDescriptor &d, const QByteArray &value)
{
    qDebug() << d.name() << value.toHex();;
}

// 读特征 开启通知后 可以触发回调
void DeviceHandler::characteristicRead(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    // ignore any other characteristic change -> shouldn't really happen though
    if (c.uuid() == QBluetoothUuid(QBluetoothUuid("{00010203-0405-0607-0809-0a0b0c0d2b10}")))
    {
        qDebug() << "characteristicRead new data:" << value.toHex();
    }
}

// write特征 开启通知后 可以触发回调
void DeviceHandler::characteristicWrittenFun(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    // ignore any other characteristic change -> shouldn't really happen though
    if (c.uuid() == QBluetoothUuid(QBluetoothUuid("{00010203-0405-0607-0809-0a0b0c0d2b10}")))
    {
        qDebug() << "characteristicRead new data:" << value.toHex();
    }
}

// 写特征
void DeviceHandler::characteristicWrite(const QLowEnergyCharacteristic character,const QByteArray &value)
{
    if (character.uuid() == QBluetoothUuid(QBluetoothUuid("{00010203-0405-0607-0809-0a0b0c0d2b11}")))
    {
        //QByteArray array = text.toLocal8Bit();
        m_service->writeCharacteristic(character,value,QLowEnergyService::WriteWithoutResponse);
        setInfo("send   :" + value.toHex());
        return;
    }
    else if (character.uuid() == QBluetoothUuid(QBluetoothUuid("{00010203-0405-0607-0809-0a0b0c0d2c21}")))
    {
        m_service_bledebug->writeCharacteristic(character,value,QLowEnergyService::WriteWithoutResponse);
        //setInfo("send   :" + value.toHex());
        return;
    }
    setInfo("err :  get unright characteristicWrite uuid");
}


void DeviceHandler::confirmedDescriptorWrite(const QLowEnergyDescriptor &d, const QByteArray &value)
{
    if (d.isValid() && d == m_notificationDesc && value == QByteArray::fromHex("0000")) {
        //disabled notifications -> assume disconnect intent
        m_control->disconnectFromDevice();
        delete m_service;
        m_service = nullptr;
    }
}

void DeviceHandler::disconnectService()
{
    //disable notifications
    if (m_notificationDesc.isValid() && m_service && m_notificationDesc.value() == QByteArray::fromHex("1000")) {
        m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0000")); //读特征 关闭通知功能
        setInfo("disable m_notificationDesc");
    }

    //disable notifications
    if (m_bledebugnotificationDesc.isValid() && m_service_bledebug && m_bledebugnotificationDesc.value() == QByteArray::fromHex("1000")) {
        m_service_bledebug->writeDescriptor(m_bledebugnotificationDesc, QByteArray::fromHex("0000")); //读特征 关闭通知功能
        setInfo("disable m_bledebugnotificationDesc");
    }
}

void DeviceHandler::continueConnectService()
{
    //enable notifications
    if (m_notificationDesc.isValid() && m_service && m_notificationDesc.value() == QByteArray::fromHex("0000")) {
        m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100")); //读特征 关闭通知功能
        setInfo("enable m_notificationDesc");
    }

    //enable notifications
    if (m_bledebugnotificationDesc.isValid() && m_service_bledebug && m_bledebugnotificationDesc.value() == QByteArray::fromHex("0000")) {
        m_service_bledebug->writeDescriptor(m_bledebugnotificationDesc, QByteArray::fromHex("0100")); //读特征 关闭通知功能
        setInfo("enable m_bledebugnotificationDesc");
    }
}

// 计算校验和
void DeviceHandler::calculate(uint8_t *data)
{
    uint8_t temp = 0;
    for(int i = 0; i < 19; i++)
    {
        temp = temp ^ data[i];
    }
    data[19] = temp;
}

void DeviceHandler::keepalive()
{
    QByteArray array;
    uint8_t data[20] = {0xaa,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    calculate(data);
    array = QByteArray((char*)data,20);
    characteristicWrite(setChar,array);
}






/*-----------------------------------------------------------------
 *
 *
 *
 *             以下是蓝牙无线打印相关函数
 *-----------------------------------------------------------------
*/

// Service functions
//! [Find HRM characteristic]
void DeviceHandler::bledebugserviceStateChanged(QLowEnergyService::ServiceState newState)
{
    switch (newState) {
    case QLowEnergyService::RemoteServiceDiscovering:
        setInfo("bledebug : QLowEnergyService::RemoteServiceDiscovering");
        break;
    case QLowEnergyService::RemoteServiceDiscovered:
    {
        //discovery already done
        setInfo("bledebug : QLowEnergyService::RemoteServiceDiscovered");
        const QList<QLowEnergyCharacteristic> chars = m_service_bledebug->characteristics();
        for (const QLowEnergyCharacteristic &ch : chars) {
            qDebug() << "found Characteristic uuid:" << ch.uuid();
        }
        // 订阅特性
        bledebuggetChar = m_service_bledebug->characteristic(QBluetoothUuid("00010203-0405-0607-0809-0a0b0c0d2c20"));
        bledebugsetChar = m_service_bledebug->characteristic(QBluetoothUuid("00010203-0405-0607-0809-0a0b0c0d2c21"));
        if (!bledebugsetChar.isValid()) {
            setError("bledebugsetChar not found.");
            break;
        }else if (!bledebuggetChar.isValid()) {
            setError("bledebuggetChar not found.");
            break;
        }

        //const QLowEnergyCharacteristic hrChar = m_service->characteristic(QBluetoothUuid::CharacteristicType::ServiceChanged); //character : 0x2a05
        m_bledebugnotificationDesc = bledebuggetChar.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration); //Descriptor : 0x2902
        //打开notification
        m_service_bledebug->writeDescriptor(m_bledebugnotificationDesc, QByteArray::fromHex("0100"));//使能通知
        m_service_bledebug->readDescriptor(m_bledebugnotificationDesc);
        m_service_bledebug->readCharacteristic(bledebuggetChar);

    }
        break;
    case QLowEnergyService::RemoteService:
    {
        //未解问题： 需要再次扫描，才能进入QLowEnergyService::RemoteServiceDiscovered
        setInfo("bledebug : QLowEnergyService::RemoteService");
        QTimer::singleShot(0, this, &DeviceHandler::serviceScanDone);
    }
        break;
    default:
        //nothing for now
        break;
    }
}
//! [Find HRM characteristic]

//! [Reading value]
void DeviceHandler::bledebugupdateInfoFromDev(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    // ignore any other characteristic change -> shouldn't really happen though
    if (c.uuid() == QBluetoothUuid(QBluetoothUuid("{00010203-0405-0607-0809-0a0b0c0d2c20}")))
    {
        qDebug() << value;
        bledebugwindow::debugtext_append(value);
        return;
    }
}

void DeviceHandler::bledebugdescriptorRead(const QLowEnergyDescriptor &d, const QByteArray &value)
{
    qDebug() << d.name() <<"descriptorRead have changed: " << value.toHex();;
}
