cmake_minimum_required(VERSION 3.16)

project(DCharVault VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Sql QuickControls2 QuickDialogs2)

qt_standard_project_setup(REQUIRES 6.8)

# -WINDOWS SETUP
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

    include_directories(${SODIUM_ROOT}/include)

    find_library(SODIUM_LIB
        NAMES libsodium sodium
        PATHS "${SODIUM_ROOT}/lib"
        NO_DEFAULT_PATH
    )
    message(STATUS "Windows Security Core linked: ${SODIUM_LIB}")
endif()

# -ANDROID SETUP
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

    include_directories("${SODIUM_INCLUDE_PATH}")
    set(SODIUM_LIB "${SODIUM_LIB_PATH}")

    find_library(ANDROID_LOG_LIB log)
    message(STATUS "Android Security Core linked: ${SODIUM_LIB}")
endif()

# -LINUX/UNIX SETUP
if(UNIX AND NOT ANDROID AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SODIUM REQUIRED libsodium)

    find_path(SODIUM_INCLUDE_DIR sodium.h
        PATH_SUFFIXES sodium
        PATHS ${SODIUM_INCLUDE_DIRS} /usr/include /usr/local/include
    )

    #both paths to be safe (One for <sodium.h>, one for <sodium/sodium.h>)
    include_directories(${SODIUM_INCLUDE_DIRS})
    if(SODIUM_INCLUDE_DIR)
        include_directories(${SODIUM_INCLUDE_DIR})
    endif()

    set(SODIUM_LIB ${SODIUM_LIBRARIES})
    message(STATUS "Linux Security Core linked: ${SODIUM_LIB}")
endif()


# Core Static Libs
add_library(DCharVault_Core STATIC
    model/EncryptionManager.cpp
    model/DatabaseManager.cpp
    model/DiaryManager.cpp
    model/DiaryEntry.cpp
    model/VaultModel.cpp
    model/SessionManager.cpp

)
target_link_libraries(DCharVault_Core PRIVATE ${SODIUM_LIB} Qt6::Core Qt6::Sql Qt6::Gui)
target_include_directories(DCharVault_Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

qt_add_executable(appDCharVault
    main.cpp
)

# static DCharVault_Core linking to appDCharVault
target_link_libraries(appDCharVault
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::QuickDialogs2
    Qt6::Sql
    ${SODIUM_LIB}
    DCharVault_Core #app connects to the models
)

set(APP_QML_FILES
    Main.qml
    ui/common/ThemeManager.qml
    ui/common/login.qml
    ui/common/VaultPasswordField.qml
   # ui/common/loginView.qml
    ui/common/about.qml
    ui/common/SettingsOverlay.qml
    ui/common/AndroidLogin.qml
    ui/common/micro-elements/DCharSwitch.qml
    ui/common/micro-elements/DCharComboBox.qml
    ui/common/tabs-settings/AppearanceTab.qml
    ui/common/tabs-settings/SecurityTab.qml
)


if(ANDROID)
    list(APPEND APP_QML_FILES
        ui/android/EditorView.qml
        # ui/android/EditorToolbar.qml
        ui/android/SidebarView.qml
        ui/android/HomeView.qml
        ui/android/virtualKeyboardPad.qml
    )
else()
    list(APPEND APP_QML_FILES
        ui/desktop/EditorView.qml
        ui/desktop/EditorToolbar.qml
        ui/desktop/SidebarView.qml
        ui/desktop/HomeView.qml
    )
endif()

set_source_files_properties(ui/common/ThemeManager.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

list(REMOVE_DUPLICATES APP_QML_FILES)

qt_add_qml_module(appDCharVault
    URI DCharVault
    VERSION 1.0
    QML_FILES
        ${APP_QML_FILES}
    SOURCES
        model/DiaryEntry.h
        model/DiaryManager.h
        model/DatabaseManager.h
        model/EncryptionManager.h
        model/TitleGenerators.h
        model/SecureAllocator.h
        model/SessionManager.h

        viewmodel/SecurePasswordInput.h
        viewmodel/DiaryListModel.h
        viewmodel/LoginViewModel.h
        viewmodel/DiaryViewModel.h
        viewmodel/SecureNetworkManager.h
        viewmodel/ClipboardSanitizer.h
        viewmodel/SessionViewModel.h
        viewmodel/TextHighlighter.h
        viewmodel/RichTextController.h

        viewmodel/LoginViewModel.cpp
        viewmodel/DiaryViewModel.cpp
        viewmodel/DiaryListModel.cpp
        viewmodel/SecurePasswordInput.cpp
        viewmodel/ClipboardSanitizer.cpp
        viewmodel/SessionViewModel.cpp
        viewmodel/TextHighlighter.cpp
        viewmodel/RichTextController.cpp
)

# The auto-generated qmltyperegistrations.cpp does #include <TextHighlighter.h>
# (angle-bracket), so the viewmodel/ dir must be on the include path.
target_include_directories(appDCharVault PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/viewmodel)

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

if(WIN32)
    target_link_libraries(appDCharVault PRIVATE dwmapi)
endif()

if(ANDROID)
    target_link_libraries(appDCharVault PRIVATE ${ANDROID_LOG_LIB})
endif()

include(GNUInstallDirs)
install(TARGETS appDCharVault
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# qt_finalize_executable(appDCharVault)

option(BUILD_TESTS "Build unit testing pipeline" ON)

if(BUILD_TESTS)
    enable_testing()

    # Fetch GoogleTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(DCharVault_tests tests/main_test.cpp tests/test_crypto.cpp tests/test_database.cpp)

    target_link_libraries(DCharVault_tests
        PRIVATE
        gtest
        DCharVault_Core
        ${SODIUM_LIB}
        Qt6::Core
        Qt6::Sql
    )

    include(GoogleTest)
    gtest_discover_tests(DCharVault_tests DISCOVERY_MODE PRE_TEST)

    if(WIN32)
        add_custom_command(TARGET DCharVault_tests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SODIUM_ROOT}/bin/libsodium.dll"
            $<TARGET_FILE_DIR:DCharVault_tests>
        )
    endif()


endif()
