#!/bin/bash

function printUsage(){
echo "usage : 
rpmbuild-bin.sh  architecture(32 or 64)

"
}

if [ $# -eq 0 ] ; then
    printUsage
    exit
fi

ARCH=$1
GITDIR=../../../
PACKAGESRCDIR=~/rpmbuild/SOURCES
PACKAGEDIR=~/rpmbuild
VERSION="`head -n 1 ../../version.txt`"
BUILDDATE=`LANG=en_EN.utf8 date +%d-%m-%Y`


rm -rf $PACKAGEDIR
cp -r ./rpmbuild/ $PACKAGEDIR
mkdir -p $PACKAGEDIR/BUILD  $PACKAGEDIR/BUILDROOT  $PACKAGEDIR/RPMS  $PACKAGEDIR/SOURCES/  $PACKAGEDIR/SPECS  $PACKAGEDIR/SRPMS  $PACKAGEDIR/tmp

MAKEPATH=$GITDIR/Makefile

# bin compilation
cd `dirname $MAKEPATH`
make clean
make
cd -
mkdir -p $PACKAGESRCDIR/usr/bin/
cp -rp $GITDIR/src-JMC-C++/general $PACKAGESRCDIR/usr/bin/diyabc

cp ./rpmbuild/SPECS/diyabc-bin.spec  $PACKAGEDIR/SPECS/diyabc-bin.spec
# spec file edition
sed -i "s/Version:   X/Version:   $VERSION/" $PACKAGEDIR/SPECS/diyabc-bin.spec
if [ $ARCH == "32" ]; then
    sed -i "s/noarch/i386/" $PACKAGEDIR/SPECS/diyabc-bin.spec
    rpmbuild --target i386  -v -bb $PACKAGEDIR/SPECS/diyabc-bin.spec
else
    sed -i "s/noarch/x86_64/" $PACKAGEDIR/SPECS/diyabc-bin.spec
    rpmbuild --target x86_64  -v -bb $PACKAGEDIR/SPECS/diyabc-bin.spec
fi

cp $PACKAGEDIR/RPMS/*/* ./
