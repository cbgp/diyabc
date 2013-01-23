#!/bin/bash

function printUsage(){
echo "usage : 
build_deb.sh  path_to_notice.tex  path_to_html_notice_generator.sh  path_to_version.txt

"
}

if [ $# -eq 0 ] ; then
    printUsage
    exit
fi

TEXPATH=$1

HTMLGENPATH=$2

VERSIONFILE=$3
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
cp -rp ../../Notice-DIYABC-v2/Notice_DIYABC_principal.pdf $PACKAGEDIR/usr/share/doc/diyabc/diyabc_notice.pdf

# html generation
cd `dirname $HTMLGENPATH`
./gen_html_doc.sh
cd -
cp -rp ../../Notice-DIYABC-v2/Notice_DIYABC_principal $PACKAGEDIR/usr/share/doc/diyabc/html


# package generation

# control file edition
sed -i "s/Version: X/Version: $VERSION/" $PACKAGEDIR/DEBIAN/control

cp -r $PACKAGEDIR /tmp
chown -R root:root /tmp/$PACKAGEDIR

dpkg-deb -b /tmp/$PACKAGEDIR
mv /tmp/$PACKAGEDIR.deb ./
rm -rf /tmp/$PACKAGEDIR $PACKAGEDIR
