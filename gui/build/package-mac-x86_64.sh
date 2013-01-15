#!/bin/bash

# script à exécuter sur la machine de raph qui donne du x86_64
# probleme : pyinstaller ne marche pas en 64 bits

packageDir="./lastDiyabc"
diyabc_git="../../"

rm -rf $packageDir
cd $diyabc_git

git checkout .
git pull --rebase 
make clean
CCVERSION="-fsf-4.7" make 

cp ./src-JMC-C++/general ./gui/data/executables/diyabc-comput-mac-i386
cp ./src-JMC-C++/general ./gui/data/executables/diyabc-comput-mac-x64

cd ./gui/build/

./mac_generation.sh ./pyinstaller_1355/pyinstaller.py ../data/icons/coccicon.icns $packageDir  ../src/diyabc.py



