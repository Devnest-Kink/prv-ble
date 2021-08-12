# PRV Program Service
This service allows an authenticated (bonded) central device to play back,
add, and delete Programs stored on the peripheral device.

## Program Indexing
Program numbers are 1-indexed rather than 0-indexed.
All indices are sequential; gaps between Program indices are not allowed.

## Channel linking
Device output channels can be logically linked to enable the selected Program to output on multiple channels.
Linked channels all play the same Program and have the same Program input value.
Channel links are indicated by specifying multiple bits in the channel selector bitfield (the first two bytes)
of the Playback Status & Playback Control Characteristics.


When channels are first linked, Program execution and input values are set to those of the lowest-indexed channel.
To link multiple channels,
specify them via the bitfield selector while setting any value in the Program Control Characteristic,
To facilitate behavior expected by users, select all linked channels and
simply select set the Program Input value to that of the lowest-indexed linked channel,
Program execution of linked higher-indexed channels will be synchronized to lowest-indexed channel in link.


When unlinking channels, select the higher-indexed channel(s) to make the change;
the lowest-indexed channel of a former link (and any channels still linked to it) are not affected by the change.
Because the firmware needs to know both sets of channels being unlinked,
the Playback Control Characteristic includes a field to indicate the non-changing half of a former link.


For example, in order to link channels 1 & 2 (while writing a value of 0x0045 to the Program input),
write `0x0003 0x00 0x00 0x0045 0x0000` to the Program Control Characteristic.
To then unlink those channels,
indicate channel 2 has changed and indicate that it used to be linked to channel 1 by writing
`0x0002 0x00 0x00 0x0045 0x0001` to the Program Control Characteristic.
To link channels 1-4, use `0x000F` as the first pair of bytes written to the Program Control Characteristic.
To unlink channels 1 & 2 from channels 3 & 4, 
indicate channels 3 & 4 have changed by using `0x000C` as the first pair of bytes and `0x0003` as the last pair
when writing to the Program Control Characteristic.


*Note:* The firmware supports arbitrary linking of channels,
but UIs generally only support linking of channels with sequential indices and may not correctly indicate
non-sequential links.

#### Program Service Metadata Characteristic
This Characteristic provides version information about the Program Service and Program API on the device.
It also provides the number of Programs stored on the device.

#### Program Metadata Characteristic
This Characteristic provides the metadata for a specific Program stored on the device.
The first value of this Characteristic is the index of the Program corresponding to the provided metadata.
The remaining data provided by this Characteristic is Program metadata, as described in `program_schema.json`.

The Program for which metadata is provided is selected by the Program Metadata Selector Characteristic.

#### Program Metadata Selector Characteristic
Writing a valid Program index to this Characteristic will cause the Program Metadata Characteristic
to be updated with metadata for the Program index written.

Writing an invalid index will result in an error response.

#### Program Status Characteristic
This Characteristic reports the state of Program playback on device output channels.
The Characteristic provides one 'program report' per channel;
linked channels will have identical information.

A 'program report' consists of eight bytes, organized sequentially into the following fields, all values little-endian:
* Bytes 0 & 1: `Uint16` bitfield indicating which outputs are linked together and used by a given Program.
  At least one bit will be set in each report and will correlate to the sequence of reports.
* Byte 2: One of the following status values:
  * `0xFF` - Program execution stopped, no Program selected, channel output disabled
  * `0x01` - Program running
  * `0x02` - Program paused during execution, associated outputs are inactive
* Byte 3: `Uint8` index of the Program, if any, being played on the indicated outputs, or 0 if stopped.
* Bytes 4 & 5: `Uint16` value provided by the user as input to the Program, usually to adjust Program behavior.
* Bytes 6 & 7: `Uint16` value set by the Program output.

#### Program Control Characteristic
Writes to this Characteristic can change the state of Program execution on the device.
This Characteristic uses mostly the same 'program report' data format as Program Status, with minor adjustments:
* Bytes 0 & 1: This `uint16` bitfield indicates which outputs the provided changes are applied to.
* Byte 2: One of the following opcodes:
  * `0x00` - No change in execution status
  * `0x01` - Start a Program from the beginning.
    If no Program is provided as part of this write, restart current Program from the beginning.
    This also sets Program Input to zero.
  * `0x02` - Pause current execution. Program progress and input value are preserved.
  * `0x03` - Resume from paused state.
  * `0xFF` - Stop execution, deselect Program. Associated device outputs are disabled.
* Byte 3: `Uint8` index of the Program, if any, to be run on the indicated outputs, or 0 for no change.
* Bytes 4 & 5: `Uint16` value provided by the user as input to the Program, usually to adjust Program behavior.
  * Note: When running a different Program than the current selection, input value is reset to zero.
    Setting a new input value requires an additional write to this characteristic.
* Bytes 6 & 7: `Uint16` Used when unlinking to specify the channels no longer linked to indicated channels,
    ignored otherwise.

Invalid or illogical values or value combinations will result in an error response.

#### Add Program Characteristic
This Characteristic is used to add Programs to the device.
The first value is an index at which to put the new Program.
If this index is currently occupied, all Programs at that index and higher will have their indices increased by 1.
A value of `0` appends the Program at the next unused index.
The remaining bytes define the Program to add.

Providing an invalid index or invalid Program will result in an error response.

##### Note
This Characteristic will need to change some to accommodate arbitrary-length patterns;
exact algorithm is currently undetermined.

#### Delete Program Characteristic
Writing a valid Program index to this Characteristic will cause the Program at that index to be deleted.
All remaining Programs with a higher index will have their indices reduced by 1.

Writing an invalid index will result in an error response.

#### Delete ALL Programs Characteristic
Writing `0x6AAC` to this characteristic will delete all the Programs stored on the device.
Writing any other value will result in an error response.
