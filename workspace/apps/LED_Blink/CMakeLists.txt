# Minimum CMake version
cmake_minimum_required(VERSION 3.22.0)

# Locate the Zephyr RTOS source
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Use CMake with -DBOARD_FLASH_RUNNER=jlink to change the default OpenOCD runner to J-Link.
set(BOARD_FLASH_RUNNER jlink)

# Name the project
project(LED_Blink)

# Locate the source code for the application
target_sources(app PRIVATE src/main.c)