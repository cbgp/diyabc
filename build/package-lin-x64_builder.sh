#!/bin/bash

# script à exécuter sur une machine linux 64bits qui comporte PyQt4, PyQwt5, g++

packageDir="./lastDiyabc"
diyabc_git="../"

cd $diyabc_git

make clean
make 

cp ./src-JMC-C++/general ./gui/data/bin/diyabc-comput-linux-x64

cd ./build/
rm -rf $packageDir

./linux_generation.sh ./pyinstaller-2.0/pyinstaller.py $packageDir  ../gui/src/diyabc.py 64
cd lastDiyabc
#zip -r diyabc_linux64.zip *
zip -r "`ls | grep diyabc*64`.zip" diyabc*64
rm -r diyabc*64
