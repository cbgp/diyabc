#!/bin/bash

# Ce script est à exécuter sur la machine virtuelle windows 7

# default values
packageDir="./lastDiyabc"
diyabc_git="../"
MinGw_bindir="/c/MinGw/bin/"

# custom values
if [[ $# > 0 ]]; then
    packageDir="$1"
fi
if [[ $# > 1 ]]; then
    diyabc_git="$2"
fi
if [[ $# > 2 ]]; then
    MinGw_bindir="$3"
fi

rm -rf $packageDir
cd $diyabc_git

git checkout .
git pull --rebase 
$MinGw_bindir/mingw32-make.exe clean
MYFLAGS="-static" $MinGw_bindir/mingw32-make.exe
cp $MinGw_bindir/*.dll ./gui/data/bin/
cp ./src-JMC-C++/general.exe ./gui/data/bin/diyabc-comput-win-i386
#cp ./src-JMC-C++/general.exe ./gui/data/bin/diyabc-comput-win-x64

cd ./build/
./windows_generation.sh --pyinst ./pyinstaller-2.0/pyinstaller.py -icon ../gui/data/icons/coccicon.ico --output $packageDir --main ../gui/src/diyabc.py --arch 32
