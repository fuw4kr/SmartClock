cmake_minimum_required(VERSION 3.16)
project(SmartClock VERSION 0.1 LANGUAGES CXX)

# ======================================================
# === BASIC SETTINGS
# ======================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia Test)
qt_standard_project_setup()

if (MSVC)
    add_compile_options(/std:c++17 /Zc:__cplusplus)
endif()

# ======================================================
# === SOURCE FILES
# ======================================================
set(SMARTCLOCK_SOURCES
        mainwindow.cpp mainwindow.h mainwindow.ui

        timer/timermanager.cpp timer/timermanager.h
        timer/timerwindow.cpp timer/timerwindow.h timer/timerwindow.ui
        timer/timereditdialog.cpp timer/timereditdialog.h timer/timereditdialog.ui
        timer/settingstimerdialog.cpp timer/settingstimerdialog.h timer/settingstimerdialog.ui
        timer/historytimerwindow.cpp timer/historytimerwindow.h timer/historytimerwindow.ui

        clock/clockwindow.cpp clock/clockwindow.h clock/clockwindow.ui
        clock/clocksettingsdialog.cpp clock/clocksettingsdialog.h clock/clocksettingsdialog.ui

        alarm/alarmwindow.cpp alarm/alarmwindow.h alarm/alarmwindow.ui
        alarm/alarmmanager.cpp alarm/alarmmanager.h
        alarm/alarmsettingsdialog.cpp alarm/alarmsettingsdialog.h alarm/alarmsettingsdialog.ui
        alarm/alarmitemwidget.cpp alarm/alarmitemwidget.h alarm/alarmitemwidget.ui

        stopwatch/stopwatchwindow.cpp stopwatch/stopwatchwindow.h stopwatch/stopwatchwindow.ui
        stopwatch/analogstopwatchdial.cpp stopwatch/analogstopwatchdial.h

        windowEdit/framelessWindow.cpp windowEdit/framelessWindow.h
        windowEdit/snapPreviewWindow.cpp windowEdit/snapPreviewWindow.h

        thememanager.cpp thememanager.h
)

# ======================================================
# === RESOURCES
# ======================================================
qt_add_resources(SMARTCLOCK_RESOURCES resources.qrc)

# ======================================================
# === MAIN LIBRARY (core logic shared between app & tests)
# ======================================================
add_library(SmartClockLib STATIC
        ${SMARTCLOCK_SOURCES}
        ${SMARTCLOCK_RESOURCES}
)

target_link_libraries(SmartClockLib
        PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Multimedia
)
target_include_directories(SmartClockLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# ======================================================
# === MAIN EXECUTABLE
# ======================================================
qt_add_executable(SmartClock
        MANUAL_FINALIZATION
        main.cpp
        ${SMARTCLOCK_RESOURCES}  # ресурси також для exe
)

target_link_libraries(SmartClock
        PRIVATE
        SmartClockLib
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Multimedia
)

# --- Kill previous process before link (Windows only) ---
if (WIN32)
    add_custom_command(TARGET SmartClock
            PRE_LINK
            COMMAND taskkill /F /IM SmartClock.exe 2> NUL || exit 0
            COMMENT "🧹 Killing previous SmartClock.exe if running..."
    )
endif()

set_target_properties(SmartClock PROPERTIES WIN32_EXECUTABLE TRUE)
qt_finalize_executable(SmartClock)

# ======================================================
# === CONNECT TESTS
# ======================================================
enable_testing()
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt")
        message(STATUS "Including GoogleTests...")
        add_subdirectory(tests)
    else()
        message(WARNING "Tests directory not found, skipping tests.")
    endif()
endif()
