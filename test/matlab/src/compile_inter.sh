#!/bin/bash
BIIPS_ROOT=`pwd`/../../..
MEX=mex
MEXEXT=`mexext`
${MEX} -I${BIIPS_ROOT}/core/include -I${BIIPS_ROOT}/matbiips/src/ inter_boost.cpp
cp inter_boost.${MEXEXT} ..
