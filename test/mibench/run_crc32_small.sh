#!/bin/bash

source definitions.sh

$BOX_HOME/box $CRC32_HOME/crc $ADPCM_HOME/data/large.pcm > $CRC32_HOME/output_small.txt
