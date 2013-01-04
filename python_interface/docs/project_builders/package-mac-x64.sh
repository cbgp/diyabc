#!/bin/bash

packageDir="/Users/raph/Documents/julien/lastDiyabc"
diyabc_git="/Users/raph/Documents/julien/diyabc"

rm -rf $packageDir
cd $diyabc_git

git checkout .
git pull --rebase 
make clean
CCVERSION=-fsf-4.7 make 
cp $diyabc_git/src-JMC-C++/general $diyabc_git/python_interface/docs/executables/diyabc-comput-mac-x64

cd $diyabc_git/python_interface/docs/project_builders/

./mac_generation.sh ./pyinstaller_1355/pyinstaller.py ../icons/coccicon.icns $packageDir  ../../diyabc.py



