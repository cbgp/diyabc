#!/bin/bash

function printUsage(){
echo "usage : 
linux_generation.sh  path_to_pyinstaller.py  [output_path  path_to_main.py] 
"
}


pyinst=$1
output=$2
pysrc=$3
BUILDDATE=`date +%d-%m-%Y`

if [ $# -eq 0 ] ; then
    printUsage
    exit
else
    if [ $# -eq 1 ]; then
        output="./lingeneration_"+`date +%e-%m-%Y_%R`
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

TMPBUILD=/tmp/$APPNAME-tmp_build-$VERSION
mkdir $TMPBUILD
SOURCEDIR=`dirname $pysrc`
cp -r $SOURCEDIR/*.py $SOURCEDIR/clean.sh $SOURCEDIR/analysis $SOURCEDIR/uis $SOURCEDIR/utils $SOURCEDIR/summaryStatistics $SOURCEDIR/mutationModel $SOURCEDIR/historicalModel $SOURCEDIR/geneticData $TMPBUILD/
pysrctmp=$TMPBUILD/`basename $pysrc`
#sed -i "s/development\ version/$VERSION ($BUILDDATE)/g" $TMPBUILD/$APPNAME.py
sed -i "s/development\ version/$VERSION/g" $TMPBUILD/$APPNAME.py
sed -i "s/01\/01\/1970/$BUILDDATE/g" $TMPBUILD/$APPNAME.py
echo "sed -i 's/development\ version/$VERSION ($BUILDDATE)/g' $TMPBUILD/$APPNAME.py"
echo `cat $TMPBUILD/$APPNAME.py | grep VERSION`

# nettoyage de la config de pyinstaller
rm -f `dirname $pyinst`/config.dat
# lancement de la generation
python $pyinst $pysrctmp -o "$output"

# copy of needed images
mkdir $output/dist/$APPNAME/docs
cp -r $SOURCEDIR/docs/accueil_pictures $SOURCEDIR/docs/executables $SOURCEDIR/docs/icons $SOURCEDIR/docs/dev* $SOURCEDIR/docs/doc* $output/dist/$APPNAME/docs/
cp -r $SOURCEDIR/uis $output/dist/$APPNAME/
rm -rf $TMPBUILD
sleep 3
mv $output/dist/$APPNAME $output/dist/$APPNAME-$VERSION
mv $output/dist $output/$APPNAME-$VERSION
