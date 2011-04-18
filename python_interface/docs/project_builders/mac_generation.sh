#!/bin/bash

function printUsage(){
echo "usage : 
mac_generation.sh  path_to_pyinstaller.py  [path_to_icon.icns  output_path  path_to_diyabc.py] 
"
}


pyinst=$1
icon=$2
output=$3
pysrc=$4

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
    fi
    echo "I assume you've intalled python-2.6, pyqwt, numpy, PyQt"
fi

# generation of the spec
python $pyinst -y -o $output $pysrc
# modification of the spec to generate .app
echo "
import sys
if sys.platform.startswith('darwin'):
    app = BUNDLE(exe,
    appname='DIYABC',
    version='1.0')" >> $output/diyabc.spec
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
