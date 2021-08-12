//
// Created by Benjamin Riggs on 8/11/21.
//

#include "prv_hardware_service.h"

static output_level_t *output_levels;
static prv_output_definitions_characteristic_t *definitions_characteristic;

int prv_hardware_service_init(prv_hw_service_init_t *init) {
    __ASSERT(init != NULL, "Invalid init parameter");
    __ASSERT(init->output_definitions_characteristic != NULL, "Invalid output definitions characteristic pointer");
    __ASSERT(init->output_definitions_characteristic->output_types_count == ARRAY_SIZE(init->output_type_definitions),
             "Output definitions types count doesn't match type definition array size");

    definitions_characteristic = init->output_definitions_characteristic;

    output_levels = k_calloc(definitions_characteristic->output_types_count, sizeof(output_level_t));
    if (output_levels == NULL) return -ENOMEM;

    return 0;
}
