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
    fakeroot ./build_deb_bin_64.sh ../../Makefile ../../gui/version.txt
else
    fakeroot ./build_deb_bin_32.sh ../../Makefile ../../gui/version.txt
fi

fakeroot ./build_deb_doc.sh ../../Notice-DIYABC-v2/Notice_DIYABC_principal.tex ../../doc/gen_html_doc.sh ../../gui/version.txt

fakeroot ./build_deb_gui.sh ../../gui/src/

fakeroot ./build_deb_virtual.sh ../../gui/version.txt

rm -r ./lastDiyabc
mkdir lastDiyabc
mv *.deb ./lastDiyabc
