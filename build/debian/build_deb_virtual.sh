#!/bin/bash

function printUsage(){
echo "usage : 
build_deb_bin.sh  path_to_version_file.txt

"
}

if [ $# -eq 0 ] ; then
    printUsage
    exit
fi

VERSIONFILE=$1
VERSION="`head -n 1 $VERSIONFILE`"

PACKAGEDIR=diyabc\_$VERSION\_all
rm -rf $PACKAGEDIR

# template copy 
echo $PACKAGEDIR
cp -rp diyabc-virtual-pkg-template/ $PACKAGEDIR/

# package generation

# control file edition
sed -i "s/Version: X/Version: $VERSION/" $PACKAGEDIR/DEBIAN/control
sed -i "s/>=X/>=$VERSION/g" $PACKAGEDIR/DEBIAN/control
#cat $PACKAGEDIR/DEBIAN/control
git log | head -n 100 > $PACKAGEDIR/usr/share/doc/diyabc/changelog
gzip -9 $PACKAGEDIR/usr/share/doc/diyabc/changelog
chmod 644 $PACKAGEDIR/usr/share/doc/diyabc/changelog.gz

cp -r $PACKAGEDIR /tmp
chown -R root:root /tmp/$PACKAGEDIR

dpkg-deb -b /tmp/$PACKAGEDIR
mv /tmp/$PACKAGEDIR.deb ./
rm -rf /tmp/$PACKAGEDIR $PACKAGEDIR
