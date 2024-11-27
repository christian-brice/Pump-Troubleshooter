################################################################################
# @file   CMakeLists.txt
# @brief  Root CMakeLists file.
#
# @author brice.c.aa
################################################################################

cmake_minimum_required(VERSION 3.16)

project(
    LIBRA-Pump-TS  # TS = Troubleshooter
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
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core SerialPort Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core SerialPort Widgets)

# Add project subdirectories
add_subdirectory(src)

# Add target executable
qt_add_executable(LIBRA-Pump-TS MANUAL_FINALIZATION ${PROJECT_SRC})

# Add all libraries directly used by this project to the include path
target_include_directories(
    LIBRA-Pump-TS
    # Project
    PRIVATE ${CMAKE_SOURCE_DIR}/src
)

# Tell the linker which libraries to link against
target_link_libraries(
    LIBRA-Pump-TS
    # Qt
    PRIVATE Qt${QT_VERSION_MAJOR}::Core
    PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    # Linux
    #PRIVATE udev  # TODO(brice.c.aa): is this necessary?
)

# Specify platform-specific properties
# ~~~
set_target_properties(
    LIBRA-Pump-TS
    PROPERTIES ${BUNDLE_ID_OPTION}
               MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
               MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
               MACOSX_BUNDLE TRUE
               WIN32_EXECUTABLE TRUE
)
# ~~~

# Auto-included by Qt -- is this necessary?
include(GNUInstallDirs)

# Specify installation settings
install(
    TARGETS LIBRA-Pump-TS
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Generate proper deployment settings for the target
qt_finalize_executable(LIBRA-Pump-TS)
