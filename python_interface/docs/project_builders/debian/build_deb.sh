#!/bin/bash

function printUsage(){
echo "usage : 
build_deb.sh  directory_of_main_source  main_flag

if main_flag is equal to 'MAIN', a main package will be generated, otherwise a packet with the name of the version will be generated.
"
}

if [ $# -eq 0 ] ; then
    printUsage
    exit
fi

# si MAIN vaut "MAIN", on doit générer un paquet qui n'a pas de numero de version
MAIN=$2
SOURCEDIR=$1

VERSIONFILE="$SOURCEDIR/version.txt"
VERSION="`head -n 1 $VERSIONFILE`"
BUILDDATE=`LANG=en_EN.utf8 date +%d-%b-%Y`

if [ $MAIN == "MAIN" ]; then
    PACKAGEDIR=diyabc\_$VERSION\_all
else
    PACKAGEDIR=diyabc-$VERSION\_$VERSION\_all
fi

# template copy 
cp -r diyabc-interface-pkg-template/ $PACKAGEDIR

# control file edition
sed -i "s/Version: X/Version: $VERSION/" $PACKAGEDIR/DEBIAN/control
if [ $MAIN != "MAIN" ]; then
    sed -i "s/Package: diyabc/Package: diyabc-$VERSION/" $PACKAGEDIR/DEBIAN/control
    sed -i "s/diyabc/diyabc-$VERSION/" $PACKAGEDIR/DEBIAN/prerm
fi


if [ $MAIN == "MAIN" ]; then
    PACKAGESRCDIR=$PACKAGEDIR/usr/local/src/diyabc
else
    PACKAGESRCDIR=$PACKAGEDIR/usr/local/src/diyabc-$VERSION
fi
# copy of all the usefull file into usr/local/src
mkdir $PACKAGESRCDIR
cp -r $SOURCEDIR/*.py $SOURCEDIR/clean.sh $SOURCEDIR/analysis $SOURCEDIR/uis $SOURCEDIR/utils $SOURCEDIR/summaryStatistics $SOURCEDIR/mutationModel $PACKAGESRCDIR/
# version modification
sed -i "s/VERSION='development version'/VERSION='$VERSION ($BUILDDATE)'/" $PACKAGESRCDIR/diyabc.py
sed -i "s/VVERSION/$VERSION/" $PACKAGEDIR/usr/share/menu/diyabc
sed -i "s/VVERSION/$VERSION/" $PACKAGEDIR/usr/share/applications/diyabc.desktop
if [ $MAIN == "MAIN" ]; then
    sed -i "s/ NAMEVERSION//" $PACKAGEDIR/usr/share/menu/diyabc
    sed -i "s/ICON/\/usr\/local\/src\/diyabc\/docs\/accueil_pictures\/coccicon.png/" $PACKAGEDIR/usr/share/applications/diyabc.desktop
    sed -i "s/ NAMEVERSION//" $PACKAGEDIR/usr/share/applications/diyabc.desktop
    sed -i "s/DIYABCEXEC/\/usr\/local\/bin\/diyabc/" $PACKAGEDIR/usr/share/applications/diyabc.desktop
    sed -i "s/COMMANDVERSION//" $PACKAGEDIR/usr/share/menu/diyabc
else
    sed -i "s/ NAMEVERSION/ $VERSION/" $PACKAGEDIR/usr/share/menu/diyabc
    sed -i "s/ NAMEVERSION/ $VERSION/" $PACKAGEDIR/usr/share/applications/diyabc.desktop
    sed -i "s/ICON/\/usr\/local\/src\/diyabc-$VERSION\/docs\/accueil_pictures\/coccicon.png/" $PACKAGEDIR/usr/share/applications/diyabc.desktop
    sed -i "s/DIYABCEXEC/\/usr\/local\/bin\/diyabc-$VERSION/" $PACKAGEDIR/usr/share/applications/diyabc.desktop
    sed -i "s/COMMANDVERSION/-$VERSION/" $PACKAGEDIR/usr/share/menu/diyabc
    mv $PACKAGEDIR/usr/share/menu/diyabc $PACKAGEDIR/usr/share/menu/diyabc-$VERSION
    mv $PACKAGEDIR/usr/share/applications/diyabc.desktop $PACKAGEDIR/usr/share/applications/diyabc-$VERSION.desktop
fi
mkdir $PACKAGESRCDIR/docs
cp -r $SOURCEDIR/docs/accueil_pictures $PACKAGESRCDIR/docs/
cp $SOURCEDIR/docs/*.png $SOURCEDIR/docs/dev* $SOURCEDIR/docs/doc* $PACKAGESRCDIR/docs/
if [ $MAIN == "MAIN" ]; then
    # generation of the launch script placed in /usr/local/bin
    echo "#!/bin/bash
cd /usr/local/src/diyabc/
python /usr/local/src/diyabc/diyabc.py" > $PACKAGEDIR/usr/local/bin/diyabc
    # change owner:group to root
    echo "chown -R 0:0 /usr/local/bin/diyabc /usr/local/src/diyabc /usr/share/applications/diyabc.desktop /usr/share/menu/diyabc" >> $PACKAGEDIR/DEBIAN/postinst
    chmod +x $PACKAGEDIR/usr/local/bin/diyabc
else
    # generation of the launch script placed in /usr/local/bin
    echo "#!/bin/bash
cd /usr/local/src/diyabc-$VERSION/
python /usr/local/src/diyabc-$VERSION/diyabc.py" > $PACKAGEDIR/usr/local/bin/diyabc-$VERSION
    # change owner:group to root
    echo "chown -R 0:0 /usr/local/bin/diyabc-$VERSION /usr/local/src/diyabc-$VERSION/ /usr/share/applications/diyabc-$VERSION.desktop /usr/share/menu/diyabc-$VERSION" >> $PACKAGEDIR/DEBIAN/postinst
    chmod +x $PACKAGEDIR/usr/local/bin/diyabc-$VERSION
fi

dpkg-deb -b $PACKAGEDIR

