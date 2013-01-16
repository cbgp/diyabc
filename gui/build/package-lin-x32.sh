#!/bin/bash

# script à exécuter sur une machine linux 64bits qui comporte PyQt4, PyQwt5, g++

packageDir="./lastDiyabc"
diyabc_git="../../"

cd $diyabc_git

git checkout .
git pull --rebase
make clean
make 

cp ./src-JMC-C++/general ./gui/data/executables/diyabc-comput-linux-x32

cd ./gui/build/
rm -rf $packageDir

./linux_generation.sh ./pyinstaller_1743/pyinstaller.py $packageDir  ../src/diyabc.py
cd lastDiyabc
zip -r diyabc_linux32.zip *
