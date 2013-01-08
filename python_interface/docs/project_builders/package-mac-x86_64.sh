#!/bin/bash

# script à exécuter sur la machine de raph qui donne du x86_64
# probleme : pyinstaller ne marche pas en 64 bits

packageDir="./lastDiyabc"
diyabc_git="../../../"

rm -rf $packageDir
cd $diyabc_git

git checkout .
git pull --rebase 
make clean
CCVERSION="-fsf-4.7" make 

cp ./src-JMC-C++/general ./python_interface/docs/executables/diyabc-comput-mac-i386
cp ./src-JMC-C++/general ./python_interface/docs/executables/diyabc-comput-mac-x64

cd ./python_interface/docs/project_builders/

./mac_generation.sh ./pyinstaller_1355/pyinstaller.py ../icons/coccicon.icns $packageDir  ../../diyabc.py



