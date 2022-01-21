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

#ifndef DEVICEHANDLER_H
#define DEVICEHANDLER_H

#include "bluetoothbaseclass.h"

#include <QDateTime>
#include <QList>
#include <QTimer>

#include <QLowEnergyController>
#include <QLowEnergyService>

class DeviceInfo;

class DeviceHandler : public BluetoothBaseClass
{
    Q_OBJECT
    Q_PROPERTY(AddressType addressType READ addressType WRITE setAddressType)

public:
    enum class AddressType {
        PublicAddress,
        RandomAddress
    };
    Q_ENUM(AddressType)

    DeviceHandler(QObject *parent = nullptr);

    void setDevice(DeviceInfo *device);
    void setAddressType(AddressType type);
    void disconnectDevice();
    AddressType addressType() const;

signals:
    void measuringChanged();
    void aliveChanged();
    void statsChanged();
    void characteristicsUpdated();

public slots:
    void disconnectService();
    void continueConnectService();
    void keepalive();

public:
    //QLowEnergyController
    void serviceDiscovered(const QBluetoothUuid &);
    void serviceScanDone();
    void calculate(unsigned char *data);

    //QLowEnergyService
    void descriptorRead(const QLowEnergyDescriptor &d,
                            const QByteArray &value);
    void serviceStateChanged(QLowEnergyService::ServiceState s);
    void bledebugserviceStateChanged(QLowEnergyService::ServiceState s);

    void updateInfoFromDev(const QLowEnergyCharacteristic &c,
                              const QByteArray &value);
    void confirmedDescriptorWrite(const QLowEnergyDescriptor &d,
                              const QByteArray &value);
    void characteristicRead(const QLowEnergyCharacteristic &c,
                            const QByteArray &value);
    void characteristicWrite(QLowEnergyCharacteristic character ,QByteArray &value);

    void addMeasurement(int value);

// ble debug
    void bledebugupdateInfoFromDev(const QLowEnergyCharacteristic &c,
                              const QByteArray &value);
    void bledebugdescriptorRead(const QLowEnergyDescriptor &d,
                            const QByteArray &value);


    QLowEnergyController *m_control = nullptr;
    QLowEnergyService *m_service = nullptr;
    QLowEnergyService *m_service_bledebug = nullptr;
    QLowEnergyDescriptor m_bledebugnotificationDesc, m_notificationDesc;
    DeviceInfo *m_currentDevice = nullptr;
    QLowEnergyCharacteristic bledebugsetChar,setChar;
    QLowEnergyCharacteristic bledebuggetChar,getChar;

    // Statistics
    QDateTime m_start;
    QDateTime m_stop;

    QList<int> m_measurements;
    QLowEnergyController::RemoteAddressType m_addressType = QLowEnergyController::PublicAddress;

};

#endif // DEVICEHANDLER_H
