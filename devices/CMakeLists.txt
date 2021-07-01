file(GLOB DEVICES *.json)
foreach (FILE IN LISTS DEVICES)
    get_filename_component(DEVICE ${FILE} NAME)
    set(OUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${DEVICE}.obj")

    # objcopy compiles data and provides the following symbols:
    #    _binary_<filename>_json_start
    #    _binary_<filename>_json_end
    #    _binary_<filename>_json_size
    # Verify:
    # $ arm-none-eabi-objdump -t external/prv_nRF5/prv_ble/devices/plug.json.obj
    #
    # external/prv_nRF5/prv_ble/devices/plug.json.obj:     file format elf32-littlearm
    #
    # SYMBOL TABLE:
    # 00000000 l    d  .data  00000000 .data
    # 00000000 g       .data  00000000 _binary_plug_json_start
    # 000000bd g       .data  00000000 _binary_plug_json_end
    # 000000bd g       *ABS*  00000000 _binary_plug_json_size


    add_custom_command(OUTPUT "${OUT_FILE}"
            COMMAND "${CMAKE_OBJCOPY}" --input-target binary --output-target elf32-littlearm --binary-architecture arm ${DEVICE} ${OUT_FILE}
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${DEVICE}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            )
    list(APPEND DEVICES_FILES "${OUT_FILE}")
endforeach ()
add_library(prv_devices OBJECT ${DEVICES_FILES})

# add_library won't actually pass binary files to the linker (only files its compiled), so link against them directly.
target_link_libraries(${PROJECT_NAME} PRIVATE prv_devices ${DEVICES_FILES})
