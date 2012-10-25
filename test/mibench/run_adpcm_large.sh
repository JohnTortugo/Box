#!/bin/bash

source definitions.sh

$BOX_HOME/box $ADPCM_HOME/bin/rawcaudio < $ADPCM_HOME/data/large.pcm   > $ADPCM_HOME/output_large.adpcm
$BOX_HOME/box $ADPCM_HOME/bin/rawdaudio < $ADPCM_HOME/data/large.adpcm > $ADPCM_HOME/output_large.pcm
