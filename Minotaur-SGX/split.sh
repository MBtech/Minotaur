#!/bin/bash

split -n 2 temp
cat xab >> $1
rm xaa xab
rm temp
