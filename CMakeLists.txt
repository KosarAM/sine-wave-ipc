cmake_minimum_required(VERSION 3.16)

project(SineWaveProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
)

# =========================
# Generator Process
# =========================

add_executable(
    sine_generator

    generator/main.cpp
    generator/SignalGenerator.cpp
    generator/SharedMemory.cpp
    generator/SharedSemaphore.cpp

)

# =========================
# Qt GUI Process
# =========================

qt_standard_project_setup()

add_executable(
    sine_gui

    gui/main.cpp
    gui/MainWindow.cpp
    gui/MainWindow.h
    gui/WaveformWidget.cpp
    gui/WaveformWidget.h

    generator/SharedData.h
    generator/SharedSemaphore.cpp
    generator/SharedSemaphore.h

)

target_link_libraries(
    sine_gui
    PRIVATE
    Qt6::Widgets
)