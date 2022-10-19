cmake_minimum_required(VERSION 3.13)
set(PICO_ENABLE_CXX_EXCEPTIONS 1)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

include(pico_sdk_import.cmake)

project(wecker C CXX ASM)
set(CMAKE_C STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(wecker
    main.cpp
    tm1637.cpp
    ds3231.cpp
)
pico_generate_pio_header(wecker ${CMAKE_CURRENT_LIST_DIR}/PicoTM1637.pio)

pico_enable_stdio_usb(wecker 1)
pico_enable_stdio_uart(wecker 0)

pico_add_extra_outputs(wecker)

target_include_directories(wecker PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
target_include_directories(wecker PRIVATE include)


target_link_libraries(wecker pico_stdlib hardware_i2c hardware_pio hardware_rtc)
