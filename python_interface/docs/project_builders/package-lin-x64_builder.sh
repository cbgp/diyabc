#!/bin/bash

# script à exécuter sur une machine linux 64bits qui comporte PyQt4, PyQwt5, g++

packageDir="./lastDiyabc"
diyabc_git="../../../"

cd $diyabc_git

make clean
make 

cp ./src-JMC-C++/general ./python_interface/docs/executables/diyabc-comput-linux-x64

cd ./python_interface/docs/project_builders/
rm -rf $packageDir

./linux_generation.sh ./pyinstaller_1743/pyinstaller.py $packageDir  ../../diyabc.py
cd lastDiyabc
zip -r diyabc_linux64.zip *

