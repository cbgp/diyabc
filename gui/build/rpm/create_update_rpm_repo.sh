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

mkdir -p $REPOPATH/i386 $REPOPATH/i686 $REPOPATH/noarch $REPOPATH/repodata $REPOPATH/SRPM $REPOPATH/x86_64

cd $PKGPATH
for rpm in *i386*.rpm; do
    cp $rpm $REPOPATH/i386/
done
for rpm in *x86_64*.rpm; do
    cp $rpm $REPOPATH/x86_64/
done
for rpm in *noarch*.rpm; do
    cp $rpm $REPOPATH/noarch/
done

createrepo -d $REPOPATH
