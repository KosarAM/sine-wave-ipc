#ifndef WAVEFORMWIDGET_H
#define WAVEFORMWIDGET_H

#include <QWidget>
#include <QTimer>

#include "../generator/SharedData.h"
#include "../generator/SharedSemaphore.h"

class WaveformWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WaveformWidget(QWidget *parent = nullptr);
    ~WaveformWidget();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateWaveform();

private:
    int shmFd;
    SharedData* sharedData;

    QTimer timer;

    SharedData localData;
    
    SharedSemaphore semaphore;
};

#endif