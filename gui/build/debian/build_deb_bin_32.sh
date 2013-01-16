#!/bin/bash

function printUsage(){
echo "usage : 
build_deb_bin.sh  path_to_makefile  path_to_version.txt

"
}

if [ $# -eq 0 ] ; then
    printUsage
    exit
fi

MAKEPATH=$1
VERSIONFILE=$2
VERSION="`head -n 1 $VERSIONFILE`"

PACKAGEDIR=diyabc-bin\_$VERSION\_i386
rm -rf $PACKAGEDIR

# template copy 
echo $PACKAGEDIR
cp -rp diyabc-bin-pkg-template/ $PACKAGEDIR/

# bin compilation
cd `dirname $MAKEPATH`
make clean
make
cd -
mkdir -p $PACKAGEDIR/usr/bin/
cp -rp ../../../src-JMC-C++/general $PACKAGEDIR/usr/bin/diyabc

# package generation

# control file edition
sed -i "s/Version: X/Version: $VERSION/" $PACKAGEDIR/DEBIAN/control
sed -i "s/Architecture: X/Architecture: i386/" $PACKAGEDIR/DEBIAN/control

cp -r $PACKAGEDIR /tmp
chown -R root:root /tmp/$PACKAGEDIR

dpkg-deb -b /tmp/$PACKAGEDIR
mv /tmp/$PACKAGEDIR.deb ./
rm -rf /tmp/$PACKAGEDIR $PACKAGEDIR
