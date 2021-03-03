# BLE Interface Definitions

## Device Definitions
Devices provide a collection of BLE Services.
`<device name>.json` files contain a top-level object with `name` and `services` keys.

#### `name`
This value is the canonical display name of the device.

#### `services`
This is an array of filenames of Service definitions.
Services will be presented on the device in the order of files in the array.

## Service Definitions
BLE Services are defined in individual files in the `services` directory.
Service files contain a top-level object with `service` and `characteristics` keys.

#### `service`
Service identifiers consist of of two keys/value pairs:
* `name` - human readable UTF-8 string
* `UUID` - 16- or 128-bit UUID values expressed as hex characters

##### 16-bit UUIDs
These are Bluetooth-assigned short-hand identifiers.
To create a proper 128-bit UUID from a 16-bit ID,
substitute the four hex characters for the 'XXXX' characters in the base UUID
"0000XXXX-0000-1000-8000-00805F9B34FB".

#### `characteristics`
This value is an array of BLE Characteristic definitions.
A Service will contain one or more Characteristics.

##### `name`
The Characteristic name describes the logical function of the Characteristic value.

##### `UUID`
The 16- or 128-bit UUID of the Characteristic.

##### `datatype`
An array of identifier strings that indicate how the Characteristic value should be parsed.
The valid list of identifier and their meanings are as follows:
* `UTF8` - A UTF-8 encoded string
* `uint8`/`uint16`/`uint24`/`uint32` - Unsigned integers of 8, 16, 24, or 32-bit lengths
* `int8`/`int16`/`int24`/`int32` - 2's complement signed integers of 8, 16, 24, or 32-bit lengths.

Characteristic values are parsed as little endian beginning with the left-most array identifier.

##### `properties`
This object indicates what interactions the Characteristic supports.
Object keys directly correspond to common BLE Characteristic property names.
