#!/bin/bash

pyinst=$1
icon=$2
output=$3
pysrc=$4

# generation of the spec
python $pyinst -o $output $pysrc
# modification of the spec to generate .app
echo "
import sys
if sys.platform.startswith('darwin'):
    app = BUNDLE(exe,
    appname='DIYABC',
    version='1.0')" >> $output/diyabc.spec
# .app build
python $pyinst $output/diyabc.spec
# icon copy
cp $icon $output/Macdiyabc.app/Contents/Resources/App.icns
# dist rep copy
cp -r $output/dist/diyabc/* $output/Macdiyabc.app/Contents/MacOS/
# qmenu copy
cp -r /Library/Frameworks/QtGui.framework/Versions/4/Resources/qt_menu.nib/ $output/Macdiyabc.app/Contents/Resources/
# config file modification
sed -i "" "s/string>1</string>0</g" $output/Macdiyabc.app/Contents/Info.plist
