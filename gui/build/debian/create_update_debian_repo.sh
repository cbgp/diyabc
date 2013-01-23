#!/bin/bash

function printUsage(){
echo "usage : 
create_update_debian_repo.sh  path_to_repo  path_to_packages

"
}

if [ $# -eq 0 ] ; then
    printUsage
    exit
fi

REPOPATH=$1
PKGPATH=$2

mkdir -p $REPOPATH/conf $REPOPATH/incoming

echo "Origin: CBGP
Label: INRA
Suite: stable
Codename: squeeze
Version: 1.0
Architectures: i386 amd64
Components: main
Description: Repository of CBGP softwares
" > $REPOPATH/conf/distributions


cd $PKGPATH
for deb in *.deb; do
    reprepro -Vb $REPOPATH includedeb stable $deb
done


