################################################################################
# @file   CMakeLists.txt
# @brief  Pump control thread module CMakeLists file.
#
# @author brice.c.aa
################################################################################

# Create and configure the module
qt_add_library(
    pump_thread
    STATIC
    # Header
    pump_thread.h
    # Source
    pump_thread.cpp
)

# Ensure this module has access to the necessary 3rd-party libraries
target_include_directories(
    pump_thread
    # Make sure the path to this directory is automatically added as an include
    # path to all targets using this library
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

# Tell the linker which libraries to link against
target_link_libraries(
    pump_thread
    # Qt
    PRIVATE Qt${QT_VERSION_MAJOR}::Core
    PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort
)
