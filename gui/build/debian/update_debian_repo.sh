#!/bin/bash

function printUsage(){
echo "usage : 
update_debian_repo.sh  path_to_repo  path_to_package_dir

"
}

if [ $# -eq 0 ] ; then
    printUsage
    exit
fi

REPOPATH=$1
PKGPATH=$2

reprepro -Vb $REPOPATH includedeb stable $PKGPATH/*.deb


