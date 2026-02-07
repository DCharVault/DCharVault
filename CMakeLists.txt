cmake_minimum_required(VERSION 3.16)

project(DCharVault VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Sql)
qt_standard_project_setup(REQUIRES 6.6)

# ==============================================================================
# 3. PLATFORM SPECIFIC SETUP (Windows, Android, Linux)
# ==============================================================================

# --- WINDOWS SETUP ---
if(WIN32 AND NOT ANDROID)
    # 1. Look for the path in System Environment Variables
    if(DEFINED ENV{SODIUM_ROOT_ENV})
        set(SODIUM_ROOT $ENV{SODIUM_ROOT_ENV})
        message(STATUS "Using Sodium from Environment: ${SODIUM_ROOT}")
    else()
        # 2. Fallback: Use local hardcoded path
        set(SODIUM_ROOT "D:/vcpkg/installed/x64-windows")
        message(WARNING "Environment variable SODIUM_ROOT_ENV not found. Using local fallback: ${SODIUM_ROOT}")
    endif()

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
    # 1. Look for the path in System Environment Variables
    if(DEFINED ENV{SODIUM_ANDROID_ROOT_ENV})
        set(SODIUM_ANDROID_ROOT $ENV{SODIUM_ANDROID_ROOT_ENV})
        message(STATUS "Using Android Sodium from Environment: ${SODIUM_ANDROID_ROOT}")
    else()
        # 2. Fallback: Use local hardcoded path
        set(SODIUM_ANDROID_ROOT "D:/AndroidLibs/libsodium")
        message(WARNING "Environment variable SODIUM_ANDROID_ROOT_ENV not found. Using local fallback.")
    endif()

    # 3. Auto-detect Architecture
    message(STATUS "Configuring for Android Arch: ${ANDROID_ABI}")

    # 4. Build the path to the static library (.a)
    set(SODIUM_LIB_PATH "${SODIUM_ANDROID_ROOT}/${ANDROID_ABI}/lib/libsodium.a")

    # Safety Check
    if(NOT EXISTS "${SODIUM_LIB_PATH}")
        message(FATAL_ERROR "Android Libsodium missing for ${ANDROID_ABI}! \nExpected at: ${SODIUM_LIB_PATH}")
    endif()

    # Setup Headers & Lib variable
    include_directories("${SODIUM_ANDROID_ROOT}/include")
    set(SODIUM_LIB "${SODIUM_LIB_PATH}")

    # Android specific logging
    find_library(ANDROID_LOG_LIB log)

    message(STATUS "Android Security Core linked: ${SODIUM_LIB}")
endif()

# --- LINUX / UNIX SETUP ---
if(UNIX AND NOT ANDROID AND NOT APPLE)
    # On Linux, we use PkgConfig to find system libraries automatically
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SODIUM REQUIRED libsodium)

    # CRITICAL FIX: Explicitly include the directories found by PkgConfig
    # This fixes the 'sodium.h not found' error
    include_directories(${SODIUM_INCLUDE_DIRS})
    link_directories(${SODIUM_LIBRARY_DIRS})
    
    set(SODIUM_LIB ${SODIUM_LIBRARIES})

    message(STATUS "Linux Sodium Includes: ${SODIUM_INCLUDE_DIRS}")
    message(STATUS "Linux Security Core linked: ${SODIUM_LIB}")
endif()

# ==============================================================================
# 4. TARGET DEFINITION
# ==============================================================================
qt_add_executable(appDCharVault
    MANUAL_FINALIZATION
    main.cpp
)

qt_add_qml_module(appDCharVault
    URI DCharVault
    VERSION 1.0
    QML_FILES
        Main.qml
        ui/login.qml
    SOURCES 
        model/VaultModel.cpp
        viewmodel/LoginViewModel.cpp
)

# Qt for iOS/macOS bundle settings
set_target_properties(appDCharVault PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appDCharVault
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Link Libraries
target_link_libraries(appDCharVault
    PRIVATE Qt6::Quick Qt6::Sql
    PRIVATE ${SODIUM_LIB}      
    PRIVATE ${ANDROID_LOG_LIB} 
)

include(GNUInstallDirs)
install(TARGETS appDCharVault
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
