# PRV Pattern Service
This service allows an authenticated (bonded) central device to playback,
add, and delete patterns stored on the peripheral device.

## Pattern Indexing
Pattern numbers are 1-indexed rather than 0-indexed.
All indices are sequential; gaps between pattern indices are not allowed.

#### Pattern Count Characteristic
This value contains number of patterns stored on the device.

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
This Characteristic reports the state of pattern playback on device outputs.
The Characteristic provides one 'playback report' per output,
though some information may be redundant when a pattern is playing on multiple outputs.

A 'playback report' consists of eight bytes, organized sequentially (little-endian) into the following fields:
* Bytes 0 & 1: `uint16` bitfield indicating which outputs are grouped together and used by a given pattern.
  At least one bit will be set in each report and will correlate to the sequence of reports.
* Byte 2: One of the following playback status values
  * `0xFF` - Playback stopped, no pattern selected (next two bytes will be `0x0000`), output disabled
  * `0x01` - Pattern playing; once pattern finishes, pattern with next highest index will play,
    resetting to index 1 after maximum pattern index
  * `0x02` - Pattern paused during playback, associated outputs are inactive
  * `0x09` - Pattern playing, will repeat from beginning after finishing
* Byte 3: Reserved for future use, will be 0.
* Bytes 4 & 5: `uint16` index of the pattern, if any, being played on the indicated outputs, or 0 if stopped
* Byte 6: `uint8` value provided by the user as input to the pattern, usually to adjust pattern behavior
* Byte 7: `uint8` value set by the pattern

#### Pattern Playback Control Characteristic
Writes to this Characteristic can change the state of pattern playback on the device.
This Characteristic uses the same 'playback report' data format as Playback Status, but with minor adjustments:
* Bytes 0 & 1: This `uint16` bitfield indicates which outputs the provided changes are applied to.
* Byte 2: One of the following opcodes:
  * `0x00` - No change in playback status
  * `0x01` - Play a pattern from the beginning, followed sequentially by the other patterns on the device.
    If no pattern is provided as part of this write, restart current pattern from the beginning.
    This also sets Pattern Input to zero.
  * `0x02` - Pause current playback. Progress and Pattern Input value are preserved.
  * `0x03` - Resume from paused state.
  * `0x08` - Toggle repeating of current pattern;
    causes playback status values to alternate between `0x01` and `0x09`, doesn't change pattern state.
  * `0x09` - Repeatedly play selected pattern from the beginning.
  * `0xFF` - Stop playback, deselect pattern. Associated device outputs are disabled.
* Byte 3: Reserved for future use, set to 0.
* Bytes 4 & 5: `uint16` index of the pattern, if any, to be played on the indicated outputs, or 0 for no change
* Byte 6: `uint8` value provided by the user as input to the pattern, usually to adjust pattern behavior.
  * Note: When playing a different pattern than the current selection, input value is reset to zero.
    Setting a new input value requires an additional write to this characteristic.
* Byte 7: Padding, set to 0.
  
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
Writing `0x6AAC` to this characteristic will delete all of the patterns stored on the device.
Writing any other value will result in an error response.
