cmake_minimum_required(VERSION 3.5)

project(voice_assistant VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Multimedia Network Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Multimedia Network Widgets)

set(PROJECT_SOURCES
    sliderbutton.h sliderbutton.cpp
    audiorecorder.h audiorecorder.cpp
    mainwindow.h mainwindow.cpp
    main.cpp
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(voice_assistant
        MANUAL_FINALIZATION
        src.qrc
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(voice_assistant PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Widgets
)

set_target_properties(voice_assistant PROPERTIES
    WIN32_EXECUTABLE TRUE
)

install(TARGETS voice_assistant
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(voice_assistant)
endif()

