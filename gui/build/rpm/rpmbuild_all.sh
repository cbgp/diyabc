#!/bin/bash

function printUsage(){
echo "usage : 
build_all.sh  architecure(64 or 32)

"
}

if [ $# -eq 0 ] ; then
    printUsage
    exit
fi

ARCH=$1

rm *.rpm
./rpmbuild-bin.sh $ARCH

./rpmbuild-doc.sh

./rpmbuild-gui.sh 

./rpmbuild-virtual.sh

rm -r ./lastDiyabc
mkdir lastDiyabc
mv *.rpm ./lastDiyabc
