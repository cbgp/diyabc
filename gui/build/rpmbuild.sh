SOURCEDIR=../src
UTILSDIR=../build
PACKAGESRCDIR=~/rpmbuild/SOURCES
PACKAGEDIR=~/rpmbuild
VERSION="`head -n 1 ../version.txt`"
BUILDDATE=`LANG=en_EN.utf8 date +%d-%m-%Y`


rm -rf $PACKAGEDIR
cp -r ./rpmbuild/ $PACKAGEDIR
mkdir -p $PACKAGEDIR/BUILD  $PACKAGEDIR/BUILDROOT  $PACKAGEDIR/RPMS  $PACKAGEDIR/SOURCES/src $PACKAGEDIR/SOURCES/bin  $PACKAGEDIR/SPECS  $PACKAGEDIR/SRPMS  $PACKAGEDIR/tmp
cp -rp $SOURCEDIR/*.py $SOURCEDIR/clean.sh $SOURCEDIR/analysis $SOURCEDIR/uis $UTILSDIR  $SOURCEDIR/summaryStatistics $SOURCEDIR/mutationModel $SOURCEDIR/historicalModel $SOURCEDIR/geneticData $PACKAGESRCDIR/src
cp -r debian/diyabc-gui-pkg-template/usr/share/menu/ $PACKAGESRCDIR/
cp -r debian/diyabc-gui-pkg-template/usr/share/applications/ $PACKAGESRCDIR/

cp -rp $SOURCEDIR/../data/images $SOURCEDIR/../data/icons   $PACKAGESRCDIR/
cp ./rpmbuild/SPECS/diyabc.spec  $PACKAGEDIR/SPECS/diyabc-bin.spec
# spec file edition
sed -i "s/Version:   X/Version:   $VERSION/" $PACKAGEDIR/SPECS/diyabc-gui.spec
sed -i "s/>= X/>= $VERSION/g" $PACKAGEDIR/SPECS/diyabc-gui.spec

sed -i "s/VERSION='development version'/VERSION='$VERSION'/" $PACKAGEDIR/SOURCES/src/variables.py
sed -i "s/01\/01\/1970/$BUILDDATE/" $PACKAGEDIR/SOURCES/src/variables.py
sed -i "s/VVERSION/$VERSION/" $PACKAGEDIR/SOURCES/menu/diyabc
sed -i "s/VVERSION/$VERSION/" $PACKAGEDIR/SOURCES/applications/diyabc.desktop

sed -i "s/ICON/\/usr\/share\/icons\/diyabc\/coccicon.png/" $PACKAGEDIR/SOURCES/applications/diyabc.desktop

rpmbuild --target i686  -v -bb $PACKAGEDIR/SPECS/diyabc-gui.spec
cp $PACKAGEDIR/RPMS/*/* ./
