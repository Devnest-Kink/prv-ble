# BLE Interface Definitions

## Device Definitions
Devices provide a collection of BLE Services.
`<device name>.json` files contain a top-level object with `name` and `services` keys.

#### `name`
This value is the canonical display name of the device.

#### `services`
This is an array of filenames of Service definitions.
Services will be presented on the device in the order of files in the array.

## DataTypes
Definitions using the `datastructure` JSON key are used to specify how binary data should be parsed.
Data is parsed as little endian beginning with the first DataType in an array.

A DataType is an object with three keys, `type`, `count`, and an optional `description`:

###### `datatype`
A string specifying a primitive data type or an array of DataTypes.
The allowed primitive values are:
* `UTF8` - A UTF-8 encoded string
* `uint8`/`uint16`/`uint32` - Unsigned integers of 8, 16, or 32-bit lengths
* `int8`/`int16`/`int32` - 2's complement signed integers of 8, 16, or 32-bit lengths

###### `count`
This value will be either a number or `null`.
* Numbers indicate how many consecutive `type`s to parse.
* `null` indicates that the Characteristic value is of variable length,
  and all of the remaining bytes in the value should be interpreted as `type`.

###### `description`
This is an optional entry provided only as a developer reference.

## Service Definitions
BLE Services are defined in individual files in the `services` directory.
Service files contain a top-level object with `service` and `characteristics` keys.

#### UUIDs
Bluetooth uses 128-bit UUIDs for identifiers of Services & Characteristics. 
The Bluetooth Spec defines a 16-bit shorthand for UUIDs for Services and Characteristics
defined by the Specification.
These shorthand values can be used as-is for Service and Characteristic identification.
All custom Services and Characteristics must use full 128-bit UUIDs.

#### `service`
Service identifiers consist of two keys/value pairs:
* `name` - human-readable UTF-8 string
* `UUID` - 128-bit or 16-bit shorthand UUID values expressed as hex characters

#### `characteristics`
This value is an array of BLE Characteristic definitions.
A Service will contain one or more Characteristics.

##### `name`
The Characteristic name describes the logical function of the Characteristic value.

##### `UUID`
The 128-bit or 16-bit shorthand UUID of the Characteristic expressed as hex characters.

##### `datastructure`
An array of DataTypes, as described above.

##### `properties`
This object indicates what interactions the Characteristic supports.
Object keys directly correspond to common BLE Characteristic property names.
