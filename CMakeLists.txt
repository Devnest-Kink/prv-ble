set(BUILD_SHARED_LIBS false)

file(GLOB SERVICES services/*.json)

foreach (FILE IN LISTS SERVICES)
    get_filename_component(SERVICE ${FILE} NAME)
    set(OUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${SERVICE}.obj")
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
