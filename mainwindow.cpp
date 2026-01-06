#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QStandardPaths>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Qt AVRDUDESS - AVRDUDE GUI for Linux");
    resize(900, 700);
    setupUi();
    loadAvrdudeConfig();
}

void MainWindow::setupUi()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // Configuration group
    QGroupBox *configGroup = new QGroupBox("Configuration");
    QGridLayout *configLayout = new QGridLayout(configGroup);

    configLayout->addWidget(new QLabel("Programmer:"), 0, 0);
    programmerCombo = new QComboBox();
    configLayout->addWidget(programmerCombo, 0, 1);

    configLayout->addWidget(new QLabel("Port:"), 0, 2);
    portCombo = new QComboBox();
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        portCombo->addItem(info.portName() + " - " + info.description(), info.portName());
    }
    configLayout->addWidget(portCombo, 0, 3);

    detectButton = new QPushButton("Detect MCU");
    configLayout->addWidget(detectButton, 0, 4);

    configLayout->addWidget(new QLabel("MCU:"), 1, 0);
    mcuCombo = new QComboBox();
    configLayout->addWidget(mcuCombo, 1, 1, 1, 4);

    mainLayout->addWidget(configGroup);

    // Flash group
    QGroupBox *flashGroup = new QGroupBox("Flash");
    QHBoxLayout *flashLayout = new QHBoxLayout(flashGroup);
    flashFileEdit = new QLineEdit();
    flashBrowseBtn = new QPushButton("Browse...");
    flashReadChk = new QCheckBox("Read");
    flashWriteChk = new QCheckBox("Write");
    flashVerifyChk = new QCheckBox("Verify");

    flashLayout->addWidget(flashFileEdit);
    flashLayout->addWidget(flashBrowseBtn);
    flashLayout->addWidget(flashReadChk);
    flashLayout->addWidget(flashWriteChk);
    flashLayout->addWidget(flashVerifyChk);

    mainLayout->addWidget(flashGroup);

    // EEPROM group
    QGroupBox *eepromGroup = new QGroupBox("EEPROM");
    QHBoxLayout *eepromLayout = new QHBoxLayout(eepromGroup);
    eepromFileEdit = new QLineEdit();
    eepromBrowseBtn = new QPushButton("Browse...");
    eepromReadChk = new QCheckBox("Read");
    eepromWriteChk = new QCheckBox("Write");
    eepromVerifyChk = new QCheckBox("Verify");

    eepromLayout->addWidget(eepromFileEdit);
    eepromLayout->addWidget(eepromBrowseBtn);
    eepromLayout->addWidget(eepromReadChk);
    eepromLayout->addWidget(eepromWriteChk);
    eepromLayout->addWidget(eepromVerifyChk);

    mainLayout->addWidget(eepromGroup);

    // Fuses group
    QGroupBox *fuseGroup = new QGroupBox("Fuses & Locks");
    QGridLayout *fuseLayout = new QGridLayout(fuseGroup);

    fuseLayout->addWidget(new QLabel("Low:"), 0, 0);
    fuseLowEdit = new QLineEdit("0xFF");
    fuseLayout->addWidget(fuseLowEdit, 0, 1);
    fuseLayout->addWidget(new QLabel("High:"), 0, 2);
    fuseHighEdit = new QLineEdit("0xFF");
    fuseLayout->addWidget(fuseHighEdit, 0, 3);

    fuseLayout->addWidget(new QLabel("Extended:"), 1, 0);
    fuseExtEdit = new QLineEdit("0xFF");
    fuseLayout->addWidget(fuseExtEdit, 1, 1);
    fuseLayout->addWidget(new QLabel("Lock:"), 1, 2);
    lockEdit = new QLineEdit("0xFF");
    fuseLayout->addWidget(lockEdit, 1, 3);

    fuseReadBtn = new QPushButton("Read Fuses");
    fuseWriteBtn = new QPushButton("Write Fuses");
    fuseLayout->addWidget(fuseReadBtn, 2, 0);
    fuseLayout->addWidget(fuseWriteBtn, 2, 1);

    mainLayout->addWidget(fuseGroup);

    // Command preview
    mainLayout->addWidget(new QLabel("Command line:"));
    commandPreview = new QTextEdit();
    commandPreview->setReadOnly(true);
    commandPreview->setMaximumHeight(80);
    mainLayout->addWidget(commandPreview);

    // Log output
    mainLayout->addWidget(new QLabel("Output:"));
    logOutput = new QTextEdit();
    logOutput->setReadOnly(true);
    mainLayout->addWidget(logOutput);

    // Go button
    goButton = new QPushButton("PROGRAM!");
    goButton->setStyleSheet("QPushButton { font-size: 18pt; font-weight: bold; padding: 15px; }");
    mainLayout->addWidget(goButton);

    // Connections
    connect(flashBrowseBtn, &QPushButton::clicked, this, &MainWindow::browseFlash);
    connect(eepromBrowseBtn, &QPushButton::clicked, this, &MainWindow::browseEeprom);
}

void MainWindow::browseFlash()
{
    QString file = QFileDialog::getOpenFileName(this, "Select Flash File", "", "Intel Hex/Binary (*.hex *.bin)");
    if (!file.isEmpty()) flashFileEdit->setText(file);
}

void MainWindow::browseEeprom()
{
    QString file = QFileDialog::getOpenFileName(this, "Select EEPROM File", "", "Intel Hex/Binary (*.hex *.bin *.eep)");
    if (!file.isEmpty()) eepromFileEdit->setText(file);
}

void MainWindow::loadAvrdudeConfig()
{
    QStringList possiblePaths = {
        "/etc/avrdude.conf",
        "/usr/share/avrdude/avrdude.conf",
        "/usr/local/etc/avrdude.conf",
        QDir(QStandardPaths::findExecutable("avrdude")).absoluteFilePath("../etc/avrdude.conf")
    };

    QString confPath;
    for (const QString& path : possiblePaths) {
        if (QFile::exists(path)) {
            confPath = path;
            break;
        }
    }

    if (confPath.isEmpty() || !parser.parse(confPath)) {
        QMessageBox::warning(this, "Error", "Could not load avrdude.conf\nSome features will be limited.");
        return;
    }

    // Populate programmers
    for (const Programmer& prog : parser.programmers()) {
        QString text = prog.desc.isEmpty() ? prog.id : prog.desc + " (" + prog.id + ")";
        programmerCombo->addItem(text, prog.id);
    }

    // Populate MCUs
    for (const MCU& mcu : parser.mcus()) {
        QString text = mcu.desc.isEmpty() ? mcu.id : mcu.desc + " (" + mcu.id + ")";
        mcuCombo->addItem(text, mcu.id);
    }
}