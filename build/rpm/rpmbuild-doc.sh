GITDIR=../../
PACKAGESRCDIR=~/rpmbuild/SOURCES
PACKAGEDIR=~/rpmbuild
VERSION="`head -n 1 ../../gui/version.txt`"
BUILDDATE=`LANG=en_EN.utf8 date +%d-%m-%Y`


rm -rf $PACKAGEDIR
cp -r ./rpmbuild/ $PACKAGEDIR
mkdir -p $PACKAGEDIR/BUILD  $PACKAGEDIR/BUILDROOT  $PACKAGEDIR/RPMS  $PACKAGEDIR/SOURCES/  $PACKAGEDIR/SPECS  $PACKAGEDIR/SRPMS  $PACKAGEDIR/tmp

TEXPATH=$GITDIR/Notice-DIYABC-v2/Notice_DIYABC_principal.tex

HTMLGENPATH=$GITDIR/doc/gen_html_doc.sh

# pdf compilation
cd `dirname $TEXPATH`
pdflatex `basename $TEXPATH` ; pdflatex `basename $TEXPATH`
cd -
cp -rp ../../Notice-DIYABC-v2/Notice_DIYABC_principal.pdf $PACKAGESRCDIR/diyabc_notice.pdf

# html generation
cd `dirname $HTMLGENPATH`
./gen_html_doc.sh
cd -
cp -rp ../../Notice-DIYABC-v2/Notice_DIYABC_principal2 $PACKAGESRCDIR/html


cp ./rpmbuild/SPECS/diyabc-doc.spec  $PACKAGEDIR/SPECS/diyabc-doc.spec
# spec file edition
sed -i "s/Version:   X/Version:   $VERSION/" $PACKAGEDIR/SPECS/diyabc-doc.spec

rpmbuild --target noarch  -v -bb $PACKAGEDIR/SPECS/diyabc-doc.spec
cp $PACKAGEDIR/RPMS/*/* ./
