cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME singal_generate)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM, C and C++ languages
enable_language(C ASM CXX)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    MiddleWare/FreeRTOS/Src/croutine.c
    MiddleWare/FreeRTOS/Src/event_groups.c
    MiddleWare/FreeRTOS/Src/list.c
    MiddleWare/FreeRTOS/Src/queue.c
    MiddleWare/FreeRTOS/Src/stream_buffer.c
    MiddleWare/FreeRTOS/Src/tasks.c
    MiddleWare/FreeRTOS/Src/timers.c
    MiddleWare/FreeRTOS/Portable/port.c
    MiddleWare/FreeRTOS/Portable/heap_4.c
    MiddleWare/class/Src/led.cpp
    MiddleWare/class/Src/oled.cpp
    MiddleWare/soft_drive/Src/i2c.cpp
    Application/Src/App_task.cpp
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    MiddleWare/FreeRTOS/Inc
    MiddleWare/FreeRTOS/Portable
    MiddleWare/class/Inc
    MiddleWare/soft_drive/Inc
    Application/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# Generate .bin and .hex after build
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary  $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex    $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
    COMMENT "Generating .bin and .hex"
)
