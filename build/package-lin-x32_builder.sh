#!/bin/bash

# script à exécuter sur une machine linux 64bits qui comporte PyQt4, PyQwt5, g++

packageDir="./lastDiyabc"
diyabc_git="../"

cd $diyabc_git

make clean
make 

strip ./src-JMC-C++/general
cp ./src-JMC-C++/general ./gui/data/bin/diyabc-comput-linux-i386

cd ./build/
rm -rf $packageDir

./linux_generation.sh ./pyinstaller-2.0/pyinstaller.py $packageDir  ../gui/src/diyabc.py 32
cd lastDiyabc
zip -r "`ls | grep diyabc*32`.zip" diyabc*32
rm -r diyabc*32

