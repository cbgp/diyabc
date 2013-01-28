#!/bin/bash

function printUsage(){
echo "usage : 
windows_generation.sh  path_to_pyinstaller.py  [path_to_icon.ico  output_path  path_to_main.py] 
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
        icon="../icons/coccicon.ico"
        output="./wingeneration_"+`date +%e-%m-%Y_%R`
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

TMPBUILD=/c/$APPNAME-tmp_build-$VERSION
rm -rf $TMPBUILD
mkdir $TMPBUILD
SOURCEDIR=`dirname $pysrc`
#cp -r $SOURCEDIR/*.py $SOURCEDIR/clean.sh $SOURCEDIR/analysis $SOURCEDIR/uis $SOURCEDIR/utils $SOURCEDIR/summaryStatistics $SOURCEDIR/mutationModel $SOURCEDIR/historicalModel $SOURCEDIR/geneticData $TMPBUILD/
cp -r $SOURCEDIR $TMPBUILD/
cp -r $SOURCEDIR/../data/ $TMPBUILD/
cp -r $SOURCEDIR/../utils/ $TMPBUILD/src/
cp -r $SOURCEDIR/uis $TMPBUILD/
pysrctmp=$TMPBUILD/src/`basename $pysrc`
#sed -i "s/development\ version/$VERSION ($BUILDDATE)/g" $TMPBUILD/$APPNAME.py
sed -i "s/development\ version/$VERSION/g" $TMPBUILD/src/variables.py
sed -i "s/01\/01\/1970/$BUILDDATE/g" $TMPBUILD/src/variables.py
echo "sed -i 's/development\ version/$VERSION ($BUILDDATE)/g' $TMPBUILD/src/variables.py"
echo `cat $TMPBUILD/src/variables.py | grep VERSION`

# nettoyage de la config de pyinstaller
rm -f `dirname $pyinst`/config.dat
# lancement de la generation
#python $pyinst $pysrctmp --onefile -w --icon="$icon" -o "$output"

python `dirname $pyinst`/utils/Makespec.py --onefile $pysrctmp -w --icon="$icon" -o "$output"
datalist="a.datas+=["
for filepath in $TMPBUILD/data/icons/*;do 
    datalist=$datalist"('data/icons/`basename $filepath`','$filepath','DATA'),"
done
for filepath in $TMPBUILD/data/images/*;do 
    datalist=$datalist"('data/images/`basename $filepath`','$filepath','DATA'),"
done
for filepath in $TMPBUILD/data/bin/*;do 
    datalist=$datalist"('data/bin/`basename $filepath`','$filepath','DATA'),"
done
for filepath in $TMPBUILD/uis/*;do 
    datalist=$datalist"('uis/`basename $filepath`','$filepath','DATA'),"
done
datalist=$datalist"]"
sed -i "s>pyz\ =>$datalist \npyz = >g" $output/diyabc.spec
python $pyinst $output/diyabc.spec

# copy of needed images
#mkdir $output/dist/data
#cp -r $SOURCEDIR/../data/images $SOURCEDIR/../data/bin $SOURCEDIR/../data/icons  $output/dist/data/
#cp -r $SOURCEDIR/uis $output/dist/
rm -rf $TMPBUILD
sleep 3
mv $output/dist/$APPNAME.exe $output/dist/$APPNAME-$VERSION.exe
mv $output/dist $output/$APPNAME-$VERSION-win32
rm -rf $output/build $output/$APPNAME.spec
