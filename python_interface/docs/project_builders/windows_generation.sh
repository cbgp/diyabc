#!/bin/bash

function printUsage(){
echo "usage : 
windows_generation.sh  path_to_pyinstaller.py  [path_to_icon.ico  output_path  path_to_main.py  version] 
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
        output="./wingeneration_"+`date +%e-%m-%Y_%R`
        echo "Your application will be generated in $output . Press enter to continue..."
        read
        pysrc="../../diyabc.py"
        VERSION="development"
    fi
    echo "I assume you've intalled python-2.6, pyqwt, numpy, PyQt"
fi

APPNAME=`basename $pysrc | cut -d . -f 1`

TMPBUILD=/c/$APPNAME-tmp_build-$VERSION
mkdir $TMPBUILD
SOURCEDIR=`dirname $pysrc`
cp -r $SOURCEDIR/*.py $SOURCEDIR/clean.sh $SOURCEDIR/analysis $SOURCEDIR/uis $SOURCEDIR/utils $SOURCEDIR/summaryStatistics $SOURCEDIR/mutationModel $TMPBUILD/
pysrctmp=$TMPBUILD/`basename $pysrc`
sed -i "s/development\ version/$VERSION/g" $TMPBUILD/$APPNAME.py

python $pyinst $pysrctmp --onefile -w --icon="$icon" -o "$output"

# copy of needed images
mkdir $output/dist/docs
cp -r $SOURCEDIR/docs/accueil_pictures $SOURCEDIR/docs/*.png $output/dist/docs/
rm -rf $TMPBUILD
sleep 3
mv $output/dist/$APPNAME.exe $output/dist/$APPNAME-$VERSION.exe
