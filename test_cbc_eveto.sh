#!/bin/bash -v

./cbc_eveto \
    --gps-start-time 1117378816 \
    --gps-end-time 1118415616 \
    --detector L1 \
    --main-channel cwb \
    --safe-channel-file safe_channels.txt \
    --cbc-trigger-database /home/erika.cowan/data/triggers/ER7/CBC/L1-16SEC_CLUSTERED_CBC_ER7_FULL_DATA.sqlite \
    --cbc-snr-threshold 5.0 \
    --omicron-trigger-path /home/dbrown/projects/students/erika_cowan/omicron/er7/  \
    --omicron-snr-threshold 8.0 \
    --omicron-cluster-window 1.0 \
    --cwb-trigger-path /home/erika.cowan/data/triggers/ER7/CWB/ \
    --cwb-snr-threshold 5.0 \
    --output-directory . \
    --sig-threshold 5.0 \
    --dumb-veto-window 3.0 \
    --max-rounds 3 \
    --verbose
