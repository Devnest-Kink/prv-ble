# PRV Hardware Service
This service provides information about the hardware outputs of a given peripheral device.
Additionally, authenticated (bonded) central devices can control the maximum output levels
of the defined hardware outputs.

### Output Bounds Values
These values define the user-controllable maximum boundary values of the device outputs.
Patterns control the device outputs over the range defined by the boundaries set by these values.
All Bounds Values are normalized to have zero as a minimum value.

#### Output Definitions Characteristic
These values define the the ways in which the hardware can provide stimulus.

##### Byte 0:
The first byte defines the total count of independent logical channels for the device output.

##### Byte 1:
The second byte defines the total distinct types of outputs for the device.

##### Output Definitions
The remaining bytes of the characteristic are quartets of `uint16` values (8-byte groups),
with each set corresponding to one *type* of device output, `N`, as defined in Byte 1.

###### Bytes 2 + 8*N, 3 + 8*N
The first `uint16` value of the Output Definitions is an enum indicating one of the types
of output for the device.

The defined output types are:
* `0x0001` - Rotational Vibration
* `0x0002` - Linear Vibration
* `0x0003` - Linear Actuation
* `0x0004` - TENS
* `0x0005` - LED  (Plug has 3-color LED used for user signaling and controllable by patterns)

###### Bytes 3 + 8*N, 4 + 8*N
The second `uint16` value of the Output Definitions is a bitfield representation of the
output channels of the device.
The bits set in the bitfield indicate which device output channels correspond to the output type
defined in the previous two bytes.
Each device output channel will be indicated in only one bitfield.

###### Bytes 5 + 8*N, 6 + 8*N
The third `uint16` value of the Output Definitions is an enum indicating the meaning of the two-byte Bounds Values
for this output type.

The defined Bounds Value types are:
* `0x0001` - Bounds values are `uint16` and define a maximum value of the output.
* `0x0002` - Bounds values are two `uint8`:
  * Byte 0: Sets the output's maximum value
  * Byte 1: Sets the output's minimum value
* `0x0003` - Bounds values are `RGB565` color values,
  indicating the maximum brightness of each of the three color channels.

###### Bytes 5 + 8*N, 6 + 8*N
The last two bytes of the Output Definitions set define the maximum value(s) for the 
output's Bounds values.
That is, these bytes define the maximum logical value of the corresponding output.
These bytes are interpreted according to the enum defined in the previous two bytes.

##### Example Output Definitions Characteristics
The plug has 1x e-stim output, 1x rotational vibration output, and 1x 3-color LED output,
thus it's characteristic will be the following 26 bytes: 1 + 1 + 3*(2 + 2 + 2 + 2):

`0x02 0x02 0x0004 0x0001 0x0001 0x0FFF 0x0001 0x0002 0x0001 0x0069 0x0005 0x0004 0x0003 0xFFFF`

* Counts
  * `0x02` - Two output channels on plug
  * `0x02` - Two *types* of output channels on plug
* TENS Output Definition
  * `0x0004` - The next 6 bytes describe the TENS output
  * `0x0001` - Bitfield indicating channel `1 << 0` is a TENS output
  * `0x0001` - Bounds Values for this output are `uint16` and allow the user to set the maximum level of the TENS output
  * `0x0FFF` - A `uint16` value that defines the maximum Bounds Value for the TENS output as `0xFFF`
* Vibrator Output Definition
  * `0x0001` - The next 6 bytes describe a eccentric rotational mass vibrator output
  * `0x0002` - Bitfield indicating channel `1 << 1` is a rotational vibrator output
  * `0x0001` - Bounds Values for this output are `uint16` and allow the user to set a maximum level
  * `0x0069` - A `uint16` value that defines the maximum Bounds Value for the rotating vibrator output as 105 (`0x69`)
* LED Output Definition 
  * `0x0005` - The next 6 bytes describe an LED output
  * `0x0004` - Bitfield indicating channel `1 << 2` is an LED output
  * `0x0003` - Bounds Values for this output are an `RGB565` color value and
    allow the user to set a maximum level of brightness for each color
  * `0xFFFF` - An `RGB565` value that defines the maximum Bounds Value for the LED output as `0b11111 0b111111 0b11111`

The box has 4x e-stim outputs, thus it's characteristic will be only 10 bytes: 1 + 1 + 1*(2 + 2 + 2 + 2):

`0x04 0x01 0x0004 0x000F 0x0001 0x0FFF`
* `0x04` - Four output channels on plug
* `0x01` - One *type* of output channels on plug
* `0x0004` - The next 6 bytes describe the TENS output
* `0x000F` - Bitfield indicating channels `1 << 0`, `1 << 1`, `1 << 2`, and `1 << 3` are TENS outputs
* `0x0001` - Bounds Values for this output are `uint16` and allow the user to set the maximum level of the TENS output
* `0x0FFF` - A `uint16` value that defines the maximum Bounds Value for the TENS output as `0xFFF`

#### Output Bounds Characteristic
This characteristic provides an array of the output Bounds Values of the device, one per channel,
in the same order as the Output Definitions channels bitfield(s).
The interpretation of each value is defined by the corresponding type as defined in the
Output Definitions characteristic.

#### Set Output Bounds Characteristic
Writing to this characteristic sets the output Bounds Values of one or more device output channels.
The first value of the characteristic the new Output Bounds Value to set on one or more output channels.
The second value is a bitfield mapping of which channels to set the new Value on,
with the bitfield mapping defined by the Output Definitions characteristic.
Note that because Bounds Values are tied to output type,
specifying outputs of two different types in the bitfield will result in an error response.

##### Example Set Output Bounds
Building upon the earlier example Output Definitions characteristic for the plug,
to set the maximum power level of the TENS output to half of the device's maximum output,
write `0x07FF 0x0001` to the Set Output Bounds characteristic (short-hand-UUID `0x6913`).

To set the LED to purple (assuming the current pattern doesn't modify the LED),
write `0xA234 0x0004` to the Set Output Bounds 

If the power level of the plug vibration is also set to half of maximum, reading from the 
Output Bounds characteristic would yield `0x07FFF 0x0034 0xA234`.
