#!/bin/bash

function printUsage(){
echo "usage : 
build_all.sh  architecure(amd64 or i386)

"
}

if [ $# -eq 0 ] ; then
    printUsage
    exit
fi

ARCH=$1

rm *.deb
if [ $ARCH == "amd64" ]; then
    fakeroot ./build_deb_bin_64.sh ../../../Makefile ../../version.txt
else
    fakeroot ./build_deb_bin_32.sh ../../../Makefile ../../version.txt
fi

fakeroot ./build_deb_doc.sh ../../../Notice-DIYABC-v2/Notice_DIYABC_principal.tex ../../../doc/gen_html_doc.sh ../../version.txt

fakeroot ./build_deb_gui.sh ../../src/

fakeroot ./build_deb_virtual.sh ../../version.txt

rm -r ./lastDiyabc
mkdir lastDiyabc
mv *.deb ./lastDiyabc
