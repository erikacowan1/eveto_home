#!/bin/bash -v 
./eveto --gps-start-time 900000000 --gps-end-time 900000010 --cbc-trigger-snr-threshold 7.0 --detector H1 --output-directory=/tmp --cbc-trigger-database sqlite://l1-lock-clustered.sqlite --x_low 3 --x_high 7 --n_bins 50
