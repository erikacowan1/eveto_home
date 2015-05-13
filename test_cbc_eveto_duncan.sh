#!/bin/bash -v

./cbc_eveto \
    --gps-start-time 969000000 \
    --gps-end-time 969020000 \
    --detector L1 \
    --safe-channel-file safe_channel_list.txt \
    --cbc-trigger-database l1-s6-triggers.sqlite \
    --cbc-snr-threshold 5.0 \
    --omicron-trigger-path /Users/dbrown/Documents/Research/detchar/triggers/  \
    --omicron-snr-threshold 8.0 \
    --omicron-cluster-window 1.0 \
    --output-directory . \
    --sig-threshold 5.0 \
    --dumb-veto-window 3.0 \
    --max-rounds 3 \
    --verbose
