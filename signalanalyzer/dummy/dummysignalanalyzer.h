/****************************************************************************
**
** Copyright (C) 2025 PT Fusi Global Teknologi. All rights reserved.
** Coded by: Yan Syafri Hidayat
**
** This file is part of the Antenna Tester GUI plugin interface.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
****************************************************************************/

#ifndef DUMMYSIGNALANALYZER_H
#define DUMMYSIGNALANALYZER_H

#include "iplugininterface.h"
#include <QObject>
#include <QString>
#include <QTimer>
#include <QRandomGenerator>

class DummySignalAnalyzer : public ISignalAnalyzerPlugin
{
    Q_OBJECT
    Q_INTERFACES(ISignalAnalyzerPlugin)
    
public:
    DummySignalAnalyzer();
    virtual ~DummySignalAnalyzer();
    
    // Device discovery
    QVector<DeviceInfo> scanDevices() override;
    bool connectToDevice(const QString &address) override;
    
    // Connection management
    bool connect() override;
    void disconnect() override;
    bool isConnected() const override;
    
    // Configuration
    void setStartFreq(double freqHz) override;
    void setStopFreq(double freqHz) override;
    void setRBW(double freqHz) override;
    
    // Measurement
    Peak findPeak() override;
    
signals:
    void connected();
    void disconnected();
    void peakFound(const Peak &peak);
    void errorOccurred(const QString &error);
    
private:
    bool m_isConnected;
    double m_startFreqHz;
    double m_stopFreqHz;
    double m_rbwHz;
    QString m_connectedAddress;
};

#endif // DUMMYSIGNALANALYZER_H
