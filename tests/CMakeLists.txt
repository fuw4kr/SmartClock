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
        test_timer.cpp
        test_stopwatch.cpp
        test_clock.cpp
        test_alarm.cpp
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
        Qt6::Multimedia
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
