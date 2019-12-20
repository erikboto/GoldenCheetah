#include "VMProConfigurator.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>

#define VO2MASTERPRO_COMIN_CHAR_UUID "{00001525-1212-EFDE-1523-785FEABCD123}"
#define VO2MASTERPRO_COMOUT_CHAR_UUID "{00001526-1212-EFDE-1523-785FEABCD123}"

VMProConfigurator::VMProConfigurator(QLowEnergyService *service, QObject *parent) : QObject(parent),
    m_service(service)
{
    // Set up the two charasteristics used to query and control device settings
    m_comInChar =  service->characteristic(QBluetoothUuid(QString(VO2MASTERPRO_COMIN_CHAR_UUID)));
    m_comOutChar =  service->characteristic(QBluetoothUuid(QString(VO2MASTERPRO_COMOUT_CHAR_UUID)));
    connect(m_service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
            this, SLOT(onDeviceReply(QLowEnergyCharacteristic,QByteArray)));

    const QLowEnergyDescriptor notificationDesc = m_comOutChar.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
    if (notificationDesc.isValid()) {
        service->writeDescriptor(notificationDesc, QByteArray::fromHex("0100"));
    }
}

void VMProConfigurator::setUserPieceSize(VMProVenturiSize size)
{
    // Trigger a write, the unit should reply on comOut with:
    //
    if (m_comInChar.isValid())
    {
        QByteArray cmd;
        cmd.append(VM_BLE_SET_VENTURI_SIZE);
        cmd.append(size);
        m_service->writeCharacteristic(m_comInChar, cmd);
    }
}

void VMProConfigurator::setVolumeCorrectionMode(VMProVolumeCorrectionMode mode)
{
    // Trigger a write, the unit should reply on comOut with:
    //
    if (m_comInChar.isValid())
    {
        QByteArray cmd;
        cmd.append(VM_BLE_SET_VO2_REPORT_MODE);
        cmd.append(mode);
        m_service->writeCharacteristic(m_comInChar, cmd);
    }
}

void VMProConfigurator::setIdleTimeout(VMProIdleTimeout timeoutState)
{
    // Trigger a write, the unit should reply on comOut with:
    //
    if (m_comInChar.isValid())
    {
        QByteArray cmd;
        cmd.append(VM_BLE_SET_IDLE_TIMEOUT_MODE);
        cmd.append(timeoutState);
        m_service->writeCharacteristic(m_comInChar, cmd);
    }
}

void VMProConfigurator::getUserPieceSize()
{
    // Trigger a write, the unit should reply on comOut
    //
    if (m_comInChar.isValid())
    {
        QByteArray cmd;
        cmd.append(VM_BLE_GET_VENTURI_SIZE);
        cmd.append('\0');
        m_service->writeCharacteristic(m_comInChar, cmd);
    }
}

void VMProConfigurator::getIdleTimeout()
{
    // Trigger a write, the unit should reply on comOut
    //
    if (m_comInChar.isValid())
    {
        QByteArray cmd;
        cmd.append(VM_BLE_GET_IDLE_TIMEOUT_MODE);
        cmd.append('\0');
        m_service->writeCharacteristic(m_comInChar, cmd);
    }
}

void VMProConfigurator::getVolumeCorrectionMode()
{
    // Trigger a write, the unit should reply on comOut
    //
    if (m_comInChar.isValid())
    {
        QByteArray cmd;
        cmd.append(VM_BLE_GET_VO2_REPORT_MODE);
        cmd.append('\0');
        m_service->writeCharacteristic(m_comInChar, cmd);
    }
}

void VMProConfigurator::onDeviceReply(const QLowEnergyCharacteristic &c,
                                      const QByteArray &value)
{
    qDebug() << "onDeviceReply: " << c.uuid().toString();

    // Return if it's not the ComOut char that has been updated
    if (c.uuid() != QBluetoothUuid(QString(VO2MASTERPRO_COMOUT_CHAR_UUID)))
    {
        return;
    }

    if (value.length() == 2)
    {
        VMProCommand cmd = static_cast<VMProCommand>(value[0]);
        char data = value[1];

        switch (cmd) {
        case VM_BLE_UNKNOWN_RESPONSE:
            qDebug() << "Got VM_BLE_UNKNOWN_RESPONSE" << (int)data;
            //appendDbgInfo("Got VM_BLE_UNKNOWN_RESPONSE");
            break;
        case VM_BLE_SET_STATE:
            qDebug() << "Got VM_BLE_SET_STATE" << (int)data;
            break;
        case VM_BLE_GET_STATE:
            qDebug() << "Got VM_BLE_GET_STATE" << (int)data;
            //appendDbgInfo(QString("Device State: %1").arg((int)data));
            break;
        case VM_BLE_SET_VENTURI_SIZE:
            qDebug() << "Got VM_BLE_SET_VENTURI_SIZE" << (int)data;
            break;
        case VM_BLE_GET_VENTURI_SIZE:
            qDebug() << "Got VM_BLE_GET_VENTURI_SIZE" << (int)data;
            //appendDbgInfo(QString("User Piece Size %1").arg((int)data));
            emit userPieceSizeChanged(static_cast<VMProVenturiSize>(data));
            break;
        case VM_BLE_GET_CALIB_PROGRESS:
            qDebug() << "Got VM_BLE_GET_CALIB_PROGRESS" << (int)data;
            //appendDbgInfo(QString("Calibration Progress %1 %").arg((int)data));
            break;
        case VM_BLE_ERROR:
            qDebug() << "Got ErrorCode: " << (int)data;
            break;
        case VM_BLE_SET_VO2_REPORT_MODE:
            qDebug() << "Got VM_BLE_SET_VO2_REPORT_MODE" << (int)data;
            break;
        case VM_BLE_GET_VO2_REPORT_MODE:
            qDebug() << "Got VM_BLE_GET_VO2_REPORT_MODE" << (int)data;
            emit volumeCorrectionModeChanged(static_cast<VMProVolumeCorrectionMode>(data));
            break;
        case VM_BLE_GET_O2_CELL_AGE:
            qDebug() << "Got VM_BLE_GET_O2_CELL_AGE: " << (int)data;
            break;
        case VM_BLE_RESET_O2_CELL_AGE:
            qDebug() << "Got VM_BLE_RESET_O2_CELL_AGE: " << (int)data;
            break;
        case VM_BLE_SET_IDLE_TIMEOUT_MODE:
            qDebug() << "Got VM_BLE_SET_IDLE_TIMEOUT_MODE: " << (int)data;
            break;
        case VM_BLE_GET_IDLE_TIMEOUT_MODE:
            qDebug() << "Got VM_BLE_GET_IDLE_TIMEOUT_MODE" << (int)data;
            emit idleTimeoutStateChanged(static_cast<VMProIdleTimeout>(data));
            break;
        case VM_BLE_SET_AUTO_RECALIB_MODE:
            qDebug() << "Got VM_BLE_SET_AUTO_RECALIB_MODE: " << (int)data;
            break;
        case VM_BLE_GET_AUTO_RECALIB_MODE:
            qDebug() << "Got VM_BLE_GET_AUTO_RECALIB_MODE: " << (int)data;
            break;
        case VM_BLE_BREATH_STATE_CHANGED:
            qDebug() << "Got breath state changed: " << (int)data;
            break;
        //default:
        //    qDebug() << "VMProConfigurator::onDeviceReply Received unexpected reply from device";
        }
    } else {
        qDebug() << "VMProConfigurator::onDeviceReply with unexpected length: " << value.length();
    }
}


