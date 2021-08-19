//
// Created by Benjamin Riggs on 8/11/21.
//

#ifndef PRV_HARDWARE_SERVICE_H
#define PRV_HARDWARE_SERVICE_H

#include <zephyr.h>
#include "prv_device.h"

typedef enum __packed {
    PRV_OUTPUT_ERM = 1,
    PRV_OUTPUT_LRA = 2,
    PRV_OUTPUT_LINEAR_ACTUATOR = 3,
    PRV_OUTPUT_TENS = 4,
    PRV_OUTPUT_LED = 5,
    PRV_OUTPUT_TYPE_TOP = 0xFFFF        // Use 16-bit values because JavaScript cannot parse unaligned values.
} prv_output_type_t;

typedef enum __packed {
    PRV_OUTPUT_LEVELS_MAX = 1,
    PRV_OUTPUT_LEVELS_MIN_MAX = 2,
    PRV_OUTPUT_LEVELS_RGB565 = 3,
    PRV_OUTPUT_LEVELS_TYPE_TOP = 0xFFFF // Use 16-bit values because JavaScript cannot parse unaligned values.
} prv_output_levels_type_t;

typedef struct __packed {
    prv_output_type_t output_type;
    bitfield_t channels_bitfield;
    prv_output_levels_type_t output_levels_type;
    output_level_t output_levels_limit;
} prv_output_type_definition_t;

typedef struct __packed {
    uint8_t output_channels_count;
    uint8_t output_types_count;
    prv_output_type_definition_t output_type_definitions[CHANNEL_TYPE_COUNT];
} prv_output_definitions_characteristic_t;

/** Called when values are written to Set Output Level Characteristic
 *
 * @param[in] conn      Opaque pointer to the connection
 * @param[in] bitfield  Bitfield written to Characteristic
 * @param[in,out] level In: Level written to Characteristic,
 *                      Out: Level set on device, notified on Output Levels Characteristic
 *
 * @retval 0            The provided values are valid
 * @retval -EINVAL      Invalid bitfield or level value
 * */
typedef int (*prv_hw_service_set_output_level_func_t)(void const *conn, bitfield_t bitfield, output_level_t *level);

/** Statically define struct, pass to init for verification. */
typedef struct {
    /** Data provided by the first Characteristic of the service */
    prv_output_definitions_characteristic_t const *const output_definitions_characteristic;
    /** Functions to call to apply output level changes, 1 per channel, in bitfield order */
    prv_hw_service_set_output_level_func_t level_func[CHANNEL_COUNT];
    /** Service-local state, only set initial values, do not mutate */
    output_level_t output_levels[CHANNEL_COUNT];
} prv_hw_service_t;

/**
 * @brief Initialize service handlers
 *
 * @param[in] init  Pointer to statically allocated and populate struct
 *
 * @retval 0        Success
 * @throws Assert   Invalid service definition
 */
int prv_hardware_service_init(prv_hw_service_t *init);

int prv_hw_service_update_levels(output_level_t const *levels, size_t len);

#endif //PRV_HARDWARE_SERVICE_H
