# PRV Hardware Service
This service provides information about the hardware outputs of a given peripheral device.
Additionally, authenticated (bonded) central devices can control the maximum output levels
of the defined hardware outputs.

#### Output Definitions Characteristic
These values define the the ways in which the hardware can provide stimulus.

##### Byte 0:
The first byte defines the total count of independent logical channels for the device output.

##### Byte 1:
The second byte defines the total distinct types of outputs for the device.

##### Output Definitions
The remaining bytes of the characteristic are pairs of `uint16` values,
with each pair corresponding to one *type* of device output, `N`, as defined in Byte 1.

###### Bytes 2 + 4*N, 3 + 4*N
The first `uint16` value of the Output Definitions pair is an enum indicating one of the types
of output for the device.

The defined output types are:
* `0x0001` - Rotational Vibration
* `0x0002` - Linear Vibration
* `0x0003` - Linear Actuation
* `0x0004` - TENS

###### Bytes 3 + 4*N, 4 + 4*N
The second `uint16` value of the Output Definitions pair is a bitfield representation of the
output channels of the device.
The bits set in the bitfield indicate which device output channels correspond to the output type
defined in the previous two bytes.
Each device output channel will be indicated in only one bitfield.

##### Output Definitions Characteristic Examples:
The plug has 1x e-stim output and 1x rotational vibration output,
thus it's Characteristic will be 10 bytes: 1 + 1 + 2*(2 + 2):

`0x02 0x02 0x0004 0x0001 0x0001 0x0002`

The box has 4x e-stim outputs, thus it's Characteristic will be only 6 bytes: 1 + 1 + 1*(2 + 2):

`0x04 0x01 0x0004 0x000F`

### Output Bounds Values
These values define the user-controllable maximum (and sometimes minimum) boundary values of the
device outputs.
Patterns control the device outputs over the range defined by the boundaries set by these values.
Output Bounds Values are interpreted as a little-endian `uint16` when they define one value,
or as two `uint8`s when they define two values.

These bytes are interpreted different depending upon the output type to which they are associated:
* Rotational Vibration: Maximum intensity
  * A value between `0x00` and `0x69` (0 to 105), with `0x69` representing the maximum intensity possible.
* Linear Vibration: As yet undefined
* Linear Actuation: Stroke maximum, stroke minimum
  * Each value is a fraction (out of `0xFF`) of the total actuation distance.
  * The minimum value always follows the maximum value, and must be less than or equal to the maximum value.
* TENS: Maximum voltage
  * A value between `0x00` and `0xFFF`, with `0xFFF`
    representing the maximum voltage output of a 12-bit Digital to Analog encoder (12-bit DAC).
  
#### Output Bounds Characteristic
An array of the Output Bounds Values of the device, one per channel,
in the same order as the Output Definitions channels bitfield(s).
The interpretation of each value is implicitly defined by the type-to-channel mapping in the
Output Definitions Characteristic.

#### Set Output Bounds Characteristic
Writing to this characteristic sets the Output Bounds Values of one or more device output channels.
The first value of the Characteristic the new Output Bounds Value to set on one or more output channels.
The second value is a bitfield mapping of which channels to set the new Value on,
with the bitfield mapping defined by the Output Definitions Characteristic.
