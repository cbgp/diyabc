#!/bin/bash

packageDir="/Users/user/lastDiyabc"
diyabc_git="/Users/user/adocs/diyabc"

rm -rf $packageDir
cd $diyabc_git

git checkout .
git pull --rebase 
make clean
make 
cp $diyabc_git/src-JMC-C++/general $diyabc_git/python_interface/docs/executables/diyabc-comput-mac-i386

cd $diyabc_git/python_interface/docs/project_builders/

./mac_generation.sh ./pyinstaller_1355/pyinstaller.py ../icons/coccicon.icns $packageDir  ../../diyabc.py



