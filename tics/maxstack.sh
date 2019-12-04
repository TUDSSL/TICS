#!/bin/bash

find . -name \*.su -print0 | xargs -0 cat | sort -t$'\t' -nrk2 | column -t -s$'\t'
