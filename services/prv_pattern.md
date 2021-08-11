# PRV Pattern Service
This service allows an authenticated (bonded) central device to play back,
add, and delete patterns stored on the peripheral device.

## Pattern Indexing
Pattern numbers are 1-indexed rather than 0-indexed.
All indices are sequential; gaps between pattern indices are not allowed.

## Channel linking
Device output channels can be logically linked to enable the selected pattern to output on multiple channels.
Linked channels all play the same pattern and have the same pattern input value.
Channel links are indicated by specifying multiple bits in the channel selector bitfield (the first two bytes)
of the Playback Status & Playback Control Characteristics.


When channels are initially linked, pattern playback and input values are set to those of the lowest-indexed channel.
To link multiple channels,
specify them via the bitfield selector while setting any value in the Playback Control Characteristic,
To facilitate behavior expected by users, select all linked channels and
simply select set the Pattern Input value to that of the lowest-indexed linked channel,
pattern playback of linked higher-indexed channels will be synchronized to lowest-indexed channel in link.


When unlinking channels, select the higher-indexed channel(s) when making the change;
the lowest-indexed channel of a former link (and any channels still linked to it) are not affected by the change.
Because the firmware needs to know both sets of channels being unlinked,
the Playback Control Characteristic includes a field to indicate the non-changing half of a former link.


For example, in order to link channels 1 & 2 (while writing a value of 0x0045 to the pattern input),
write `0x0003 0x00 0x00 0x0045 0x0000` to the Playback Control Characteristic.
To then unlink those channels,
indicate channel 2 has changed and indicate that it used to be linked to channel 1 by writing
`0x0002 0x00 0x00 0x0045 0x0001` to the Playback Control Characteristic.
To link channels 1-4, use `0x000F` as the first pair of bytes written to the Control Characteristic.
To unlink channels 1 & 2 from channels 3 & 4, 
indicate channels 3 & 4 have changed by using `0x000C` as the first pair of bytes and `0x0003` as the last pair
when writing to the Control Characteristic.


*Note:* The firmware supports arbitrary linking of channels,
but UIs generally only support linking of channels with sequential indices and may not correctly indicate
non-sequential links.

#### Pattern Service Metadata Characteristic
This Characteristic provides version information about the pattern service and pattern API on the device.
It also provides the number of patterns stored on the device.

#### Pattern Metadata Characteristic
This Characteristic provides the metadata for a specific pattern stored on the device.
The first value of this Characteristic is the index of the pattern corresponding to the provided metadata.
The remaining data provided by this Characteristic is pattern metadata, as described in `pattern_schema.json`.

The pattern for which metadata is provided is selected by the Pattern Metadata Selector Characteristic.

#### Pattern Metadata Selector Characteristic
Writing a valid pattern index to this Characteristic will cause the Pattern Metadata Characteristic
to be updated with metadata for the pattern index written.

Writing an invalid index will result in an error response.

#### Pattern Playback Status Characteristic
This Characteristic reports the state of pattern playback on device output channels.
The Characteristic provides one 'playback report' per channel,
though some information may be redundant when channels are linked.

A 'playback report' consists of eight bytes, organized sequentially into the following fields, all values little-endian:
* Bytes 0 & 1: `Uint16` bitfield indicating which outputs are grouped together and used by a given pattern.
  At least one bit will be set in each report and will correlate to the sequence of reports.
* Byte 2: One of the following playback status values:
  * `0xFF` - Playback stopped, no pattern selected, channel output disabled
  * `0x01` - Pattern playing
  * `0x02` - Pattern paused during playback, associated outputs are inactive
* Byte 3: `Uint8` index of the pattern, if any, being played on the indicated outputs, or 0 if stopped.
* Bytes 4 & 5: `Uint16` value provided by the user as input to the pattern, usually to adjust pattern behavior.
* Bytes 6 & 7: `Uint16` value set by the pattern output.

#### Pattern Playback Control Characteristic
Writes to this Characteristic can change the state of pattern playback on the device.
This Characteristic uses the same 'playback report' data format as Playback Status, but with minor adjustments:
* Bytes 0 & 1: This `uint16` bitfield indicates which outputs the provided changes are applied to.
* Byte 2: One of the following opcodes:
  * `0x00` - No change in playback status
  * `0x01` - Play a pattern from the beginning.
    If no pattern is provided as part of this write, restart current pattern from the beginning.
    This also sets Pattern Input to zero.
  * `0x02` - Pause current playback. Progress and Pattern Input value are preserved.
  * `0x03` - Resume from paused state.
  * `0xFF` - Stop playback, deselect pattern. Associated device outputs are disabled.
* Byte 3: `Uint8` index of the pattern, if any, to be played on the indicated outputs, or 0 for no change.
* Bytes 4 & 5: `Uint16` value provided by the user as input to the pattern, usually to adjust pattern behavior.
  * Note: When playing a different pattern than the current selection, input value is reset to zero.
    Setting a new input value requires an additional write to this characteristic.
* Bytes 6 & 7: `Uint16` Used when unlinking to specify the channels no longer linked to indicated channels.

Invalid or illogical values or value combinations will result in an error response.

#### Add Pattern Characteristic
This Characteristic is used to add patterns to the device.
The first value is an index at which to put the new pattern.
If this index is currently occupied, all patterns at that index and higher will have their indices increased by 1.
A value of `0` appends the pattern at the next unused index.
The remaining bytes define the pattern to add.

Providing an invalid index or invalid pattern will result in an error response.

#### Delete Pattern Characteristic
Writing a valid pattern index to this Characteristic will cause the pattern at that index to be deleted.
All remaining patterns with a higher index will have their indices reduced by 1.

Writing an invalid index will result in an error response.

#### Delete ALL Patterns Characteristic
Writing `0x6AAC` to this characteristic will delete all the patterns stored on the device.
Writing any other value will result in an error response.
