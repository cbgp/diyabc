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
BUILDDATE=`date +%d-%m-%Y`


if [ $# -eq 0 ] ; then
    printUsage
    exit
else
    if [ $# -eq 1 ]; then
        icon="../icons/coccicon.icns"
        output="./macgeneration_"`date +%e-%m-%Y_%R`
        output=`echo $output | sed "s/ //g"`
        echo "Your application will be generated in $output . Press enter to continue..."
        read
        pysrc="../../diyabc.py"
        VERSION="development"
    fi
    echo "I assume you've intalled python-2.6, pyqwt, numpy, PyQt"
fi
VERSIONFILE="`dirname $pysrc`/../version.txt"
VERSION="`head -n 1 $VERSIONFILE`"

APPNAME=`basename $pysrc | cut -d . -f 1`
echo "Name of the app : $APPNAME"

TMPBUILD=/tmp/$APPNAME-tmp_build-$VERSION
rm -rf $TMPBUILD
mkdir $TMPBUILD
SOURCEDIR=`dirname $pysrc`
#cp -r $SOURCEDIR/*.py $SOURCEDIR/clean.sh $SOURCEDIR/analysis $SOURCEDIR/uis $SOURCEDIR/utils $SOURCEDIR/summaryStatistics $SOURCEDIR/mutationModel $SOURCEDIR/historicalModel $SOURCEDIR/geneticData $TMPBUILD/
cp -r $SOURCEDIR $TMPBUILD/
cp -r $SOURCEDIR/../data $TMPBUILD/
cp -r $SOURCEDIR/../utils $TMPBUILD/src/
pysrctmp=$TMPBUILD/src/`basename $pysrc`
#sed -i "" "s/development\ version/$VERSION ($BUILDDATE)/g" $TMPBUILD/$APPNAME.py
sed -i "" "s/development\ version/$VERSION/g" $TMPBUILD/src/variables.py
sed -i "" "s/01\/01\/1970/$BUILDDATE/g" $TMPBUILD/src/variables.py

# nettoyage de la config de pyinstaller
rm -f `dirname $pyinst`/config.dat
# generation of the spec
python2.6 $pyinst -y -o $output $pysrctmp
# modification of the spec to generate .app
echo "
import sys
if sys.platform.startswith('darwin'):
    app = BUNDLE(exe,
    appname='$APPNAME',
    version='$VERSION')" >> $output/$APPNAME.spec
# .app build
python2.6 $pyinst -y $output/$APPNAME.spec
# icon copy
cp $icon $output/Mac$APPNAME.app/Contents/Resources/icon-windowed.icns
# dist rep copy
cp -r $output/dist/$APPNAME/* $output/Mac$APPNAME.app/Contents/MacOS/
# qmenu copy
if [ -d /Library/Frameworks/QtGui.framework/Versions/4/Resources/qt_menu.nib ]; then
	cp -r /Library/Frameworks/QtGui.framework/Versions/4/Resources/qt_menu.nib $output/Mac$APPNAME.app/Contents/Resources/
else
	if [ -d /opt/local/lib/Resources/qt_menu.nib ]; then
		cp -r /opt/local/lib/Resources/qt_menu.nib $output/Mac$APPNAME.app/Contents/Resources/
	else
		echo "qt_menu.nib copy FAILURE. App will be unusable"
	fi
fi
# config file modification
sed -i "" "s/string>1</string>0</g" $output/Mac$APPNAME.app/Contents/Info.plist
sed -i "" "s/Macdiyabc/Diyabc/g" $output/Mac$APPNAME.app/Contents/Info.plist
#if [ `grep diyabc $output/Mac$APPNAME.app/Contents/Info.plist | wc -l` -eq 1 ]; then
#    sed -i "" "s/diyabc/MacOS\/diyabc/g" $output/Mac$APPNAME.app/Contents/Info.plist
#fi
mv $output/Mac$APPNAME.app $output/$APPNAME-$VERSION.app
# copy of needed images
mkdir $output/$APPNAME-$VERSION.app/Contents/Resources/data
#cp -r $SOURCEDIR/docs/accueil_pictures $SOURCEDIR/docs/icons $SOURCEDIR/docs/executables $SOURCEDIR/docs/dev* $SOURCEDIR/docs/doc* $output/$APPNAME-$VERSION.app/Contents/Resources/docs/
cp -r $SOURCEDIR/../data/images $SOURCEDIR/../data/bin $SOURCEDIR/../data/icons  $output/$APPNAME-$VERSION.app/Contents/Resources/data/
cp -r $SOURCEDIR/uis/ $output/$APPNAME-$VERSION.app/Contents/Resources/uis
test -d $SOURCEDIR/../../../html && mkdir $output/$APPNAME-$VERSION.app/Contents/Resources/doc && cp -r $SOURCEDIR/../../../html $output/$APPNAME-$VERSION.app/Contents/Resources/doc/html

rm -rf $output/build $output/dist $output/$APPNAME.spec
# a tester, sinon : yoursway-create-dmg
#ln -s /Applications "$output/Applications"
##cp "$icon" "$output/.VolumeIcon.icns"
##SetFile -c icnC "$output/.VolumeIcon.icns"
#hdiutil create $output/$APPNAME-$VERSION-mac.dmg -srcfolder "$output/" -volname $APPNAME-$VERSION
##hdiutil create $output/$APPNAME-$VERSION-mac.dmg -srcfolder $output/$APPNAME-$VERSION.app -volname $APPNAME-$VERSION
cd yoursway-create-dmg/
echo "./create-dmg --window-size 500 300 --background ../../gui/data/images/right-arrow.png --icon-size 96 --volname '$APPNAME-$VERSION' --volicon ../../gui/data/icons/coccicon.icns --app-drop-link 380 55 --icon '$APPNAME-$VERSION' 110 55 ../$output/$APPNAME-$VERSION-mac.dmg ../$output/$APPNAME-$VERSION.app"
./create-dmg --window-size 500 300 --background ../../gui/data/images/right-arrow-bluegrey.png --icon-size 96 --volname "$APPNAME-$VERSION" --volicon ../../gui/data/icons/coccicon.icns --app-drop-link 380 55 --icon "$APPNAME-$VERSION" 110 55 ../$output/$APPNAME-$VERSION-mac.dmg ../$output/$APPNAME-$VERSION.app
cd ..
# clean
rm -rf $TMPBUILD
