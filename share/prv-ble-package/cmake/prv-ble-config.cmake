# This file provides CMake Config Package functionality.

# This file enables the 'find_package' command to include the prv-ble resources.

get_filename_component(PRV_BLE_BASE ${CMAKE_CURRENT_LIST_DIR}/../../.. ABSOLUTE CACHE)

target_sources(app PRIVATE
        ${PRV_BLE_BASE}/src/prv_hardware_service.c
        )

target_include_directories(app PRIVATE
        ${PRV_BLE_BASE}/include
        )

include(gen_prv_UUIDs.cmake)
