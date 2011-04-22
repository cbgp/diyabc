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
# version modification
sed -i "s/VERSION='development version'/VERSION='$VERSION'/" $PACKAGEDIR/usr/local/src/diyabc-$VERSION/diyabc.py
sed -i "s/VERSION/$VERSION/" $PACKAGEDIR/usr/share/menu/diyabc
mv $PACKAGEDIR/usr/share/menu/diyabc $PACKAGEDIR/usr/share/menu/diyabc-$VERSION
mkdir $PACKAGEDIR/usr/local/src/diyabc-$VERSION/docs
cp -r $SOURCEDIR/docs/accueil_pictures $PACKAGEDIR/usr/local/src/diyabc-$VERSION/docs/
cp $SOURCEDIR/docs/*.png $PACKAGEDIR/usr/local/src/diyabc-$VERSION/docs/
echo "#!/bin/bash
cd /usr/local/src/diyabc-$VERSION/
python /usr/local/src/diyabc-$VERSION/diyabc.py" > $PACKAGEDIR/usr/local/bin/diyabc-$VERSION
# change owner:group to root
echo "chown -R 0:0 /usr/local/bin/diyabc-$VERSION /usr/local/src/diyabc-$VERSION/" >> $PACKAGEDIR/DEBIAN/postinst
chmod +x $PACKAGEDIR/usr/local/bin/diyabc-$VERSION

dpkg-deb -b $PACKAGEDIR
