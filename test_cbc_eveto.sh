#!/bin/bash -v

./cbc_eveto \
    --gps-start-time 9000000000 \
    --gps-end-time   9000000000 \
    --detector L1 \
    --safe-channel-file safe_channel_list.txt \
    --cbc-trigger-database /path/to/cbc \
    --cbc-snr-threshold 5.0 \
    --omicron-trigger-path /Users/dbrown/Documents/Research/detchar/triggers \
    --omicron-snr-threshold 8.0 \
    --omicron-cluster-window 1.0 \
    --output-directory . \
    --verbose
