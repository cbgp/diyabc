#!/bin/bash

fakeroot ./build_deb_bin.sh -m $1 -v $2 -a i386
mkdir lastDiyabc
mv *i386*deb lastDiyabc
