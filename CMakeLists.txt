# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(RP2040zeroTestTool C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

if (TARGET tinyusb_device)
    add_executable(RP2040zeroTestTool
            RP2040zeroTestTool.c
            )

    # Add pico_stdlib library which includes standard input/output functions
    target_link_libraries(RP2040zeroTestTool hardware_pio pico_stdlib hardware_pio )

    # Add pico_stdio_usb library for USB standard input/output
    target_link_libraries(RP2040zeroTestTool pico_stdio_usb pico_multicore)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(RP2040zeroTestTool 1)
    pico_enable_stdio_uart(RP2040zeroTestTool 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(RP2040zeroTestTool)

    # add url via pico_set_program_url
elseif(PICO_ON_DEVICE)
    message("Skipping RP2040zeroTestTool because TinyUSB submodule is not initialized in the SDK")
endif()




