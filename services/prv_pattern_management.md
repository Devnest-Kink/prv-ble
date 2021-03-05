# PRV Pattern Management Service
This service allows an authenticated (bonded) central device to
add and delete patterns stored on the peripheral device.

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

Writing an invalid index will return an error.

#### Add Pattern Characteristic
This Characteristic is used to add patterns to the device.
The first value is an index at which to put the new pattern.
If this index is currently occupied, all patterns at that index and higher will have their indices increased by 1.
A value of `0` appends the pattern at the next unused index.
The remaining bytes define the pattern to add.

Providing an invalid index or invalid pattern will return an error.

#### Delete Pattern Characteristic
Writing a valid pattern index to this Characteristic will cause the pattern at that index to be deleted.
All remaining patterns with a higher index will have their indices reduced by 1.

Writing an invalid index will return an error.

#### Delete ALL Patterns Characteristic
Writing `0x6AAC` to this characteristic will delete all of the patterns stored on the device.
Writing any other value will return an error.
