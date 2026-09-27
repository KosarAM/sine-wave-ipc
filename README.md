# Sine Wave IPC Project

A C++/Qt application consisting of two independent processes that communicate using **Inter-Process Communication (IPC)**.

The first process continuously generates a sine wave, while the second process receives and visualizes the signal using Qt and `QPainter`. The GUI also provides controls for starting/stopping the signal generation and changing the sine wave parameters.

## Overview

The project was developed as a two-process application:

* **Signal Generator**
  A C++ process responsible for generating the sine wave and writing the generated samples to shared memory.

* **Qt Viewer**
  A Qt-based GUI process that reads the signal from shared memory and displays it using `QPainter`.

The two processes are completely independent and communicate only through IPC.

## Architecture

```text
                ┌─────────────────────────┐
                │     Qt Viewer Process   │
                │                         │
                │  - GUI                  │
                │  - QPainter             │
                │  - Waveform display     │
                │  - Start / Stop         │
                │  - Amplitude / Frequency│
                └────────────┬────────────┘
                             │
                             │ IPC
                             │
                  Shared Memory + Semaphore
                             │
                             │
                ┌────────────▼────────────┐
                │   Signal Generator      │
                │        Process          │
                │                         │
                │  - C++                  │
                │  - Sine wave generation│
                │  - Signal parameters    │
                └─────────────────────────┘
```

## IPC

The communication between the two processes is implemented using:

* **POSIX Shared Memory**
* **POSIX Semaphores**

Shared memory is used for exchanging signal data and control information between the processes.

Semaphores are used to synchronize access to the shared data and avoid simultaneous read/write operations.

This approach allows the generated samples to be exchanged without using files or requiring the two processes to be part of the same application.

## Features

### Signal Generation

The generator continuously produces a sine wave based on configurable parameters such as:

* Amplitude
* Frequency

The generated samples are periodically written to shared memory.

### Process Control

The Qt application can control the generator process.

Available controls include:

* Start signal generation
* Stop signal generation
* Change signal parameters

### Signal Visualization

The received signal is displayed using Qt's `QPainter`.

The waveform widget includes:

* Grid
* Horizontal axis
* Vertical axis
* Axis labels
* Units
* Real-time waveform display
* Automatic scaling based on the signal amplitude

The visualization is implemented as a custom Qt widget rather than using a ready-made plotting library.

## Project Structure

```text
sine_project/
│
├── CMakeLists.txt
├── README.md
│
├── generator/
│   ├── main.cpp
│   ├── SignalGenerator.cpp
│   ├── SignalGenerator.h
│   ├── SharedMemory.cpp
│   ├── SharedMemory.h
│   ├── SharedSemaphore.cpp
│   └── SharedSemaphore.h
│
└── gui/
    ├── main.cpp
    ├── MainWindow.cpp
    ├── MainWindow.h
    ├── WaveformWidget.cpp
    └── WaveformWidget.h
```

The `build/` directory is intentionally excluded from version control because it contains generated build files and executables.

## Requirements

The project requires:

* Linux
* C++ compiler with C++ support
* CMake
* Qt
* POSIX shared memory and semaphore support

The project was developed and tested in a Linux environment using WSL.

## Build

Clone the repository:

```bash
git clone <repository-url>
cd sine_project
```

Create a build directory:

```bash
mkdir build
cd build
```

Configure the project:

```bash
cmake ..
```

Build:

```bash
make
```

After a successful build, the generator and GUI executables will be available in the build directory.

## Running

The two applications are independent processes and should be started separately.

Start the signal generator:

```bash
./sine_generator
```

Then, from another terminal, start the Qt application:

```bash
./sine_gui
```



## Technologies

* **C++**
* **Qt**
* **QPainter**
* **CMake**
* **POSIX Shared Memory**
* **POSIX Semaphores**
* **Linux / WSL**

## Author

**Kosar Asadmasjedi — [GitHub](https://github.com/KosarAM)**
