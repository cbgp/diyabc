#!/bin/bash

function printUsage(){
echo "usage : 
build_deb_doc.sh  (-t | --texfile) path_to_notice.tex (-g | --generator) path_to_html_notice_generator.sh (-v | --version) path_to_version.txt

"
}

ARGS=$(getopt -o t:g:v: -l "texfile:generator:version:" -n "build_deb_doc.sh" -- "$@");

#Bad arguments
if [ $? -ne 0 ] || [ $# -eq 0 ];
then
      printUsage
    exit
fi
GENFLAG=0
TEXFLAG=0
VERSIONFLAG=0
eval set -- "$ARGS";

while true; do
  case "$1" in
    -t|--texfile)
      shift;
      if [ -n "$1" ]; then
        if [ -f "$1" ]; then
            TEXPATH=$1
            TEXFLAG=1
        else
            echo "Texfile does not exist"
            exit
        fi
        shift;
      fi
      ;;
    -g|--generator)
      shift;
      if [ -n "$1" ]; then
        if [ -f "$1" ]; then
            HTMLGENPATH=$1
            GENFLAG=1
        else
            echo "Generator script does not exist"
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

if [ $TEXFLAG == 0 ] && [ $GENFLAG == 0 ] && [ $VERSIONFLAG == 0 ]; then
    printUsage
    exit
fi

VERSION="`head -n 1 $VERSIONFILE`"

PACKAGEDIR=diyabc-doc\_$VERSION\_all
rm -rf $PACKAGEDIR

# template copy 
echo $PACKAGEDIR
cp -rp diyabc-doc-pkg-template/ $PACKAGEDIR/

# pdf compilation
cd `dirname $TEXPATH`
pdflatex `basename $TEXPATH` ; pdflatex `basename $TEXPATH`
cd -
cp -rp ../../Notice-DIYABC-v2/Notice_DIYABC_principal.pdf $PACKAGEDIR/usr/share/doc/diyabc-doc/diyabc_notice.pdf

# html generation
cd `dirname $HTMLGENPATH`
./gen_html_doc.sh
cd -
cp -rp ../../Notice-DIYABC-v2/Notice_DIYABC_principal2 $PACKAGEDIR/usr/share/doc/diyabc-doc/html


# package generation

# control file edition
sed -i "s/Version: X/Version: $VERSION/" $PACKAGEDIR/DEBIAN/control
git log | head -n 100 > $PACKAGEDIR/usr/share/doc/diyabc-doc/changelog
gzip -9 $PACKAGEDIR/usr/share/doc/diyabc-doc/changelog
chmod 644 $PACKAGEDIR/usr/share/doc/diyabc-doc/changelog.gz
chmod 644 $PACKAGEDIR/usr/share/doc/diyabc-doc/diyabc_notice.pdf $PACKAGEDIR/usr/share/doc/diyabc-doc/html/*
chmod 755 $PACKAGEDIR/usr $PACKAGEDIR/usr/share $PACKAGEDIR/usr/share/doc $PACKAGEDIR/usr/share/doc/diyabc-doc

cp -r $PACKAGEDIR /tmp
cd /tmp/$PACKAGEDIR
find . -type f ! -regex '.*.hg.*' ! -regex '.*?debian-binary.*' ! -regex '.*?DEBIAN.*' -printf '%P ' | xargs md5sum > DEBIAN/md5sums
cd -
chown -R root:root /tmp/$PACKAGEDIR

dpkg-deb -b /tmp/$PACKAGEDIR
mv /tmp/$PACKAGEDIR.deb ./
rm -rf /tmp/$PACKAGEDIR $PACKAGEDIR
