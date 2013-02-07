#!/bin/bash

function printUsage(){
echo "usage : 
build_deb_bin.sh  (-m | --makefile) path_to_makefile (-v | --version)  path_to_version.txt (-a | --arch) architecture(amd64 | i386)

You have to give all the parameters

"
}

ARGS=$(getopt -o m:v:a: -l "makefile:version:arch:" -n "build_deb_bin.sh" -- "$@");

#Bad arguments
if [ $? -ne 0 ] || [ $# -eq 0 ];
then
      printUsage
    exit
fi
MAKEFLAG=0
VERSIONFLAG=0
ARCHFLAG=0
eval set -- "$ARGS";

while true; do
  case "$1" in
    -a|--arch)
      shift;
      if [ -n "$1" ]; then
        if [ "$1" != "amd64" ] && [ "$1" != "i386" ]; then
            echo "invalid architecture"
            exit
        fi
        ARCH=$1
        ARCHFLAG=1
        shift;
      fi
      ;;
    -m|--makefile)
      shift;
      if [ -n "$1" ]; then
        if [ -f "$1" ]; then
            MAKEPATH=$1
            MAKEFLAG=1
        else
            echo "Makefile does not exist"
            exit
        fi
        shift;
      fi
      ;;
    -v|--version)
      shift;
      if [ -n "$1" ]; then
        if [ -f "$1" ]; then
            VERSIONFILE=$1
            VERSIONFLAG=1
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

if [ $ARCHFLAG == 0 ] && [ $MAKEFLAG == 0 ] && [ $VERSIONFLAG == 0 ]; then
    printUsage
    exit
fi

VERSION="`head -n 1 $VERSIONFILE`"

PACKAGEDIR=diyabc-bin\_$VERSION\_$ARCH
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
strip ../../src-JMC-C++/general
cp -rp ../../src-JMC-C++/general $PACKAGEDIR/usr/bin/diyabc
chmod 755 $PACKAGEDIR/usr/bin/
chmod 755 $PACKAGEDIR/usr/bin/diyabc  $PACKAGEDIR/usr  $PACKAGEDIR/usr/share  $PACKAGEDIR/usr/share/doc  $PACKAGEDIR/usr/share/doc/diyabc-bin

# package generation

# gestion du man
sed -i "s/BUILDDATE/$BUILDDATE/" $PACKAGEDIR/usr/share/man/man1/diyabc.1
sed -i "s/VersionX/Version $VERSION/" $PACKAGEDIR/usr/share/man/man1/diyabc.1
gzip -9 $PACKAGEDIR/usr/share/man/man1/diyabc.1
chmod 644 $PACKAGEDIR/usr/share/man/man1/diyabc.1.gz
# control file edition
sed -i "s/Version: X/Version: $VERSION/" $PACKAGEDIR/DEBIAN/control
sed -i "s/Architecture: X/Architecture: $ARCH/" $PACKAGEDIR/DEBIAN/control
git log | head -n 100 > $PACKAGEDIR/usr/share/doc/diyabc-bin/changelog
gzip -9 $PACKAGEDIR/usr/share/doc/diyabc-bin/changelog
chmod 644 $PACKAGEDIR/usr/share/doc/diyabc-bin/changelog.gz

cp -r $PACKAGEDIR /tmp
cd /tmp/$PACKAGEDIR
find . -type f ! -regex '.*.hg.*' ! -regex '.*?debian-binary.*' ! -regex '.*?DEBIAN.*' -printf '%P ' | xargs md5sum > DEBIAN/md5sums
cd -
chown -R root:root /tmp/$PACKAGEDIR

dpkg-deb -b /tmp/$PACKAGEDIR
mv /tmp/$PACKAGEDIR.deb ./
rm -rf /tmp/$PACKAGEDIR $PACKAGEDIR
