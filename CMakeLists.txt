cmake_minimum_required(VERSION 3.16)

project(DCharVault VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Sql QuickControls2 QuickDialogs2)

qt_standard_project_setup(REQUIRES 6.8)

# 3-> PLATFORM SPECIFIC SETUP (Windows, Android, Linux)

# --- WINDOWS SETUP
if(WIN32 AND NOT ANDROID)
    if(DEFINED ENV{SODIUM_ROOT_ENV})
        set(SODIUM_ROOT $ENV{SODIUM_ROOT_ENV})
        message(STATUS "Using Sodium from Environment: ${SODIUM_ROOT}")
    else()
        set(SODIUM_ROOT "D:/vcpkg/installed/x64-windows")
        message(WARNING "Environment variable SODIUM_ROOT_ENV not found. Using local fallback.")
    endif()

    if(NOT EXISTS "${SODIUM_ROOT}/include/sodium.h")
        message(FATAL_ERROR "Windows Libsodium missing! \nExpected at: ${SODIUM_ROOT}/include/sodium.h")
    endif()

    # Setup Headers
    include_directories(${SODIUM_ROOT}/include)

    find_library(SODIUM_LIB
        NAMES libsodium sodium
        PATHS "${SODIUM_ROOT}/lib"
        NO_DEFAULT_PATH
    )
    message(STATUS "Windows Security Core linked: ${SODIUM_LIB}")
endif()

# --- ANDROID SETUP
if(ANDROID)
    if(DEFINED ENV{SODIUM_ANDROID_ROOT_ENV})
        set(SODIUM_ANDROID_ROOT $ENV{SODIUM_ANDROID_ROOT_ENV})
    else()
        set(SODIUM_ANDROID_ROOT "D:/AndroidLibs/libsodium")
        message(WARNING "Using hardcoded Android path.")
    endif()

    message(STATUS "Configuring for Android Arch: ${ANDROID_ABI}")
    
    # 1. FIX: Point to the ABI specific folder (arm64-v8a), not the root!
    set(SODIUM_INCLUDE_PATH "${SODIUM_ANDROID_ROOT}/${ANDROID_ABI}/include")
    set(SODIUM_LIB_PATH "${SODIUM_ANDROID_ROOT}/${ANDROID_ABI}/lib/libsodium.a")

    if(NOT EXISTS "${SODIUM_LIB_PATH}")
        message(FATAL_ERROR "Android Libsodium missing! Checked: ${SODIUM_LIB_PATH}")
    endif()

    # 2. Add the Correct Include Path
    include_directories("${SODIUM_INCLUDE_PATH}")
    set(SODIUM_LIB "${SODIUM_LIB_PATH}")

    find_library(ANDROID_LOG_LIB log)
    message(STATUS "Android Security Core linked: ${SODIUM_LIB}")
endif()

# --- LINUX / UNIX SETUP 
if(UNIX AND NOT ANDROID AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SODIUM REQUIRED libsodium)

    find_path(SODIUM_INCLUDE_DIR sodium.h 
        PATH_SUFFIXES sodium 
        PATHS ${SODIUM_INCLUDE_DIRS} /usr/include /usr/local/include
    )
    
    # Add both paths to be safe (One for <sodium.h>, one for <sodium/sodium.h>)
    include_directories(${SODIUM_INCLUDE_DIRS})
    if(SODIUM_INCLUDE_DIR)
        include_directories(${SODIUM_INCLUDE_DIR})
    endif()
    
    set(SODIUM_LIB ${SODIUM_LIBRARIES})
    message(STATUS "Linux Security Core linked: ${SODIUM_LIB}")
endif()


# --- 4. APP TARGET ---

qt_add_executable(appDCharVault
    MANUAL_FINALIZATION
    main.cpp
)

# Base QML files that every platform needs
set(APP_QML_FILES
    Main.qml
    ui/common/login.qml
   # ui/common/loginView.qml 
    ui/common/about.qml
)

# Branch the UI files based on the platform
if(ANDROID)
    list(APPEND APP_QML_FILES
        ui/android/EditorView.qml
        # ui/android/EditorToolbar.qml
        ui/android/SidebarView.qml
        ui/android/HomeView.qml
    )
else()
    list(APPEND APP_QML_FILES
        ui/desktop/EditorView.qml
        ui/desktop/EditorToolbar.qml
        ui/desktop/SidebarView.qml
        ui/desktop/HomeView.qml
    )
endif()

qt_add_qml_module(appDCharVault
    URI DCharVault
    VERSION 1.0
    QML_FILES
        ${APP_QML_FILES}
    SOURCES
        model/DiaryEntry.h
        model/DiaryManager.h
        model/DiaryEntry.cpp
        model/VaultModel.cpp
        viewmodel/LoginViewModel.cpp
        model/DiaryManager.cpp
        SOURCES model/DatabaseManager.cpp
        SOURCES model/DatabaseManager.h
        SOURCES model/EncryptionManager.h
        SOURCES model/EncryptionManager.cpp
        SOURCES viewmodel/LoginViewModel.h

        SOURCES viewmodel/DiaryViewModel.h
        SOURCES viewmodel/DiaryViewModel.cpp
        SOURCES viewmodel/DiaryListModel.h
        SOURCES viewmodel/DiaryListModel.cpp
        SOURCES model/SecureAllocator.h
        SOURCES viewmodel/SecurePasswordInput.h
        SOURCES viewmodel/SecurePasswordInput.cpp
)


set_target_properties(appDCharVault PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appDCharVault
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appDCharVault
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Sql
    PRIVATE Qt6::QuickControls2
    PRIVATE Qt6::QuickDialogs2
    PRIVATE ${SODIUM_LIB}
)

# Only link Android log lib if we are actually on Android
if(ANDROID)
    target_link_libraries(appDCharVault PRIVATE ${ANDROID_LOG_LIB})
endif()

include(GNUInstallDirs)
install(TARGETS appDCharVault
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(appDCharVault)
