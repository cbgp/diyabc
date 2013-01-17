#!/bin/bash

./build_deb_bin_32.sh $1 $2
mkdir lastDiyabc
mv *i386*deb lastDiyabc
