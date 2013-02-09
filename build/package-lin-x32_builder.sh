#!/bin/bash

# script à exécuter sur une machine linux 64bits qui comporte PyQt4, PyQwt5, g++

packageDir="./lastDiyabc"
diyabc_git="../"
HTMLGENPATH="../doc/gen_html_doc.sh"

cd $diyabc_git

make clean
MYFLAGS="-static" make 

strip ./src-JMC-C++/general
cp ./src-JMC-C++/general ./gui/data/bin/diyabc-comput-linux-i386

# html generation
cd ./build
cd `dirname $HTMLGENPATH`
./gen_html_doc.sh

cd ../build/
rm -rf $packageDir

./linux_generation.sh -p ./pyinstaller-2.0/pyinstaller.py -o $packageDir  -m ../gui/src/diyabc.py -a 32
cd lastDiyabc
zip -r "`ls | grep diyabc*32`.zip" diyabc*32
rm -r diyabc*32

