
cmake_minimum_required(VERSION 3.13.1)

set(BOARD nrf52833dk_nrf52833)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(maximum-throughput-example)

target_include_directories(app
    PRIVATE
        include
)

target_sources(app
    PRIVATE
        src/main.c
        src/ble.c
        src/test_service.c
)
