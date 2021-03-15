set(BUILD_SHARED_LIBS false)

file(GLOB SERVICES services/*.json)

foreach (FILE IN LISTS SERVICES)
    get_filename_component(SERVICE ${FILE} NAME)
    set(OUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${SERVICE}.obj")

    # objcopy compiles data and provides the following symbols:
    #    _binary_<filename>_json_start
    #    _binary_<filename>_json_end
    #    _binary_<filename>_json_size
    # Verify:
    # $ arm-none-eabi-objdump -t external/prv_nRF5/prv_ble/bond_management.json.obj
    #
    # external/prv_nRF5/prv_ble/bond_management.json.obj:     file format elf32-littlearm
    #
    # SYMBOL TABLE:
    # 00000000 l    d  .data  00000000 .data
    # 00000000 g       .data  00000000 _binary_bond_management_json_start
    # 000003cb g       .data  00000000 _binary_bond_management_json_end
    # 000003cb g       *ABS*  00000000 _binary_bond_management_json_size

    add_custom_command(OUTPUT ${OUT_FILE}
            COMMAND "${CMAKE_OBJCOPY}" --input-target binary --output-target elf32-littlearm --binary-architecture arm ${SERVICE} ${OUT_FILE}
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/services/${SERVICE}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/services"
            )
    list(APPEND OUTPUT_FILES ${OUT_FILE})
endforeach ()

add_library(prv_services OBJECT ${OUTPUT_FILES})
# add_library won't actually pass binary files to the linker (only files its compiled), so link against them directly.
target_link_libraries(${PROJECT_NAME} PRIVATE prv_services ${OUTPUT_FILES})
