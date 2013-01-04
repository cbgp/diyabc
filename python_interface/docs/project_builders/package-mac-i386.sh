#!/bin/bash

packageDir="~/lastDiyabc"
diyabc_git="~/adocs/diyabc"

rm -rf $packageDir
cd $diyabc_git

git checkout .
git pull --rebase 
make clean
make || echo compilation error ; exit
cp $diyabc_git/src-JMC-C++/general.exe $diyabc_git/python_interface/docs/executables/diyabc-comput-mac-i386

cd $diyabc_git/python_interface/docs/project_builders/
./windows_generation.sh ./pyinstaller_1355/pyinstaller.py ../icons/coccicon.ico $packageDir ../../diyabc.py
./mac_generation.sh ./pyinstaller_1355/pyinstaller.py ../icons/coccicon.icns $packageDir  ../../diyabc.py



