//
// Created by Benjamin Riggs on 8/11/21.
//

#include "prv_hardware_service.h"

#include "prv_UUIDs.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include <logging/log.h>

LOG_MODULE_REGISTER(prv_ble, LOG_LEVEL_DBG);

static prv_hw_service_t *service = NULL;


static ssize_t output_definitions_read(struct bt_conn *conn,
                                       struct bt_gatt_attr const *attr,
                                       void *buf,
                                       uint16_t len,
                                       uint16_t offset) {
    ARG_UNUSED(conn);
    ARG_UNUSED(attr);

    if (offset >= sizeof(prv_output_definitions_characteristic_t)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
    if (len + offset > sizeof(prv_output_definitions_characteristic_t)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    LOG_DBG("%d: Reading output definitions characteristic, %d bytes", (int) conn, len);

    memcpy(buf, service->output_definitions_characteristic + offset, len);

    // Returns: Number of bytes read, or in case of an error BT_GATT_ERR() with a specific ATT error code.
    return sizeof(prv_output_definitions_characteristic_t);
}


static ssize_t output_levels_read(struct bt_conn *conn,
                                  struct bt_gatt_attr const *attr,
                                  void *buf,
                                  uint16_t len,
                                  uint16_t offset) {
    ARG_UNUSED(conn);
    ARG_UNUSED(attr);

    if (offset >= sizeof(service->output_levels)) return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    if (len + offset > sizeof(service->output_levels)) return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);

    LOG_DBG("%d: Reading output levels, %d bytes", (int) conn, len);

    memcpy(buf, (void *) (service->output_levels + offset), len);

    // Returns: Number of bytes read, or in case of an error BT_GATT_ERR() with a specific ATT error code.
    return len;
}


#define SET_OUTPUT_LEVEL_SIZE (2 * sizeof(uint16_t))
static ssize_t write_set_output_levels(struct bt_conn *conn,
                                       struct bt_gatt_attr const *attr,
                                       void const *buf,
                                       uint16_t len,
                                       uint16_t offset,
                                       uint8_t flags) {
    ARG_UNUSED(attr);
    ARG_UNUSED(flags);

    if (offset != 0) return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    if (len != SET_OUTPUT_LEVEL_SIZE) return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);

    uint16_t bitfield = ((uint16_t *) buf)[0];
    output_level_t level = ((uint16_t *) buf)[1];

    LOG_DBG("%d: Setting output levels - bitfield: %d, level: %d", (int) conn, bitfield, level);

    if (service->callback != NULL) {
        int err = service->callback(conn, bitfield, level);
        if (err) return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
    }

    bool notify = false;
    for (int i = 0; i < CHANNEL_COUNT; ++i) {
        if (bitfield & (1U << i) && service->output_levels[i] != level) {
            service->output_levels[i] = level;
            notify = true;
        }
    }

    if (notify) {
        int err = prv_hw_service_update_levels((void *) service->output_levels);
        if (err) LOG_DBG("Notify during write callback failed: %d", err);
    }

    // Returns: Number of bytes written, or in case of an error BT_GATT_ERR() with a specific ATT error code.
    return SET_OUTPUT_LEVEL_SIZE;
}


BT_GATT_SERVICE_DEFINE(
    prv_hardware_service_definition,
    BT_GATT_PRIMARY_SERVICE(PRV_HARDWARE_SERVICE_UUID),
    BT_GATT_CHARACTERISTIC(
        PRV_HARDWARE_SERVICE_OUTPUT_DEFINITIONS_UUID,
        BT_GATT_CHRC_READ,
        BT_GATT_PERM_READ,
        output_definitions_read,
        NULL,
        NULL
    ),
    BT_GATT_CHARACTERISTIC(
        PRV_HARDWARE_SERVICE_OUTPUT_LEVELS_UUID,
        BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
        BT_GATT_PERM_READ_AUTHEN,
        output_levels_read,
        NULL,
        NULL
    ),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(
        PRV_HARDWARE_SERVICE_SET_OUTPUT_LEVELS_UUID,
        BT_GATT_CHRC_WRITE,
        BT_GATT_PERM_WRITE_AUTHEN,
        NULL,
        write_set_output_levels,
        NULL
    )
);


int prv_hardware_service_init(prv_hw_service_t *init) {
    __ASSERT(init != NULL, "Invalid init parameter");
    __ASSERT(init->output_definitions_characteristic != NULL, "Invalid output definitions characteristic pointer");
    __ASSERT(init->output_definitions_characteristic->output_types_count == ARRAY_SIZE(init->output_type_definitions),
             "Output definitions types count doesn't match type definition array size");

    service = init;
    return 0;
}


int prv_hw_service_update_levels(output_level_t const *levels) {
    for (int i = 0; i < CHANNEL_COUNT; ++i) {
        service->output_levels[i] = levels[i];
    }

    struct bt_gatt_notify_params params = {0};

    params.attr = &prv_hardware_service_definition.attrs[3];
    params.data = levels;
    params.len = sizeof(output_level_t) * CHANNEL_COUNT;

    int err = bt_gatt_notify_cb(NULL, &params);
    if (err) return err;

    return 0;
}
