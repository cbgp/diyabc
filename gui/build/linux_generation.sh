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
    echo "I assume you've intalled python-2.6, matplotlib, numpy, PyQt"
fi
VERSIONFILE="`dirname $pysrc`/../version.txt"
VERSION="`head -n 1 $VERSIONFILE`"

APPNAME=`basename $pysrc | cut -d . -f 1`

TMPBUILD=/tmp/$APPNAME-tmp_build-$VERSION
mkdir $TMPBUILD
SOURCEDIR=`dirname $pysrc`
#cp -r $SOURCEDIR/*.py $SOURCEDIR/clean.sh $SOURCEDIR/analysis $SOURCEDIR/uis $SOURCEDIR/utils $SOURCEDIR/summaryStatistics $SOURCEDIR/mutationModel $SOURCEDIR/historicalModel $SOURCEDIR/geneticData $TMPBUILD/
cp -r $SOURCEDIR $TMPBUILD/
cp -r $SOURCEDIR/../data/ $TMPBUILD/
cp -r $SOURCEDIR/../utils/ $TMPBUILD/src/
pysrctmp=$TMPBUILD/src/`basename $pysrc`
#sed -i "s/development\ version/$VERSION ($BUILDDATE)/g" $TMPBUILD/$APPNAME.py
sed -i "s/development\ version/$VERSION/g" $TMPBUILD/src/dataPath.py
sed -i "s/01\/01\/1970/$BUILDDATE/g" $TMPBUILD/src/dataPath.py
echo "sed -i 's/development\ version/$VERSION ($BUILDDATE)/g' $TMPBUILD/src/dataPath.py"
echo `cat $TMPBUILD/src/dataPath.py | grep VERSION`

# nettoyage de la config de pyinstaller
rm -f `dirname $pyinst`/config.dat
# lancement de la generation
python $pyinst $pysrctmp -o "$output"

# copy of needed images
mkdir $output/dist/$APPNAME/data/
cp -r $SOURCEDIR/../data/images $SOURCEDIR/../data/executables $SOURCEDIR/../data/icons  $output/dist/$APPNAME/data/
cp -r $SOURCEDIR/uis /etc/matplotlibrc $output/dist/$APPNAME/
#rm -rf $TMPBUILD
sleep 3
mv $output/dist/$APPNAME $output/dist/$APPNAME-$VERSION
mv $output/dist/$APPNAME-$VERSION $output/$APPNAME-$VERSION-linux64
rmdir $output/dist
rm -rf $output/build $output/$APPNAME.spec
