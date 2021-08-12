#!/usr/bin/env python3

import argparse
import pathlib
import json

defines = []


def define(name, value):
    defines.append(f'#define {f"{name}":<64}"{value}"')


parser = argparse.ArgumentParser('Generate C header files with macros containing BLE UUIDs used by a device.')
parser.add_argument('infile', help='Name of device JSON file to parse.')
parser.add_argument('outfile', help='Name of file containing C macros.')

args = parser.parse_args()

infile = pathlib.Path(args.infile).resolve(True)

with open(infile, 'r') as fp:
    device = json.load(fp)

define("CANONICAL_NAME", device['name'])

for service_file in device['services']:
    with open(pathlib.Path(infile.parent.parent, 'services', service_file).resolve(True), 'r') as fp:
        service = json.load(fp)

    service_name = service['service']['name'].upper().replace(' ', '_')
    define(f'{service_name}_UUID', service['service']['UUID'])
    for characteristic in service['characteristics']:
        characteristic_name = f"{service_name}_{characteristic['name'].upper().replace(' ', '_')}_UUID"
        define(characteristic_name, characteristic['UUID'])

outfile = pathlib.Path(args.outfile)
if not outfile.exists():
    if not outfile.parent.exists():
        outfile.parent.mkdir(parents=True, exist_ok=True)
    outfile.touch(exist_ok=True)

outfile.write_text('\n'.join(defines))
