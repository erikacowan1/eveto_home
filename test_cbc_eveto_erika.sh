#!/bin/bash -v

./cbc_eveto \
    --gps-start-time  969000000  \
    --gps-end-time 969020000 \
    --detector L1 \
    --safe-channel-file safe_channels.txt \
    --cbc-trigger-database /Users/erikacowan1/Documents/eveto_home_COPY/l1-s6-triggers.sqlite \
    --cbc-snr-threshold 5.0 \
    --omicron-trigger-path /Users/erikacowan1/Documents/eveto_home_COPY/L1/omicron/triggers/ \
    --omicron-snr-threshold 8.0 \
    --omicron-cluster-window 1.0 \
    --cwb-trigger-path /home/erika.cowan/data/triggers/ER7/CWB/ \
    --output-directory . \
    --sig-threshold 5.0 \
    --dumb-veto-window 3.0 \
    --max-rounds 3 \
    --verbose
