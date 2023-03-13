#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "device.h"
#include "QTimer"
#include "QElapsedTimer"
#include "QLabel"
#include "device.h"
#include <QFile>
#include <QTextEdit>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDebug>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Device* device;
    bool connected = true;
    QTimer *timer;

    void updateSessionTextDisplay();
    int flashes = 7;

private slots:
    //Slot for the progress bar aka the battery level
    void updateBattery();

    //Slots triggered by the pushbuttons
    void power();
    void decreaseIntensity();
    void increaseIntensity();
    void selectSession();
    void recordSession();
    void testConnection();
    void disconnect();
    void reconnect();
    void previousSession();

    //Slots for comboboxes
    void sessionGroup();
    void sessionType();

    //Slots for labels
    void updateDisplay(int t);
    void disableIcons();
    void enableIcons();

    //Slots for timer
    void timeout();

};
#endif // MAINWINDOW_H
