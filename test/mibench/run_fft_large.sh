#!/bin/bash

source definitions.sh

$BOX_HOME/box $FFT_HOME/fft 8 32768 > $FFT_HOME/output_large.txt
$BOX_HOME/box $FFT_HOME/fft 8 32768 -i > $FFT_HOME/output_large.inv.txt
