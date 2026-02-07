cmake_minimum_required(VERSION 3.16)

project(DCharVault VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Sql)
qt_standard_project_setup(REQUIRES 6.8)

# ==============================================================================
# 3. PLATFORM SPECIFIC SETUP (Manual & Offline)
# ==============================================================================

# --- WINDOWS SETUP ---
if(WIN32 AND NOT ANDROID)
    # CONFIGURATION: Point this to your VCPKG 'installed' folder
    set(SODIUM_ROOT "D:/vcpkg/installed/x64-windows")

    # Safety Check
    if(NOT EXISTS "${SODIUM_ROOT}/include/sodium.h")
        message(FATAL_ERROR "Windows Libsodium missing! \nExpected at: ${SODIUM_ROOT}/include/sodium.h")
    endif()

    # Setup Headers
    include_directories(${SODIUM_ROOT}/include)

    # Find the Library explicitly
    find_library(SODIUM_LIB
        NAMES libsodium sodium
        PATHS "${SODIUM_ROOT}/lib"
        NO_DEFAULT_PATH
    )

    if(NOT SODIUM_LIB)
        message(FATAL_ERROR "Could not find 'libsodium.lib' in ${SODIUM_ROOT}/lib")
    endif()

    message(STATUS "Windows Security Core linked: ${SODIUM_LIB}")
endif()

# --- ANDROID SETUP ---
if(ANDROID)
    # CONFIGURATION: Point this to where you copied your Android libs
    # (e.g., D:/AndroidLibs/libsodium)
    set(SODIUM_ANDROID_ROOT "D:/AndroidLibs/libsodium")

    # 1. Auto-detect Architecture (arm64-v8a, armeabi-v7a, x86, etc.)
    # ANDROID_ABI is automatically set by the Android deployment tool.
    message(STATUS "Configuring for Android Arch: ${ANDROID_ABI}")

    # 2. Build the path to the static library (.a)
    # Assumes structure: .../libsodium/arm64-v8a/lib/libsodium.a
    set(SODIUM_LIB_PATH "${SODIUM_ANDROID_ROOT}/${ANDROID_ABI}/lib/libsodium.a")

    # Safety Check
    if(NOT EXISTS "${SODIUM_LIB_PATH}")
        message(FATAL_ERROR "Android Libsodium missing for ${ANDROID_ABI}! \nExpected at: ${SODIUM_LIB_PATH}")
    endif()

    # Setup Headers & Lib variable
    include_directories("${SODIUM_ANDROID_ROOT}/include")
    set(SODIUM_LIB "${SODIUM_LIB_PATH}")

    # Android specific logging (useful for debugging encrypted crashes)
    find_library(ANDROID_LOG_LIB log)

    message(STATUS "Android Security Core linked: ${SODIUM_LIB}")
endif()


qt_add_executable(appDCharVault
    MANUAL_FINALIZATION
    main.cpp
)

qt_add_qml_module(appDCharVault
    URI DCharVault
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES model/VaultModel.cpp
        SOURCES viewmodel/LoginViewModel.cpp
        QML_FILES ui/login.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appDCharVault PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appDCharVault
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appDCharVault
    PRIVATE Qt6::Quick Qt6::Sql
    PRIVATE ${SODIUM_LIB}      # D: drive library
    PRIVATE ${ANDROID_LOG_LIB} # Android logger
)

include(GNUInstallDirs)
install(TARGETS appDCharVault
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
