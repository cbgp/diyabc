#!/bin/bash

function printUsage(){
echo "usage : 
create_debian_repo.sh  path_to_repo 

"
}

if [ $# -eq 0 ] ; then
    printUsage
    exit
fi

REPOPATH=$1

mkdir -p $REPOPATH/debian/conf $REPOPATH/debian/incoming

echo "Origin: CBGP
Label: INRA
Suite: stable
Codename: squeeze
Version: 1.0
Architectures: i386 amd64
Components: main
Description: Repository of CBGP softwares
" > $REPOPATH/debian/conf/distributions

