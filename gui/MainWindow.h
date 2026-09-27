#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "../generator/SharedData.h"
#include "../generator/SharedSemaphore.h"

class WaveformWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startGenerator();
    void stopGenerator();
    void updateFrequency();
    void updateAmplitude();

private:
    void writeSharedData();

    WaveformWidget *waveformWidget;

    QLineEdit *frequencyEdit;
    QLineEdit *amplitudeEdit;

    QPushButton *startButton;
    QPushButton *stopButton;

    int shmFd;
    SharedData *sharedData;

    SharedSemaphore semaphore;
};

#endif