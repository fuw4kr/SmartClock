# ======================================================
# === GOOGLE TEST SETUP
# ======================================================
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# ======================================================
# === TEST EXECUTABLE
# ======================================================
add_executable(SmartClockTests
        test_main.cpp
        test_logic_timer.cpp
        test_logic_alarm.cpp
        test_logic_clock.cpp
        test_logic_stopwatch.cpp
        test_theme.cpp
)

target_compile_definitions(SmartClockTests PRIVATE UNIT_TESTING)

target_link_libraries(SmartClockTests
        PRIVATE
        SmartClockLib
        GTest::gtest_main
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Test
)

if (WIN32)
    add_custom_command(TARGET SmartClockTests
            PRE_LINK
            COMMAND taskkill /F /IM SmartClockTests.exe 2> NUL || exit 0
            COMMENT "🧹 Killing previous SmartClockTests.exe if running..."
    )
endif()

# ======================================================
# === Discover & Run
# ======================================================
gtest_discover_tests(SmartClockTests
        PROPERTIES
        ENVIRONMENT "TEST_MODE=1;QT_QPA_PLATFORM=offscreen"
)

# ======================================================
# === UI TEST EXECUTABLE (no visible windows/tray)
# ======================================================
add_executable(SmartClockUITests
        test_main_ui.cpp
        test_alarm.cpp
        test_clock.cpp
        test_timer.cpp
        test_timerwindow.cpp
        test_stopwatch.cpp
        test_controllers.cpp
        test_windowedit.cpp
        test_mainwindow.cpp
)

target_compile_definitions(SmartClockUITests PRIVATE UNIT_TESTING)

target_link_libraries(SmartClockUITests
        PRIVATE
        SmartClockLib
        GTest::gtest_main
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Test
        Qt6::Multimedia
)

if (WIN32)
    add_custom_command(TARGET SmartClockUITests
            PRE_LINK
            COMMAND taskkill /F /IM SmartClockUITests.exe 2> NUL || exit 0
            COMMENT "Killing previous SmartClockUITests.exe if running..."
    )
endif()

gtest_discover_tests(SmartClockUITests
        PROPERTIES
        ENVIRONMENT "TEST_MODE=1;QT_QPA_PLATFORM=offscreen"
)
