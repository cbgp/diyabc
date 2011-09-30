#!/bin/bash

function printUsage(){
echo "usage : 
mac_generation.sh  path_to_pyinstaller.py  [path_to_icon.icns  output_path  path_to_main.py] 
"
}


pyinst=$1
icon=$2
output=$3
pysrc=$4
BUILDDATE=`date +%d-%b-%Y`


if [ $# -eq 0 ] ; then
    printUsage
    exit
else
    if [ $# -eq 1 ]; then
        icon="../accueil_pictures/coccicon.icns"
        output="./macgeneration_"+`date +%e-%m-%Y_%R`
        echo "Your application will be generated in $output . Press enter to continue..."
        read
        pysrc="../../diyabc.py"
        VERSION="development"
    fi
    echo "I assume you've intalled python-2.6, pyqwt, numpy, PyQt"
fi
VERSIONFILE="`dirname $pysrc`/version.txt"
VERSION="`head -n 1 $VERSIONFILE`"

APPNAME=`basename $pysrc | cut -d . -f 1`
echo "Name of the app : $APPNAME"

TMPBUILD=/tmp/$APPNAME-tmp_build-$VERSION
mkdir $TMPBUILD
SOURCEDIR=`dirname $pysrc`
cp -r $SOURCEDIR/*.py $SOURCEDIR/clean.sh $SOURCEDIR/analysis $SOURCEDIR/uis $SOURCEDIR/utils $SOURCEDIR/summaryStatistics $SOURCEDIR/mutationModel $TMPBUILD/
pysrctmp=$TMPBUILD/`basename $pysrc`
sed -i "" "s/development\ version/$VERSION ($BUILDDATE)/g" $TMPBUILD/$APPNAME.py

# nettoyage de la config de pyinstaller
rm -f `dirname $pyinst`/config.dat
# generation of the spec
python $pyinst -y -o $output $pysrctmp
# modification of the spec to generate .app
echo "
import sys
if sys.platform.startswith('darwin'):
    app = BUNDLE(exe,
    appname='$APPNAME',
    version='$VERSION')" >> $output/$APPNAME.spec
# .app build
python $pyinst -y $output/$APPNAME.spec
# icon copy
cp $icon $output/Mac$APPNAME.app/Contents/Resources/App.icns
# dist rep copy
cp -r $output/dist/$APPNAME/* $output/Mac$APPNAME.app/Contents/MacOS/
# qmenu copy
cp -r /Library/Frameworks/QtGui.framework/Versions/4/Resources/qt_menu.nib $output/Mac$APPNAME.app/Contents/Resources/
# config file modification
sed -i "" "s/string>1</string>0</g" $output/Mac$APPNAME.app/Contents/Info.plist
sed -i "" "s/Macdiyabc/Diyabc/g" $output/Mac$APPNAME.app/Contents/Info.plist
mv $output/Mac$APPNAME.app $output/$APPNAME-$VERSION.app
# copy of needed images
mkdir $output/docs
cp -r $SOURCEDIR/docs/accueil_pictures $SOURCEDIR/docs/icons $SOURCEDIR/docs/executables $SOURCEDIR/docs/dev* $SOURCEDIR/docs/doc* $output/docs/
cp -r $SOURCEDIR/uis/ $output/uis
# clean
rm -rf $TMPBUILD
