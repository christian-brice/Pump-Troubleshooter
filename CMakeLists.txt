################################################################################
# @file   CMakeLists.txt
# @brief  Root CMakeLists file.
#
# @author brice.c.aa
################################################################################

cmake_minimum_required(VERSION 3.16)

project(
    Pump-Troubleshooter
    VERSION 0.1
    LANGUAGES CXX
)

# CMake options
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # used by clang-tidy

# Tell CMake to automatically handle Qt code generators and preprocessors
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Specify C++ standard to define available features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies (installed on system)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core SerialPort Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core SerialPort Widgets)

# Add project subdirectories
add_subdirectory(src)  # PROJECT_SOURCE

# Add target executable
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Pump-Troubleshooter MANUAL_FINALIZATION ${PROJECT_SOURCE})
    # Define target properties for Android with Qt 6 as:
    #    set_property(TARGET Pump-Troubleshooter APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
    #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
    # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Pump-Troubleshooter SHARED ${PROJECT_SOURCE})
        # Define properties for Android with Qt 5 after find_package() calls as:
        #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Pump-Troubleshooter ${PROJECT_SOURCE})
    endif()
endif()

# Add all libraries directly used by this project to the include path
target_include_directories(
    Pump-Troubleshooter
    # Project
    PRIVATE ${CMAKE_SOURCE_DIR}/src
)

# Tell the linker which libraries to link against
target_link_libraries(
    Pump-Troubleshooter
    # Qt
    PRIVATE Qt${QT_VERSION_MAJOR}::Core
    PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    # Linux
    PRIVATE udev
)

# Specify platform-specific properties
# ~~~
if(${QT_VERSION} VERSION_LESS 6.1.0)
    # Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
    # If you are developing for iOS or macOS you should consider setting an
    # explicit, fixed bundle identifier manually though.
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Pump-Troubleshooter)
endif()
set_target_properties(
    Pump-Troubleshooter
    PROPERTIES ${BUNDLE_ID_OPTION}
               MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
               MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
               MACOSX_BUNDLE TRUE
               WIN32_EXECUTABLE TRUE
)
# ~~~

# Auto-included by Qt
include(GNUInstallDirs)

# Specify installation settings
install(
    TARGETS Pump-Troubleshooter
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Generate proper deployment settings for the target
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Pump-Troubleshooter)
endif()
