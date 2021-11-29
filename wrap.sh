#!/bin/bash
set -eou pipefail
UN=T
#sleep 666
ARGS="passwd $UN"
eval "$ARGS"
exit
sleep 2
eval "$ARGS"
sleep 2
eval "$ARGS"
sleep 2
eval "$ARGS"
sleep 2
eval "$ARGS"


sleep 2
eval "$ARGS"
#eval "sh -c '$ARGS; sleep 3;'"
