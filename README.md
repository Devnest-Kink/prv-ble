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

#### UUIDs
The Bluetooth Spec defines a 16-bit shorthand for UUIDs for Services and Characteristics
defined by the Specification.
All 16-bit Bluetooth-defined UUIDs are substituted into a 'base' UUID,
`00000000-0000-1000-8000-00805F9B34FB`, by replacing the third and fourth bytes of
a big-endian UUID representation.

For hexadecimal representations of UUID, the net effect is that a four characters of the full
UUID are replaced by the 16-bit shorthand hex values. For all Bluetooth-assigned UUIDs,
substitute the four hex characters for the 'XXXX' characters in the base UUID as follows
`0000XXXX-0000-1000-8000-00805F9B34FB`.

#### `baseUUID`
The aforementioned UUID substitution scheme is replicated by many others in the device industry,
and is heavily leveraged by the BLE libraries used in PRV device firmware. 
Because of this, all Service and Characteristic definitions here follow this pattern.
Each service defines a `baseUUID` that the Service and Characteristic `UUID` fields will
be substituted into, following the same pattern as Bluetoot-assigned UUIDs.

In practical terms, each `UUID` field in a Service definition is used to replace the
fifth through eighth hex characters in the `baseUUID` string to create the full
128-bit UUID value for the Service or Characteristic. 

#### `service`
Service identifiers consist of of two keys/value pairs:
* `name` - human readable UTF-8 string
* `UUID` - 16-bit UUID-shorthand values expressed as hex characters

#### `characteristics`
This value is an array of BLE Characteristic definitions.
A Service will contain one or more Characteristics.

##### `name`
The Characteristic name describes the logical function of the Characteristic value.

##### `UUID`
The 16-bit UUID-shorthand of the Characteristic expressed as hex characters

##### `datatype`
This value specifies how the Characteristic value should be parsed.
Characteristic values are parsed as little endian beginning with the first datatype.

`datatype` is an array of objects with two keys, `type` and `count`
(or a string of `TBD` if the spec is unfinished):

###### `type`
A string specifying a data type. The allowed values are:
* `UTF8` - A UTF-8 encoded string
* `uint8`/`uint16`/`uint32` - Unsigned integers of 8, 16, or 32-bit lengths
* `int8`/`int16`/`int32` - 2's complement signed integers of 8, 16, or 32-bit lengths.

###### `count`
This value will be either a number or `null`.
* Numbers indicate how many consecutive `type`s to parse.
* `null` indicates that the Characteristic value is of variable length,
and all of the remaining bytes in the value should be interpreted as `type`.

##### `properties`
This object indicates what interactions the Characteristic supports.
Object keys directly correspond to common BLE Characteristic property names.
