# This file provides CMake Config Package functionality.

# This file enables the 'find_package' command to include the prv-ble resources.

get_filename_component(PRV_BLE_BASE ${CMAKE_CURRENT_LIST_DIR}/../../.. ABSOLUTE CACHE)

target_sources(app PRIVATE
        ${PRV_BLE_BASE}/lib/prv-ble-firmware/src/prv_hardware_service.c
        )

target_include_directories(app PRIVATE
        ${PRV_BLE_BASE}/lib/prv-ble-firmware/include
        )

include(${CMAKE_CURRENT_LIST_DIR}/gen_prv_UUIDs.cmake)
