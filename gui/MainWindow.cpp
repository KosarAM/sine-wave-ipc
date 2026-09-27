#include "MainWindow.h"
#include "WaveformWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QDoubleValidator>
#include <QDebug>

#include <cstring>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      waveformWidget(nullptr),
      frequencyEdit(nullptr),
      amplitudeEdit(nullptr),
      startButton(nullptr),
      stopButton(nullptr),
      shmFd(-1),
      sharedData(nullptr),
      semaphore("/sine_semaphore")
{
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
        qDebug() << "Failed to open shared memory";
        return;
    }

    sharedData = static_cast<SharedData*>(
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
        qDebug() << "Failed to map shared memory";

        sharedData = nullptr;

        ::close(shmFd);
        shmFd = -1;

        return;
    }

    qDebug() << "MainWindow connected to shared memory.";

    if (!semaphore.open())
{
    qDebug()
        << "Failed to open semaphore.";

    return;
}

    // =========================
    // Main widget
    // =========================

    QWidget *centralWidget = new QWidget(this);

    QVBoxLayout *mainLayout =
        new QVBoxLayout(centralWidget);

    // =========================
    // Waveform
    // =========================

    waveformWidget =
        new WaveformWidget(centralWidget);

    mainLayout->addWidget(
        waveformWidget,
        1
    );

    // =========================
    // Controls
    // =========================

    QHBoxLayout *controlLayout =
        new QHBoxLayout();

    QLabel *frequencyLabel =
        new QLabel("Frequency (Hz):");

    frequencyEdit =
    new QLineEdit();

    frequencyEdit->setReadOnly(false);
    frequencyEdit->setEnabled(true);
    frequencyEdit->setText(
    QString::number(sharedData->frequency)
);



    QLabel *amplitudeLabel =
        new QLabel("Amplitude:");

    amplitudeEdit =
    new QLineEdit();

    amplitudeEdit->setReadOnly(false);
    amplitudeEdit->setEnabled(true);
    amplitudeEdit->setText(
    QString::number(sharedData->amplitude)
);

    startButton =
        new QPushButton("Start");

    stopButton =
        new QPushButton("Stop");

    controlLayout->addWidget(
        frequencyLabel
    );

    controlLayout->addWidget(
        frequencyEdit
    );

    controlLayout->addWidget(
        amplitudeLabel
    );

    controlLayout->addWidget(
        amplitudeEdit
    );

    controlLayout->addWidget(
        startButton
    );

    controlLayout->addWidget(
        stopButton
    );

    mainLayout->addLayout(
        controlLayout
    );

    setCentralWidget(
        centralWidget
    );

    // =========================
    // Connections
    // =========================

    connect(
        startButton,
        &QPushButton::clicked,
        this,
        &MainWindow::startGenerator
    );

    connect(
        stopButton,
        &QPushButton::clicked,
        this,
        &MainWindow::stopGenerator
    );

    connect(
        frequencyEdit,
        &QLineEdit::editingFinished,
        this,
        &MainWindow::updateFrequency
    );

    connect(
        amplitudeEdit,
        &QLineEdit::editingFinished,
        this,
        &MainWindow::updateAmplitude
    );
}

MainWindow::~MainWindow()
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

void MainWindow::startGenerator()
{
    if (sharedData == nullptr)
        return;

    semaphore.lock();

    sharedData->running = true;

    semaphore.unlock();

    qDebug()
        << "Generator started.";
}

void MainWindow::stopGenerator()
{
    if (sharedData == nullptr)
        return;

    semaphore.lock();

    sharedData->running = false;

    semaphore.unlock();

    qDebug()
        << "Generator stopped.";
}

void MainWindow::updateFrequency()
{
    if (sharedData == nullptr)
        return;

    double frequency =
        frequencyEdit->text().toDouble();

    semaphore.lock();

    sharedData->frequency = frequency;

    semaphore.unlock();

    qDebug()
        << "Frequency changed to:"
        << frequency;
}

void MainWindow::updateAmplitude()
{
    if (sharedData == nullptr)
        return;

    double amplitude =
        amplitudeEdit->text().toDouble();

    semaphore.lock();

    sharedData->amplitude = amplitude;

    semaphore.unlock();

    qDebug()
        << "Amplitude changed to:"
        << amplitude;
}