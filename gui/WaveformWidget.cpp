#include "WaveformWidget.h"

#include <QPainter>
#include <QPen>
#include <QDebug>

#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <algorithm>
#include <cmath>

WaveformWidget::WaveformWidget(QWidget *parent)
    : QWidget(parent),
      shmFd(-1),
      sharedData(nullptr),
      semaphore("/sine_semaphore")
{
    setMinimumSize(800, 500);

    std::memset(
        &localData,
        0,
        sizeof(SharedData)
    );

    // =========================
    // Open shared memory
    // =========================

    shmFd = shm_open(
        "/sine_shared_memory",
        O_RDWR,
        0666
    );

    if (shmFd == -1)
    {
        qDebug()
            << "Failed to open shared memory";

        return;
    }

    sharedData =
        static_cast<SharedData*>(
            mmap(
                nullptr,
                sizeof(SharedData),
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                shmFd,
                0
            )
        );

    if (sharedData == MAP_FAILED)
    {
        qDebug()
            << "Failed to map shared memory";

        sharedData = nullptr;

        ::close(shmFd);
        shmFd = -1;

        return;
    }

    qDebug()
        << "Connected to shared memory.";

    // =========================
    // Open semaphore
    // =========================

    if (!semaphore.open())
    {
        qDebug()
            << "Failed to open semaphore.";

        return;
    }

    qDebug()
        << "Connected to semaphore.";

    // =========================
    // Timer
    // =========================

    connect(
        &timer,
        &QTimer::timeout,
        this,
        &WaveformWidget::updateWaveform
    );

    timer.start(50);
}

WaveformWidget::~WaveformWidget()
{
    if (sharedData != nullptr)
    {
        munmap(
            sharedData,
            sizeof(SharedData)
        );
    }

    if (shmFd != -1)
    {
        ::close(shmFd);
    }
}

void WaveformWidget::updateWaveform()
{
    if (sharedData == nullptr)
    {
        return;
    }

    semaphore.lock();

    std::memcpy(
        &localData,
        sharedData,
        sizeof(SharedData)
    );

    semaphore.unlock();

    update();
}

void WaveformWidget::paintEvent(
    QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.setRenderHint(
        QPainter::Antialiasing
    );

    painter.fillRect(
        rect(),
        Qt::white
    );

    // =========================
    // Plot area
    // =========================

    QRect plotArea(
        90,
        40,
        width() - 130,
        height() - 120
    );

    // =========================
    // Grid
    // =========================

    painter.setPen(
        QPen(
            QColor(220, 220, 220),
            1
        )
    );

    const int verticalDivisions = 10;
    const int horizontalDivisions = 8;

    for (int i = 0;
         i <= verticalDivisions;
         ++i)
    {
        int x =
            plotArea.left()
            +
            i * plotArea.width()
            /
            verticalDivisions;

        painter.drawLine(
            x,
            plotArea.top(),
            x,
            plotArea.bottom()
        );
    }

    for (int i = 0;
         i <= horizontalDivisions;
         ++i)
    {
        int y =
            plotArea.top()
            +
            i * plotArea.height()
            /
            horizontalDivisions;

        painter.drawLine(
            plotArea.left(),
            y,
            plotArea.right(),
            y
        );
    }

    // =========================
    // Axes
    // =========================

    painter.setPen(
        QPen(Qt::black, 2)
    );

    const int centerY =
        plotArea.center().y();

    painter.drawLine(
        plotArea.left(),
        centerY,
        plotArea.right(),
        centerY
    );

    painter.drawLine(
        plotArea.left(),
        plotArea.top(),
        plotArea.left(),
        plotArea.bottom()
    );

    // =========================
    // Y axis ticks
    // =========================

    painter.setPen(
        QPen(Qt::black, 1)
    );

    double maxAmplitude =
    std::max(
        1.0,
        std::abs(localData.amplitude)
    );

const double yMin = -maxAmplitude;
const double yMax = maxAmplitude;

    for (int i = 0;
         i <= horizontalDivisions;
         ++i)
    {
        double value =
            yMax
            -
            i * (yMax - yMin)
            /
            horizontalDivisions;

        int y =
            plotArea.top()
            +
            i * plotArea.height()
            /
            horizontalDivisions;

        painter.drawLine(
            plotArea.left() - 5,
            y,
            plotArea.left(),
            y
        );

        QString label =
            QString::number(
                value,
                'f',
                1
            );

        painter.drawText(
            35,
            y + 5,
            label
        );
    }

    // =========================
    // X axis ticks
    // =========================

    double duration = 0.0;

    if (localData.sampleRate > 0)
    {
        duration =
            static_cast<double>(
                localData.sampleCount
            )
            /
            localData.sampleRate;
    }

    for (int i = 0;
         i <= verticalDivisions;
         ++i)
    {
        double time =
            duration
            *
            static_cast<double>(i)
            /
            verticalDivisions;

        int x =
            plotArea.left()
            +
            i * plotArea.width()
            /
            verticalDivisions;

        painter.drawLine(
            x,
            plotArea.bottom(),
            x,
            plotArea.bottom() + 5
        );

        QString label =
            QString::number(
                time,
                'f',
                3
            );

        painter.drawText(
            x - 20,
            plotArea.bottom() + 22,
            label
        );
    }

    // =========================
    // Axis titles
    // =========================

    painter.setPen(
        QPen(Qt::black, 1)
    );

    painter.drawText(
        plotArea.right() - 50,
        plotArea.bottom() + 45,
        "Time (s)"
    );

    painter.drawText(
        10,
        plotArea.top() - 15,
        "Amplitude"
    );

    // =========================
    // Waveform
    // =========================

    if (localData.sampleCount > 1)
    {
        painter.setPen(
            QPen(Qt::blue, 2)
        );

        QPolygonF waveform;

        for (int i = 0;
             i < localData.sampleCount;
             ++i)
        {
            double x =
                plotArea.left()
                +
                static_cast<double>(i)
                /
                (localData.sampleCount - 1)
                *
                plotArea.width();

            double value =
                localData.samples[i];

            // Limit value to plot range
            if (value > yMax)
                value = yMax;

            if (value < yMin)
                value = yMin;

            double normalized =
                (value - yMin)
                /
                (yMax - yMin);

            double y =
                plotArea.bottom()
                -
                normalized
                *
                plotArea.height();

            waveform.append(
                QPointF(x, y)
            );
        }

        painter.drawPolyline(
            waveform
        );
    }

    // =========================
    // Information
    // =========================

    painter.setPen(Qt::black);

    QString info =
        QString(
            "Frequency: %1 Hz    "
            "Amplitude: %2    "
            "Sample Rate: %3 Hz"
        )
        .arg(
            localData.frequency,
            0,
            'f',
            2
        )
        .arg(
            localData.amplitude,
            0,
            'f',
            2
        )
        .arg(
            localData.sampleRate,
            0,
            'f',
            0
        );

    painter.drawText(
        90,
        height() - 25,
        info
    );
}