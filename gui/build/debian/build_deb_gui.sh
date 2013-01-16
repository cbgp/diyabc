#!/bin/bash

function printUsage(){
echo "usage : 
build_deb.sh  path_to_main_source_dir

"
}

if [ $# -eq 0 ] ; then
    printUsage
    exit
fi

SOURCEDIR=$1

VERSIONFILE="$SOURCEDIR/../version.txt"
VERSION="`head -n 1 $VERSIONFILE`"
BUILDDATE=`LANG=en_EN.utf8 date +%d-%m-%Y`

PACKAGEDIR=diyabc-gui\_$VERSION\_all

# template copy 
echo $PACKAGEDIR
cp -rp diyabc-gui-pkg-template/ $PACKAGEDIR

# control file edition
sed -i "s/Version: X/Version: $VERSION/" $PACKAGEDIR/DEBIAN/control
sed -i "s/>=X/>=$VERSION/g" $PACKAGEDIR/DEBIAN/control

mkdir -p $PACKAGEDIR/usr/share/icons/diyabc/
mkdir -p $PACKAGEDIR/usr/share/images/diyabc/
mkdir -p $PACKAGEDIR/usr/local/src/diyabc/

# for git errors
mkdir -p $PACKAGEDIR/usr/bin 
chmod 755 diyabc-gui-pkg-template/DEBIAN/*

# file copy
cp -rp $SOURCEDIR/*.py $SOURCEDIR/clean.sh $SOURCEDIR/analysis $SOURCEDIR/uis $SOURCEDIR/../utils $SOURCEDIR/summaryStatistics $SOURCEDIR/mutationModel $SOURCEDIR/historicalModel $SOURCEDIR/geneticData $PACKAGEDIR/usr/local/src/diyabc/
# version modification
sed -i "s/VERSION='development version'/VERSION='$VERSION'/" $PACKAGEDIR/usr/local/src/diyabc/variables.py
sed -i "s/01\/01\/1970/$BUILDDATE/" $PACKAGEDIR/usr/local/src/diyabc/variables.py
sed -i "s/VVERSION/$VERSION/" $PACKAGEDIR/usr/share/menu/diyabc
sed -i "s/VVERSION/$VERSION/" $PACKAGEDIR/usr/share/applications/diyabc.desktop

sed -i "s/ICON/\/usr\/share\/icons\/diyabc\/coccicon.png/" $PACKAGEDIR/usr/share/applications/diyabc.desktop

cp -rp $SOURCEDIR/../data/images/* $PACKAGEDIR/usr/share/images/diyabc/
cp -r $SOURCEDIR/../data/icons/* $PACKAGEDIR/usr/share/icons/diyabc/

# generation of the launch script placed in /usr/bin
echo "#!/bin/bash
cd /usr/local/src/diyabc/
python /usr/local/src/diyabc/diyabc.py \$@" > $PACKAGEDIR/usr/bin/diyabc-gui
# change owner:group to root
#echo "chown -R 0:0 /usr/local/bin/diyabc /usr/local/src/diyabc /usr/share/applications/diyabc.desktop /usr/share/menu/diyabc" >> $PACKAGEDIR/DEBIAN/postinst
chmod +x $PACKAGEDIR/usr/bin/diyabc-gui

cp -r $PACKAGEDIR /tmp
chown -R root:root /tmp/$PACKAGEDIR

dpkg-deb -b /tmp/$PACKAGEDIR
mv /tmp/$PACKAGEDIR.deb ./
if $clean; then
    rm -rf /tmp/$PACKAGEDIR $PACKAGEDIR
else
    echo "The package directory was not removed"
fi

