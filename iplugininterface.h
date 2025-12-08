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

#ifndef IPLUGININTERFACE_H
#define IPLUGININTERFACE_H

#include <QString>
#include <QObject>
#include <QVector>

// Device information structure
struct DeviceInfo {
    QString name;           // Device name/model
    QString serialNumber;   // Serial number or unique identifier
    QString address;        // IP address, COM port, USB path, etc.
    QString type;           // Connection type (LAN, USB, GPIB, Serial, etc.)
    bool isAvailable;       // Whether device is available for connection
    
    DeviceInfo() : isAvailable(true) {}
};

// Peak data structure for Signal Analyzer
struct Peak {
    double frequencyHz;
    double leveldBm;
};

// Positioner data structures
struct Step {
    double AZ;
    double EL;
    double POL;
    double X;
    double Y;
    double V;
};

struct MinRange {
    double AZ;
    double EL;
    double POL;
    double X;
    double Y;
    double V;
};

struct MaxRange {
    double AZ;
    double EL;
    double POL;
    double X;
    double Y;
    double V;
};

struct Movement {
    double AZ;
    double EL;
    double POL;
    double X;
    double Y;
    double V;
};

// Plugin interface for Signal Analyzer
class ISignalAnalyzerPlugin : public QObject
{
    Q_OBJECT
public:
    virtual ~ISignalAnalyzerPlugin() = default;
    
    // Device discovery
    virtual QVector<DeviceInfo> scanDevices() = 0;
    virtual bool connectToDevice(const QString &address) = 0;
    
    // Connection management
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    
    // Configuration
    virtual void setStartFreq(double freqHz) = 0;
    virtual void setStopFreq(double freqHz) = 0;
    virtual void setRBW(double freqHz) = 0;
    
    // Measurement
    virtual Peak findPeak() = 0;
    
signals:
    void connected();
    void disconnected();
    void peakFound(const Peak &peak);
    void errorOccurred(const QString &error);
    void devicesScanned(const QVector<DeviceInfo> &devices);
};

// Plugin interface for Signal Generator
class ISignalGeneratorPlugin : public QObject
{
    Q_OBJECT
public:
    virtual ~ISignalGeneratorPlugin() = default;
    
    // Device discovery
    virtual QVector<DeviceInfo> scanDevices() = 0;
    virtual bool connectToDevice(const QString &address) = 0;
    
    // Connection management
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    
    // Configuration
    virtual void setFreq(double freqHz) = 0;
    virtual void setPower(double powerDbm) = 0;
    
    // RF Control
    virtual void enableRf() = 0;
    virtual void disableRf() = 0;
    virtual bool isRfEnabled() const = 0;
    
signals:
    void connected();
    void disconnected();
    void rfEnabled();
    void rfDisabled();
    void errorOccurred(const QString &error);
    void devicesScanned(const QVector<DeviceInfo> &devices);
};

// Plugin interface for Positioner
class IPositionerPlugin : public QObject
{
    Q_OBJECT
public:
    virtual ~IPositionerPlugin() = default;
    
    // Device discovery
    virtual QVector<DeviceInfo> scanDevices() = 0;
    virtual bool connectToDevice(const QString &address) = 0;
    
    // Connection management
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    
    // Configuration
    virtual void setAZStep(double degrees) = 0;
    virtual void setStep(const Step &step) = 0;
    virtual void setMinRange(const MinRange &minRange) = 0;
    virtual void setMaxRange(const MaxRange &maxRange) = 0;
    virtual void setMovement(const Movement &movement) = 0;
    virtual void setDistance(double distance) = 0;
    
    // Control
    virtual void start() = 0;
    virtual void stop() = 0;
    
signals:
    void connected();
    void disconnected();
    void movementStarted();
    void movementStopped();
    void positionChanged(double az, double el, double pol);
    void errorOccurred(const QString &error);
    void devicesScanned(const QVector<DeviceInfo> &devices);
};

// Factory function type definitions
typedef ISignalAnalyzerPlugin* (*CreateSignalAnalyzerPluginFunc)();
typedef ISignalGeneratorPlugin* (*CreateSignalGeneratorPluginFunc)();
typedef IPositionerPlugin* (*CreatePositionerPluginFunc)();
typedef void (*DestroyPluginFunc)(QObject*);

#define SIGNAL_ANALYZER_PLUGIN_IID "id.co.fusi.antenna.ISignalAnalyzerPlugin/1.0"
#define SIGNAL_GENERATOR_PLUGIN_IID "id.co.fusi.antenna.ISignalGeneratorPlugin/1.0"
#define POSITIONER_PLUGIN_IID "id.co.fusi.antenna.IPositionerPlugin/1.0"

Q_DECLARE_INTERFACE(ISignalAnalyzerPlugin, SIGNAL_ANALYZER_PLUGIN_IID)
Q_DECLARE_INTERFACE(ISignalGeneratorPlugin, SIGNAL_GENERATOR_PLUGIN_IID)
Q_DECLARE_INTERFACE(IPositionerPlugin, POSITIONER_PLUGIN_IID)

#endif // IPLUGININTERFACE_H
