# This file enables the 'find_package' command to include the prv-ble resources.

get_filename_component(PRV_BLE_BASE ${CMAKE_CURRENT_LIST_DIR}/../../.. ABSOLUTE CACHE)

target_sources(app PRIVATE
        ${PRV_BLE_BASE}/lib/prv-ble-firmware/src/prv_hardware_service.c
        )

target_include_directories(app PRIVATE
        ${PRV_BLE_BASE}/lib/prv-ble-firmware/include
        )

# Generate prv_UUIDs.h
set(OUTPUT_FILE ${ZEPHYR_BINARY_DIR}/include/generated/prv_UUIDs.h)

generate_unique_target_name_from_filename(${OUTPUT_FILE} GENERATED_TARGET_NAME)

add_custom_target(${GENERATED_TARGET_NAME} DEPENDS ${OUTPUT_FILE})

# Called during build, not during configure
add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${PRV_BLE_BASE}/lib/prv-ble-firmware/gen_macros.py
        ${PRV_BLE_BASE}/devices/${DEVICE}.json
        ${OUTPUT_FILE}
        DEPENDS ${PRV_BLE_BASE}/devices/${DEVICE}.json
        DEPENDS ${PRV_BLE_BASE}/lib/prv-ble-firmware/gen_macros.py
)

add_dependencies(app ${GENERATED_TARGET_NAME})
