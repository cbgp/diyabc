#!/bin/bash

# script à exécuter sur une machine linux 64bits qui comporte PyQt4, PyQwt5, g++

packageDir="./lastDiyabc"
diyabc_git="../"
HTMLGENPATH="../doc/gen_html_doc.sh"

cd $diyabc_git

make clean
MYFLAGS="-static" make 

strip ./src-JMC-C++/general
cp ./src-JMC-C++/general ./gui/data/bin/diyabc-comput-linux-x64

# html generation
cd -
cd `dirname $HTMLGENPATH`
./gen_html_doc.sh
cd -

cd ./build/
rm -rf $packageDir

./linux_generation.sh -p ./pyinstaller-2.0/pyinstaller.py -o $packageDir  -m ../gui/src/diyabc.py -a 64
cd lastDiyabc
#zip -r diyabc_linux64.zip *
zip -r "`ls | grep diyabc*64`.zip" diyabc*64
rm -r diyabc*64
