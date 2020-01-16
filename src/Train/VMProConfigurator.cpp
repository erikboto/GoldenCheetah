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
            emit calibrationProgressChanged(data);
            qDebug() << "Got VM_BLE_GET_CALIB_PROGRESS" << (int)data;
            //appendDbgInfo(QString("Calibration Progress %1 %").arg((int)data));
            break;
        case VM_BLE_ERROR:
            qDebug() << VMProErrorToStringHelper::errorDescription((int)data);
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

QString VMProErrorToStringHelper::errorDescription(int errorCode)
{
    switch(errorCode)
    {
    //VM_ERROR_FATAL
    case (FatalErrorOffset + 0):        //VM_FATAL_ERROR_NONE
        return "No Error.";
    case (FatalErrorOffset + 1):        //VM_FATAL_ERROR_INIT
        return "Initialization error, shutting off.";
    case (FatalErrorOffset + 2):        //VM_FATAL_ERROR_TOO_HOT
        return "Too hot, shutting off.";
    case (FatalErrorOffset + 3):        //VM_FATAL_ERROR_TOO_COLD
        return "Too cold, shutting off.";
    case (FatalErrorOffset + 4):        //VM_FATAL_ERROR_IDLE_TIMEOUT
        return "Sat idle too long, shutting off.";
    case (FatalErrorOffset + 5):        //VM_FATAL_ERROR_DEAD_BATTERY_LBO_V
    case (FatalErrorOffset + 6):        //VM_FATAL_ERROR_DEAD_BATTERY_SOC
    case (FatalErrorOffset + 7):        //VM_FATAL_ERROR_DEAD_BATTERY_STARTUP
        return "Battery is out of charge, shutting off.";
    case (FatalErrorOffset + 8):        //VM_FATAL_ERROR_BME_NO_INIT
        return "Failed to initialize environmental sensor.\n Send unit in for service.";
    case (FatalErrorOffset + 9):        //VM_FATAL_ERROR_ADS_NO_INIT
        return "Failed to initialize oxygen sensor.\n Send unit in for service.";
    case (FatalErrorOffset + 10):        //VM_FATAL_ERROR_AMS_DISCONNECTED
        return "Failed to initialize flow sensor.\n Send unit in for service.";
    case (FatalErrorOffset + 11):        //VM_FATAL_ERROR_TWI_NO_INIT
        return "Failed to initialize sensor communication.\n Send unit in for service.";
    case (FatalErrorOffset + 12):        //VM_FATAL_ERROR_FAILED_FLASH_WRITE
        return "Failed to write a page to flash memory.\n Send unit in for service.";
    case (FatalErrorOffset + 13):        //VM_FATAL_ERROR_FAILED_FLASH_ERASE
        return "Failed to erase a page from flash memory.\n Send unit in for service.";

    //VM_ERROR_WARN
    case (WarningErrorOffset + 0):      //VM_WARN_ERROR_NONE
        return "No warning.";
    case (WarningErrorOffset + 1):      //VM_WARN_ERROR_MASK_LEAK
        return "Mask leak detected.";
    case (WarningErrorOffset + 2):      //VM_WARN_ERROR_VENTURI_TOO_SMALL
        return "User piece too small.";
    case (WarningErrorOffset + 3):      //VM_WARN_ERROR_VENTURI_TOO_BIG
        return "User piece too big.";
    case (WarningErrorOffset + 4):      //VM_WARN_ERROR_TOO_HOT
        return "Device very hot.";
    case (WarningErrorOffset + 5):      //VM_WARN_ERROR_TOO_COLD
        return "Device very cold.";
    case (WarningErrorOffset + 6):      //VM_WARN_ERROR_UNDER_BREATHING_VALVE
        return "Breathing less than valve trigger.";
    case (WarningErrorOffset + 7):      //VM_WARN_ERROR_O2_TOO_HUMID
        return "Oxygen sensor too humid.";
    case (WarningErrorOffset + 8):      //VM_WARN_ERROR_O2_TOO_HUMID_END
        return "Oxygen sensor dried.";
    case (WarningErrorOffset + 9):      //VM_WARN_ERROR_BREATHING_DURING_DIFFP_CALIB
        return "Breathing during ambient calibration.\n Hold your breath for 5 seconds.";
    case (WarningErrorOffset + 10):     //VM_WARN_ERROR_TOO_MANY_CONSECUTIVE_BREATHS_REJECTED
        return "Many breaths rejected.";
    case (WarningErrorOffset + 11):     //VM_WARN_ERROR_LOW_BATTERY_VOLTAGE
        return "Low battery.";
    case (WarningErrorOffset + 12):     //VM_WARN_ERROR_THERMAL_SHOCK_BEGIN
        return "Thermal change occurring.";
    case (WarningErrorOffset + 13):     //VM_WARN_ERROR_THERMAL_SHOCK_END
        return "Thermal change slowed.";
    case (WarningErrorOffset + 14):     //VM_WARN_ERROR_FINAL_VE_OUT_OF_RANGE
        return "Ventilation out of range.";

    //VM_ERROR_O2_RECALIB
    case (O2CalibrationErrorOffset + 0):    //VM_O2_RECALIB_NONE
        return "No calibration message.";
    case (O2CalibrationErrorOffset + 1):    //VM_O2_RECALIB_DRIFT
        return "O2 sensor signal drifted.";
    case (O2CalibrationErrorOffset + 2):    //VM_O2_RECALIB_PRESSURE_DRIFT
        return "Ambient pressure changed a lot.";
    case (O2CalibrationErrorOffset + 3):    //VM_O2_RECALIB_TEMPERATURE_DRIFT
        return "Temperature changed a lot.";
    case (O2CalibrationErrorOffset + 4):    //VM_O2_RECALIB_TIME_MAX
        return "Maximum time between calibrations reached.";
    case (O2CalibrationErrorOffset + 5):    //VM_O2_RECALIB_TIME_5MIN
        return "5 minute recalibration.";
    case (O2CalibrationErrorOffset + 6):    //VM_O2_RECALIB_REASON_THERMAL_SHOCK_OVER
        return "Post-thermal shock calibration.";

    //VM_ERROR_DIAG
    case (DiagnosticErrorOffset + 0):       //VM_DIAG_ERROR_FLOW_DELAY_RESET
        return "Calibration: waiting for user to start breathing.";
    case (DiagnosticErrorOffset + 1):       //VM_DIAG_ERROR_BREATH_TOO_JITTERY
        return "Breath rejected; too jittery.";
    case (DiagnosticErrorOffset + 2):       //VM_DIAG_ERROR_SEGMENT_TOO_SHORT
        return "Breath rejected; segment too short.";
    case (DiagnosticErrorOffset + 3):       //VM_DIAG_ERROR_BREATH_TOO_SHORT
        return "Breath rejected; breath too short.";
    case (DiagnosticErrorOffset + 4):       //VM_DIAG_ERROR_BREATH_TOO_SHALLOW
        return "Breath rejected; breath too small.";
    case (DiagnosticErrorOffset + 5):       //VM_DIAG_ERROR_FINAL_RF_OUT_OF_RANGE
        return "Breath rejected; Rf out of range.";
    case (DiagnosticErrorOffset + 6):       //VM_DIAG_ERROR_FINAL_TV_OUT_OF_RANGE
        return "Breath rejected; Tv out of range.";
    case (DiagnosticErrorOffset + 7):       //VM_DIAG_ERROR_FINAL_VE_OUT_OF_RANGE
        return "Breath rejected; Ve out of range.";
    case (DiagnosticErrorOffset + 8):       //VM_DIAG_ERROR_FINAL_FEO2_OUT_OF_RANGE
        return "Breath rejected; FeO2 out of range.";
    case (DiagnosticErrorOffset + 9):       //VM_DIAG_ERROR_FINAL_VO2_OUT_OF_RANGE
        return "Breath rejected; VO2 out of range.";
    case (DiagnosticErrorOffset + 10):      //VM_DIAG_ERROR_DEVICE_INITIALIZED
        return "Device initialized.";
    case (DiagnosticErrorOffset + 11):      //VM_DIAG_ERROR_TRIED_RECORD_BEFORE_CALIB
        return "Device attempted to enter Record mode\n before completing calibration.";
    case (DiagnosticErrorOffset + 12):      //VM_DIAG_ERROR_O2_CALIB_AVG_TOO_VOLATILE
        return "Oxygen sensor calibration waveform is volatile.";
    case (DiagnosticErrorOffset + 13):      //VM_DIAG_ERROR_ADS_HIT_MAX_VALUE
        return "Oxygen sensor reading clipped at its maximum value.";
    case (DiagnosticErrorOffset + 17):       //VM_DIAG_ERROR_VALVE_REQUEST_OPEN
        return "Valve opened.";
    case (DiagnosticErrorOffset + 18):       //VM_DIAG_ERROR_VALVE_REQUEST_CLOSE
        return "Valve closed.";
    case (DiagnosticErrorOffset + 19):       //VM_DIAG_ERROR_CALIB_ADC_THEO_DIFF_MINIMUM
        return "Calib diff: minimum.";
    case (DiagnosticErrorOffset + 20):       //VM_DIAG_ERROR_CALIB_ADC_THEO_DIFF_SMALL
        return "Calib diff: small.";
    case (DiagnosticErrorOffset + 21):       //VM_DIAG_ERROR_CALIB_ADC_THEO_DIFF_MEDIUM
        return "Calib diff: medium.";
    case (DiagnosticErrorOffset + 22):       //VM_DIAG_ERROR_CALIB_ADC_THEO_DIFF_LARGE
        return "Calib diff: large.";

    // Undisclosed error codes.
    case (DiagnosticErrorOffset + 14):
    case (DiagnosticErrorOffset + 15):
    case (DiagnosticErrorOffset + 16):
    default:
        return "Error Code: " + errorCode;
    }
}
