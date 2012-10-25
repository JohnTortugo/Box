#!/bin/bash

source definitions.sh

$BOX_HOME/box $ADPCM_HOME/bin/rawcaudio < $ADPCM_HOME/data/small.pcm > $ADPCM_HOME/output_small.adpcm
$BOX_HOME/box $ADPCM_HOME/bin/rawdaudio < $ADPCM_HOME/data/small.adpcm > $ADPCM_HOME/output_small.pcm
