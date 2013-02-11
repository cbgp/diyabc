#!/bin/bash

function printUsage(){
echo "usage : 
linux_generation.sh  (-p | --pyinst) path_to_pyinstaller.py  (-o | --output) output_path  (-m | --main) path_to_main.py (-a | --arch) arch(32 or 64)
"
}

ARGS=$(getopt -o p:o:m:a: -l "pyinst:output:main:arch:" -n "linux_generation.sh" -- "$@");

#Bad arguments
if [ $? -ne 0 ] || [ $# -eq 0 ];
then
      printUsage
    exit
fi
MAINFLAG=0
PYINSTFLAG=0
OUTFLAG=0
ARCHFLAG=0
eval set -- "$ARGS";

while true; do
  case "$1" in
    -p|--pyinst)
      shift;
      if [ -n "$1" ]; then
        if [ -f "$1" ]; then
            pyinst=$1
            PYINSTFLAG=1
        else
            echo "Pyinstaller.py file does not exist"
            exit
        fi
        shift;
      fi
      ;;
    -o|--output)
      shift;
      if [ -n "$1" ]; then
        if [ -d `dirname "$1"` ]; then
            output=$1
            OUTFLAG=1
        else
            echo "Impossible to use this location for output"
            exit
        fi
        shift;
      fi
      ;;
    -m|--main)
      shift;
      if [ -n "$1" ]; then
        if [ -f "$1" ]; then
            pysrc=$1
            MAINFLAG=1
        else
            echo "main python file does not exist"
            exit
        fi
        shift;
      fi
      ;;
    -a|--arch)
      shift;
      if [ -n "$1" ]; then
        if [ "$1" != "64" ] && [ "$1" != "32" ]; then
            echo "invalid architecture"
            exit
        fi
        ARCH=$1
        ARCHFLAG=1
        shift;
      fi
      ;;
    --)
      shift;
      break;
      ;;
  esac
done

if [ $PYINSTFLAG == 0 ] && [ $MAINFLAG == 0 ] && [ $OUTFLAG == 0 ] && [ $ARCHFLAG == 0 ]; then
    printUsage
    exit
fi

BUILDDATE=`date +%d-%m-%Y`

VERSIONFILE="`dirname $pysrc`/../version.txt"
VERSION="`head -n 1 $VERSIONFILE`"

APPNAME=`basename $pysrc | cut -d . -f 1`

TMPBUILD=/tmp/$APPNAME-tmp_build-$VERSION
rm -rf $TMPBUILD
mkdir $TMPBUILD
SOURCEDIR=`dirname $pysrc`
#cp -r $SOURCEDIR/*.py $SOURCEDIR/clean.sh $SOURCEDIR/analysis $SOURCEDIR/uis $SOURCEDIR/utils $SOURCEDIR/summaryStatistics $SOURCEDIR/mutationModel $SOURCEDIR/historicalModel $SOURCEDIR/geneticData $TMPBUILD/
cp -r $SOURCEDIR $TMPBUILD/
cp -r $SOURCEDIR/../data/ $TMPBUILD/
cp -r $SOURCEDIR/../../Notice-DIYABC-v2/Notice_DIYABC_principal2 $TMPBUILD/html
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
python `dirname $pyinst`/utils/Makespec.py --onefile $pysrctmp -o "$output"
datalist="a.datas+=["
for filepath in $TMPBUILD/data/txt/*;do 
    datalist=$datalist"('data/txt/`basename $filepath`','$filepath','DATA'),"
done
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
if [ -d $TMPBUILD/html ]; then
    for filepath in $TMPBUILD/html/*;do
        datalist=$datalist"('doc/html/`basename $filepath`','$filepath','DATA'),"
    done
fi
datalist=$datalist"]"
sed -i "s>pyz\ =>$datalist \npyz = >g" $output/diyabc.spec
python $pyinst $output/diyabc.spec

# copy of needed images
#mkdir -p $output/$APPNAME-$VERSION-linux$ARCH/data/
#cp -r $SOURCEDIR/../data/images $SOURCEDIR/../data/bin $SOURCEDIR/../data/icons  $output/$APPNAME-$VERSION-linux$ARCH/data/
#cp -r $SOURCEDIR/uis /etc/matplotlibrc $output/$APPNAME-$VERSION-linux$ARCH/
rm -rf $TMPBUILD
sleep 3
mkdir $output/$APPNAME-$VERSION-linux$ARCH/
mv $output/dist/$APPNAME $output/$APPNAME-$VERSION-linux$ARCH/
rmdir $output/dist
rm -rf $output/build $output/$APPNAME.spec
