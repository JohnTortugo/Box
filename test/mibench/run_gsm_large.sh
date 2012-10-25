#!/bin/bash

echo "Box doesnt run this program yet"

source definitions.sh

$BOX_HOME/box $GSM_HOME/bin/toast   -fps -c $GSM_HOME/data/large.au         > $GSM_HOME/output_large.encode.gsm
$BOX_HOME/box $GSM_HOME/bin/untoast -fps -c $GSM_HOME/data/large.au.run.gsm > $GSM_HOME/output_large.decode.run
