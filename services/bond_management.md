# Bond Management Service
This service allows an authenticated (bonded) device to delete bonds stored on the device.

#### Bond Management Feature Characteristic
This characteristic provides a bitfield indicating what features, or tasks, the device supports.
PRV devices will return a value of `0x010410`, which indicates the tasks described below.

#### Bond Management Control Point Characteristic
This Characteristic will accept a write of a 1-byte opcode to perform one of three different tasks:
* `0x03` - Delete the bond information of the central device writing to this Characteristic.
* `0x06` - Delete all bonds saved on the peripheral device.
* `0x09` - Delete all bonds except the information of the central device writing to this Characteristic.
