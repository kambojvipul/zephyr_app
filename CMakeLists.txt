cmake_minimum_required(VERSION 3.22.0)

set(DTC_EXTRA_BINDINGS "${CMAKE_CURRENT_SOURCE_DIR}/dts/bindings")

#set(CONF_FILE "boards/xiao_esp32c3.conf")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(blink)

target_sources(app PRIVATE 
src/main.c
src/adc_sampler.c
src/ble.c
src/buttons.c
src/status_led.c)

target_include_directories(app PRIVATE . include)

