#!/bin/bash
launcher=`readlink -f $0`
diyabcRoot=`dirname $launcher`
cd $diyabcRoot/gui/src
python diyabc.py
