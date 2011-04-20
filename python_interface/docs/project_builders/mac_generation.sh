#!/bin/bash

function printUsage(){
echo "usage : 
mac_generation.sh  path_to_pyinstaller.py  [path_to_icon.icns  output_path  path_to_main.py  version] 
"
}


pyinst=$1
icon=$2
output=$3
pysrc=$4
VERSION=$5

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

TMPBUILD=/tmp/diyabc_tmp_build-$VERSION
mkdir TMPBUILD
SOURCEDIR=`dirname $pysrc`
cp -r $SOURCEDIR/*.py $SOURCEDIR/clean.sh $SOURCEDIR/analysis $SOURCEDIR/uis $SOURCEDIR/utils $SOURCEDIR/summaryStatistics $SOURCEDIR/mutationModel $TMPBUILD/
pysrctmp=$SOURCEDIR/`basename $pysrc`

# generation of the spec
python $pyinst -y -o $output $pysrctmp
# modification of the spec to generate .app
echo "
import sys
if sys.platform.startswith('darwin'):
    app = BUNDLE(exe,
    appname='`basename pysrc | cut -d '.' -f 1` $VERSION',
    version='$VERSION')" >> $output/diyabc.spec
# .app build
python $pyinst -y $output/diyabc.spec
# icon copy
cp $icon $output/Macdiyabc.app/Contents/Resources/App.icns
# dist rep copy
cp -r $output/dist/diyabc/* $output/Macdiyabc.app/Contents/MacOS/
# qmenu copy
cp -r /Library/Frameworks/QtGui.framework/Versions/4/Resources/qt_menu.nib/ $output/Macdiyabc.app/Contents/Resources/
# config file modification
sed -i "" "s/string>1</string>0</g" $output/Macdiyabc.app/Contents/Info.plist
mv $output/Macdiyabc.app $output/diyabc-$VERSION.app
