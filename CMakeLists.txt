cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 20)

set(ENV{PICO_BOARD} "pico2")
set(PICO_STDIO_ENABLE_CRLF_SUPPORT ON)
if (NOT $ENV{PICO_SDK_PATH})
    set($ENV{PICO_SDK_PATH} "/usr/share/pico-sdk/")
endif()
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(TrueControlFirmware C CXX ASM)

pico_sdk_init()

add_subdirectory(external/displaylib_16bit_PICO EXCLUDE_FROM_ALL)

add_executable(TrueControlFirmware
    src/main.cpp
    src/tusb/usb_descriptors.c
    src/repl/Repl.cpp
    src/repl/Executable.cpp
    src/ProfileManagement/Profile.cpp
    src/ProfileManagement/Manager.cpp
    src/ProfileManagement/ProfileExecutor.cpp
    src/ui/Screen.cpp
    src/ui/ProfileSelectionScreen.cpp
    src/ui/ProfileCreateScreen.cpp
    src/ui/component/Keyboard.cpp
    src/ui/component/Tile.cpp
    src/ui/ProfileDeleteScreen.cpp
    src/ui/ProfileEditScreen.cpp
    src/ui/ProfileRenameScreen.cpp
    src/ui/EditSequenceScreen.cpp
    src/ui/component/CreateTile.cpp
    src/ui/EditComboScreen.cpp
    src/ui/ProfileLoadScreen.cpp
    src/driver/MCP3204.cpp
        src/debug/lapTimer.cpp
        include/debug/lapTimer.h
)

pico_enable_stdio_usb(TrueControlFirmware 1)
pico_enable_stdio_uart(TrueControlFirmware 1)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(TrueControlFirmware PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/include/tusb)

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(TrueControlFirmware PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board pico_displaylib_16 hardware_spi)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
#target_compile_definitions(dev_hid_composite PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_add_extra_outputs(TrueControlFirmware)
