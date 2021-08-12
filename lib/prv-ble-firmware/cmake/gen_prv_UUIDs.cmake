# Generate prv_UUIDs.h

set(OUTPUT_FILE ${ZEPHYR_BINARY_DIR}/include/generated/prv_UUIDs.h)

generate_unique_target_name_from_filename(${OUTPUT_FILE} GENERATED_TARGET_NAME)

add_custom_target(${GENERATED_TARGET_NAME} DEPENDS ${OUTPUT_FILE})

# Called during build, not during configure
add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${PRV_BLE_BASE}/lib/firmware/gen_macros.py
        ${PRV_BLE_BASE}/devices/${DEVICE}.json
        ${OUTPUT_FILE}
        DEPENDS ${PRV_BLE_BASE}/devices/${DEVICE}.json
        COMMENT Generating ${OUTPUT_FILE}
)

add_dependencies(app ${GENERATED_TARGET_NAME})
