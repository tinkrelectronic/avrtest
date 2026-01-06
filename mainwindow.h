#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QTextEdit>
#include <QLabel>
#include "avrdude_conf_parser.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void browseFlash();
    void browseEeprom();

private:
    void setupUi();
    void loadAvrdudeConfig();

    // UI elements
    QComboBox *programmerCombo;
    QComboBox *portCombo;
    QComboBox *mcuCombo;
    QPushButton *detectButton;

    QLineEdit *flashFileEdit;
    QPushButton *flashBrowseBtn;
    QCheckBox *flashReadChk;
    QCheckBox *flashWriteChk;
    QCheckBox *flashVerifyChk;

    QLineEdit *eepromFileEdit;
    QPushButton *eepromBrowseBtn;
    QCheckBox *eepromReadChk;
    QCheckBox *eepromWriteChk;
    QCheckBox *eepromVerifyChk;

    QLineEdit *fuseLowEdit;
    QLineEdit *fuseHighEdit;
    QLineEdit *fuseExtEdit;
    QLineEdit *lockEdit;
    QPushButton *fuseReadBtn;
    QPushButton *fuseWriteBtn;

    QTextEdit *commandPreview;
    QTextEdit *logOutput;

    QPushButton *goButton;

    AvrdudeConfParser parser;
};

#endif // MAINWINDOW_H