#!/bin/bash


# this should be 0 or 1 for either datapoint
DATAPOINT=0

#High Temp
THIGH=300

#Parameters
LINE=2
LASTLINE=2
MODEL=CPintheDark
DATASET=L15_m268_ms6m2200_A-11
INPUT=cpvdmS_${DATASET}_${DATAPOINT}.tsv
OUTPUT=output/cpvdm_MinTraOut_${DATASET}_${DATAPOINT}.tsv
EXECUTABLE=./../build/linux-x86_64-release/bin/MinimaTracer

$EXECUTABLE --model=$MODEL --input=$INPUT --output=$OUTPUT --firstline=$LINE --lastline=$LASTLINE --thigh=$THIGH --logginglevel::complete=true --logginglevel::minimizerdetailed=true