GITDIR=../../../
PACKAGESRCDIR=~/rpmbuild/SOURCES
PACKAGEDIR=~/rpmbuild
VERSION="`head -n 1 ../../version.txt`"
BUILDDATE=`LANG=en_EN.utf8 date +%d-%m-%Y`


rm -rf $PACKAGEDIR
cp -r ./rpmbuild/ $PACKAGEDIR
mkdir -p $PACKAGEDIR/BUILD  $PACKAGEDIR/BUILDROOT  $PACKAGEDIR/RPMS  $PACKAGEDIR/SOURCES/  $PACKAGEDIR/SPECS  $PACKAGEDIR/SRPMS  $PACKAGEDIR/tmp

cp ./rpmbuild/SPECS/diyabc.spec  $PACKAGEDIR/SPECS/diyabc.spec
# spec file edition
sed -i "s/Version:   X/Version:   $VERSION/" $PACKAGEDIR/SPECS/diyabc.spec
sed -i "s/>= X/>= $VERSION/g" $PACKAGEDIR/SPECS/diyabc.spec

rpmbuild --target noarch  -v -bb $PACKAGEDIR/SPECS/diyabc.spec
cp $PACKAGEDIR/RPMS/*/* ./
