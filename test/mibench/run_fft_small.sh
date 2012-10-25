#!/bin/bash

source definitions.sh

$BOX_HOME/box $FFT_HOME/fft 4 4096 > $FFT_HOME/output_small.txt
$BOX_HOME/box $FFT_HOME/fft 4 8192 -i > $FFT_HOME/output_small.inv.txt
