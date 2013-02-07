#!/bin/bash

function printUsage(){
echo "usage : 
build_deb_bin.sh (-v | --version) path_to_version_file.txt

"
}

ARGS=$(getopt -o v: -l "version:" -n "build_deb_virtual.sh" -- "$@");

#Bad arguments
if [ $? -ne 0 ] || [ $# -eq 0 ];
then
      printUsage
    exit
fi
eval set -- "$ARGS";

while true; do
  case "$1" in
    -v|--version)
      shift;
      if [ -n "$1" ]; then
        if [ -f $1 ]; then
            VERSIONFILE=$1
        else
            echo "Version file does not exist"
            exit
        fi
        shift;
      fi
      ;;
    --)
      shift;
      break;
      ;;
  esac
done

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
