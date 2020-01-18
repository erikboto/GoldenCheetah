#include "VMProWidget.h"

#include <QThread>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QScrollBar>

VMProWidget::VMProWidget(QLowEnergyService * service, QObject * parent)
{
    m_vmProConfigurator = new VMProConfigurator(service, this);

    connect(m_vmProConfigurator, &VMProConfigurator::logMessage, this, &VMProWidget::addStatusMessage);

    QWidget * settingsWidget = new QWidget();

    // User Piece Setting
    QLabel * userPieceLabel = new QLabel("The size engraved on the user pieces. This is the skrew-in part that attaches to the mask.");
    userPieceLabel->setWordWrap(true);
    m_userPiecePicker = new QComboBox();
    m_userPiecePicker->addItem("Resting", VMProVenturiSize::VM_VENTURI_RESTING);
    m_userPiecePicker->addItem("Small", VMProVenturiSize::VM_VENTURI_SMALL);
    m_userPiecePicker->addItem("Medium", VMProVenturiSize::VM_VENTURI_MEDIUM);
    m_userPiecePicker->addItem("Large", VMProVenturiSize::VM_VENTURI_LARGE);
    m_userPiecePicker->addItem("Extra Large", VMProVenturiSize::VM_VENTURI_XLARGE);

    QVBoxLayout *userPieceLayout = new QVBoxLayout();
    userPieceLayout->addWidget(userPieceLabel);
    userPieceLayout->addWidget(m_userPiecePicker);

    QGroupBox *userPieceBox = new QGroupBox("User Piece Size");
    userPieceBox->setLayout(userPieceLayout);


    // Volume Correction Mode
    QLabel * volumeLabel = new QLabel("This is an advanced setting. If you don't know what it is, it's recommended that you leave it on STPD.");
    volumeLabel->setWordWrap(true);
    m_volumePicker = new QComboBox();
    m_volumePicker->addItem("STPD", VMProVolumeCorrectionMode::VM_STPD);
    m_volumePicker->addItem("BTPS", VMProVolumeCorrectionMode::VM_BTPS);

    QVBoxLayout *volumeLayout = new QVBoxLayout();
    volumeLayout->addWidget(volumeLabel);
    volumeLayout->addWidget(m_volumePicker);

    QGroupBox *volumeBox = new QGroupBox("Volume Correction Mode");
    volumeBox->setLayout(volumeLayout);

    // Auto Re-Calibration
    QLabel * autocalibLabel = new QLabel("If enabled, the device will automatically start a recalibration 5 minutes into the session. If disabled you can still trigger a manual calibration.");
    autocalibLabel->setWordWrap(true);
    m_autocalibPicker = new QComboBox();

    QVBoxLayout *autocalibLayout = new QVBoxLayout();
    autocalibLayout->addWidget(autocalibLabel);
    autocalibLayout->addWidget(m_autocalibPicker);

    m_autocalibPicker->addItem("Enabled", VMProIdleTimeout::VM_ON);
    m_autocalibPicker->addItem("Disabled", VMProIdleTimeout::VM_OFF);
    QGroupBox *autocalibBox = new QGroupBox("Auto Recalibration");
    autocalibBox->setLayout(autocalibLayout);

    // Idle Timeout
    QLabel * idleTimeoutLabel = new QLabel("If enabled, the device will shut off after 15 minutes of no breathing.");
    idleTimeoutLabel->setWordWrap(true);
    m_idleTimeoutPicker = new QComboBox();

    QVBoxLayout *idleTimeoutLayout = new QVBoxLayout();
    idleTimeoutLayout->addWidget(idleTimeoutLabel);
    idleTimeoutLayout->addWidget(m_idleTimeoutPicker);

    m_idleTimeoutPicker->addItem("Enabled", VMProIdleTimeout::VM_ON);
    m_idleTimeoutPicker->addItem("Disabled", VMProIdleTimeout::VM_OFF);
    QGroupBox *idleTimeoutBox = new QGroupBox("Idle Timeout");
    idleTimeoutBox->setLayout(idleTimeoutLayout);

    // Calibration Progress
    QLabel * calibrationProgressInfoLabel = new QLabel("Calibration Status:");
    m_calibrationProgressLabel = new QLabel("Unknown");

    QVBoxLayout *calibrationProgressLayout = new QVBoxLayout();
    calibrationProgressLayout->addWidget(calibrationProgressInfoLabel);
    calibrationProgressLayout->addWidget(m_calibrationProgressLabel);

    QGroupBox *calibrationProgressBox = new QGroupBox("Calibration Status");
    calibrationProgressBox->setLayout(calibrationProgressLayout);

    // Settings Widget
    QVBoxLayout *settingsLayout = new QVBoxLayout();
    settingsLayout->addWidget(userPieceBox);
    settingsLayout->addWidget(volumeBox);
    settingsLayout->addWidget(autocalibBox);
    settingsLayout->addWidget(idleTimeoutBox);
    settingsLayout->addWidget(calibrationProgressBox);
    settingsWidget->setLayout(settingsLayout);

    QWidget * dbgWidget = new QWidget();
    QGroupBox *dbgBox = new QGroupBox("Status Information");
    QVBoxLayout *dbgLayout = new QVBoxLayout(dbgWidget);
    m_deviceLog = new QTextEdit("...", dbgWidget);
    QPushButton * saveButton = new QPushButton("Save log to file");
    saveButton->setMaximumHeight(25);
    dbgLayout->addWidget(m_deviceLog);
    dbgLayout->addWidget(saveButton);
    dbgBox->setLayout(dbgLayout);

    // Main layout
    QWidget * w = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(w);
    mainLayout->addWidget(settingsWidget);
    mainLayout->addWidget(dbgBox);

    w->setLayout(mainLayout);

    w->show();

    // Basic init of settings
    m_vmProConfigurator->setIdleTimeout(VMProIdleTimeout::VM_ON);
    QThread::msleep(50);
    m_vmProConfigurator->setUserPieceSize(VMProVenturiSize::VM_VENTURI_MEDIUM);
    QThread::msleep(50);
    m_vmProConfigurator->setVolumeCorrectionMode(VMProVolumeCorrectionMode::VM_STPD);


    // set up connections
    connect(m_vmProConfigurator, &VMProConfigurator::idleTimeoutStateChanged, this, &VMProWidget::onIdleTimeoutChanged);
    connect(m_vmProConfigurator, &VMProConfigurator::userPieceSizeChanged, this, &VMProWidget::onUserPieceSizeChanged);
    connect(m_vmProConfigurator, &VMProConfigurator::calibrationProgressChanged, this, &VMProWidget::onCalibrationProgressChanged);
    connect(m_vmProConfigurator, &VMProConfigurator::volumeCorrectionModeChanged, this, &VMProWidget::onVolumeCorrectionModeChanged);
    connect(m_vmProConfigurator, &VMProConfigurator::errorCodeReceived, this, &VMProWidget::onErrorCodeReceived);
    connect(saveButton, &QPushButton::clicked, this, &VMProWidget::onSaveClicked);

    connect(m_userPiecePicker, static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged), this, &VMProWidget::onUserPieceSizePickerChanged);
    connect(m_volumePicker, static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged), this, &VMProWidget::onVolumeCorrectionModePickerChanged);
    connect(m_idleTimeoutPicker, static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged), this, &VMProWidget::onIdleTimeoutPickerChanged);
}

void VMProWidget::addStatusMessage(const QString & msg)
{
    m_deviceLog->append(msg);
    QScrollBar *sb = m_deviceLog->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void VMProWidget::onVolumeCorrectionModeChanged(VMProVolumeCorrectionMode mode)
{
    m_currVolumeCorrectionMode = mode;

    int index = m_volumePicker->findData(mode);
    if (index != -1) {
        m_volumePicker->setCurrentIndex(index);
    }
}

void VMProWidget::onUserPieceSizeChanged(VMProVenturiSize size)
{
    m_currVenturiSize = size;

    int index = m_userPiecePicker->findData(size);
    if (index != -1) {
        m_userPiecePicker->setCurrentIndex(index);
    }
}

void VMProWidget::onIdleTimeoutChanged(VMProIdleTimeout state)
{
    m_currIdleTimeoutState = state;

    int index = m_idleTimeoutPicker->findData(state);
    if (index != -1) {
        m_idleTimeoutPicker->setCurrentIndex(index);
    }
}

void VMProWidget::onErrorCodeReceived(quint8 code)
{
}

void VMProWidget::onCalibrationProgressChanged(quint8 percentCompleted)
{
    m_calibrationProgressLabel->setText(QString::number(percentCompleted));
}

void VMProWidget::onIdleTimeoutPickerChanged(int /*index*/)
{
    VMProIdleTimeout newState = m_idleTimeoutPicker->currentData().value<VMProIdleTimeout>();

    if (newState != m_currIdleTimeoutState)
    {
        m_vmProConfigurator->setIdleTimeout(newState);
    }
}

void VMProWidget::onUserPieceSizePickerChanged(int /*index*/)
{
    VMProVenturiSize newState = m_userPiecePicker->currentData().value<VMProVenturiSize>();

    if (newState != m_currVenturiSize)
    {
        m_vmProConfigurator->setUserPieceSize(newState);
    }
}

void VMProWidget::onVolumeCorrectionModePickerChanged(int /*index*/)
{
    VMProVolumeCorrectionMode newState = m_volumePicker->currentData().value<VMProVolumeCorrectionMode>();
    if (newState != m_currVolumeCorrectionMode)
    {
        m_vmProConfigurator->setVolumeCorrectionMode(newState);
    }
}

void VMProWidget::onSaveClicked()
{
    QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save File"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (!fileName.isEmpty())
    {
        QFile outfile(fileName);
        if (outfile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&outfile);
            stream << m_deviceLog->toPlainText();
            outfile.flush();
            outfile.close();
        }
    }
}
