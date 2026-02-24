# SmartClock

**SmartClock** is a modern, multifunctional desktop application developed in **C++ / Qt6**, combining a **Smart Timer**, **Alarm**, **Clock**, and **Stopwatch** in one elegant and user-friendly interface.  

---

## Overview

SmartClock supports:
- Multiple simultaneous timers and alarms.
- Configurable end actions (sounds, file or app launching).
- Persistent data across sessions.
- Dynamic theming and user interface customization.
- Visual and sound feedback through notifications and taskbar progress indicators.
  
---

##  Core Features

##  Smart Timer System

The **Smart Timer** is the core module of SmartClock.  
It unifies **countdown timers** (running for a set duration) into a single, intuitive interface for scheduling, filtering, and managing multiple events simultaneously.

###  Main Capabilities
- **Multiple concurrent timers:** run any number of timers simultaneously without limitation.  
- **Persistent timers:** all running and completed timers are saved and automatically restored after restarting the app or OS.  
- **Visual and sound notifications:**  
  - On-screen highlights and messages when time expires.  
  - Audio alerts through `QSoundEffect`.  
- **Custom actions upon completion:**  
  - Launch an external program.  
  - Open files or documents.  
  - Show a system notification or popup.

---

###  User Interface
- **Unified list view:** all timers and alarms appear together in one table.  
- **Filters:** display only active, expired, or future timers.  
- **Sorting options:** organize by name, duration, remaining time, or creation order.  
- **Inline editing:** modify timer names, durations, or sounds directly from the table.  
- **“Next Up” indicator:** shows which timer is expected to trigger next.  
- **Windows Taskbar integration:** progress bar displays remaining time for the next timer.

---

###  Advanced Functionalities
- **Timer groups:** predefined sets of timers that can be launched together (e.g., “Study”, “Work”).  
- **Timer dependencies:** define relations — e.g., after a “Work” timer finishes, suggest starting a “Break” timer.  
- **Do Not Disturb mode:** mute all notifications.  
- **Configurable timer types:** customize duration, sound, and end actions per timer.  
- **Smart hints and tooltips:** provide guidance during timer setup.  
- **Different input formats:** set time either as a duration (e.g., “00:25:00”) or target time (e.g., “12:30”).  
- **Comprehensive sorting & filtering system:** by type, time remaining, or state (running, paused, finished).  

---

##  Alarm System

The **Alarm** module extends timer functionality with fixed-time scheduling and daily recurrence options.

###  Features
- Create alarms for any time of day.  
- Repetition options: **Once**, **Daily**, or **Selected weekdays**.  
- Enable/disable alarms via visual toggle buttons.  
- Clear distinction between active and inactive alarms.  
- Visual and sound alerts when triggered.  
- All alarms are automatically saved to `alarms.json` and reloaded on startup.  
- Stored in the user’s system AppData directory (`QStandardPaths::AppDataLocation`).  

---

##  Clock Module

- Displays the **current system time** with smooth real-time updates.  
- Supports **multiple time zones**.  
- Switch between **12-hour** and **24-hour** formats.  
- Styled dynamically to match the selected application theme.  
- Serves as a background and reference for timers and alarms.

---

##  Stopwatch Module

The **Stopwatch** provides a simple yet powerful time-tracking interface.

### Features
- Start, pause, and reset controls.  
- Record laps (lap times) with automatic highlighting of best and worst laps.  
- Displays elapsed time in `HH:MM:SS` format or as a graphical analog dial.  
- Responsive interface with smooth animations and transitions.  
- Fully adaptive to theme and color scheme.  

---

##  Theming and Design

- **Two complete visual themes:** Light and Dark.  
- Switch themes instantly without restarting.  
- **Consistent design:** all modules (Clock, Timer, Alarm, Stopwatch) share the same visual style.  
- **Rounded corners and soft shadows** for a modern look.  
- **Hover and focus effects** for all interactive UI elements.  
- **Table design:** alternating row colors, header styling, and clear selection highlighting.  
- **Hotkeys:** perform key actions (Start/Pause, OK/Cancel, Delete) directly from the keyboard for accessibility.  

---

##  Data Persistence

SmartClock saves all user data automatically to ensure no progress is lost.

When the application restarts, it automatically restores:
- All active and completed timers.
- All alarm settings.
- The active theme (light/dark).

---

##  Notifications & System Integration

- **Sound signals:** each timer/alarm can use a unique sound.
- **Visual popups:** highlight or flash the timer row upon completion.
- **Taskbar progress indicator (Windows):**
  - Displays remaining time progress for the nearest timer.
  - Resets automatically when the timer finishes or is canceled.
- **System tray icon:**
  - Allows minimizing the app to tray.
  - Provides a quick restore and quit menu.
  - Shows tooltip with current active timers.

---

## Project Structure
```
SmartClock/

├── CMakeLists.txt
├── main.cpp
├── mainwindow.ui / .h / .cpp
├── resources.qrc
├── thememanager.h / . cpp
│
├── alarm/
│ ├── alarmwindow.ui / .h / .cpp
│ ├── alarmmanager.h / .cpp
│ ├── alarmsettingsdialog.ui / .h / .cpp
│ ├── alarmitemwidget.ui / .h / .cpp
│
├── timer/
│ ├── timermanager.h / .cpp
│ ├── timerwindow.ui / .h / .cpp
│ ├── timereditdialog.ui / .h / .cpp
│ ├── settingstimerdialog.ui / .h / .cpp
│ ├── historytimerwindow.ui / .h / .cpp
│
├── clock/
│ ├── clockwindow.ui / .h / .cpp
│ ├── clocksettingsdialog.ui / .h / .cpp
│
├── stopwatch/
│ ├── stopwatchwindow.ui / .h / .cpp
│ ├── analogstopwatchdial.h / .cpp
│
├── windowEdit/
│ ├── framelessWindow.h / .cpp
│ ├── snapPreviewWindow.h / .cpp
│
├── style/
│ ├── darkTheme.qss
│ └── lightTheme.qss
│
├── resources/
│ ├── icons/ 
│ └── sounds/ 
│
└── tests/
├── CMakeLists.txt
├── test_main.cpp
├── test_timer.cpp
├── test_alarm.cpp
├── test_clock.cpp
└── test_stopwatch.cpp
```
---

##  Unit Testing

SmartClock includes a comprehensive **unit testing system** built with **Google Test (GTest)**.  
The tests are designed to ensure that every part of the application — especially core logic — behaves reliably under different conditions.

All tests are automatically built and run through **CMake** and **GitHub Actions CI**, ensuring continuous verification on every commit and pull request.  
This integration provides automatic feedback, confirming that the application remains stable across platforms and compilers.

---

## Running Tests

The project includes an integrated **Google Test** framework.  
To build the tests, make sure the `BUILD_TESTS` option is enabled in your `CMakeLists.txt` file:

```cmake
option(BUILD_TESTS "Build unit tests" ON)
```
After compilation, you can run all tests using CTest:
```
cd build/tests
ctest --output-on-failure
```
Or run the test executable directly: 
```
./SmartClockTests
```

---

###  Overview

Unit tests verify:
- correctness of logic in non-UI classes,
- file persistence and data loading,
- proper initialization of Qt widgets and signals,
- interaction between managers and their corresponding windows.

All test files are located in the **`tests/`** directory and compiled into a separate executable named **`SmartClockTests`**.

---

## Tech Stack

| Component | Technology |
|------------|-------------|
| Language | C++17 |
| Framework | Qt6 (Core, Widgets, Multimedia, Test) |
| Build System | CMake |
| Testing | Google Test |
| IDE Support | Qt Creator, CLion |
