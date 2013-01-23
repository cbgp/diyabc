#!/bin/bash

# script à exécuter sur la machine virtuelle osxqt

packageDir="./lastDiyabc"
diyabc_git="../"

rm -rf $packageDir
cd $diyabc_git

git checkout .
git pull --rebase 
make clean
CCVERSION="" make 

cp ./src-JMC-C++/general ./gui/data/executables/diyabc-comput-mac-i386

cd ./build/

./mac_generation.sh ./pyinstaller_1355/pyinstaller.py ../gui/data/icons/coccicon.icns $packageDir  ../gui/src/diyabc.py



