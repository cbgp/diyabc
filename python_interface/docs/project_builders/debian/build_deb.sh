#!/bin/bash

VERSION=$1
PACKAGEDIR=diyabc-$VERSION
SOURCEDIR=$2

# template copy 
cp -r diyabc-interface-pkg-template/ $PACKAGEDIR

# control file edition
sed -i "s/Version: X/Version: $VERSION/" $PACKAGEDIR/DEBIAN/control
sed -i "s/Package: diyabc/Package: diyabc-$VERSION/" $PACKAGEDIR/DEBIAN/control
sed -i "s/diyabc-X/diyabc-$VERSION/" $PACKAGEDIR/DEBIAN/prerm

# copy of all the usefull file into usr/local/src
mkdir $PACKAGEDIR/usr/local/src/diyabc-$VERSION/
cp -r $SOURCEDIR/*.py $SOURCEDIR/clean.sh $SOURCEDIR/analysis $SOURCEDIR/uis $SOURCEDIR/utils $SOURCEDIR/summaryStatistics $SOURCEDIR/mutationModel $PACKAGEDIR/usr/local/src/diyabc-$VERSION/
mkdir $PACKAGEDIR/usr/local/src/diyabc-$VERSION/docs
cp -r $SOURCEDIR/docs/accueil_pictures $PACKAGEDIR/usr/local/src/diyabc-$VERSION/docs/
cp $SOURCEDIR/docs/*.png $PACKAGEDIR/usr/local/src/diyabc-$VERSION/docs/
echo "#!/bin/bash
python /usr/local/src/diyabc-$VERSION/diyabc.py" > $PACKAGEDIR/usr/local/bin/diyabc-$VERSION
chmod +x $PACKAGEDIR/usr/local/bin/diyabc-$VERSION

dpkg-deb -b $PACKAGEDIR
