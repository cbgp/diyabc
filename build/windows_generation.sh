#!/bin/bash

function printUsage(){
echo "usage : 
#windows_generation.sh  path_to_pyinstaller.py  [path_to_icon.ico  output_path  path_to_main.py] 

windows_generation.sh  (-p | --pyinst) path_to_pyinstaller.py  (-i | --icon) path_to_icon.ico (-o | --output) output_path  (-m | --main) path_to_main.py (-a | --arch) arch(32 or 64)
"
}
#-------------
ARGS=$(getopt -o p:i:o:m:a: -l "pyinst:icon:output:main:arch:" -n "linux_generation.sh" -- "$@");

#Bad arguments
if [ $? -ne 0 ] || [ $# -eq 0 ];
then
      printUsage
    exit
fi
MAINFLAG=0
PYINSTFLAG=0
ICOFLAG=0
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
    -i|--icon)
      shift;
      if [ -n "$1" ]; then
        if [ -f "$1" ]; then
            ico=$1
            ICOFLAG=1
        else
            echo "Icon $1 file does not exist"
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

if [ "$PYINSTFLAG" == 0 ] && [ "$ICOFLAG" == 0 ] &&[ "$MAINFLAG" == 0 ] && [ "$OUTFLAG" == 0 ] && [ "$ARCHFLAG" == 0 ]; then
    printUsage
    exit
fi


#---------------
#pyinst=$1
#icon=$2
#output=$3
#pysrc=$4
#ARCH=$5

BUILDDATE=`date +%d-%m-%Y`

if [ "$ICOFLAG" == "0" ]
    then
        icon="../icons/coccicon.ico"
        echo "Take default icon path : ../icons/coccicon.ico\n"
fi
if [ "$OUPUTFLAG" == "0" ]
    then
        output="./wingeneration_"+`date +%e-%m-%Y_%R`
        echo "Your application will be generated in $output . Press enter to continue..."
        read
        pysrc="../../diyabc.py"
        VERSION="development"
    fi
fi
echo "I assume you've intalled python-2.6, pyqwt, numpy, PyQt"
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
test -d $SOURCEDIR/../../../html && cp -r $SOURCEDIR/../../../html $TMPBUILD/html
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
for filepath in $TMPBUILD/data/txt/*;do 
    datalist=$datalist"('data/txt/`basename $filepath`','`echo $filepath | sed s=/=xxx123=g`','DATA'),"
done
for filepath in $TMPBUILD/data/icons/*;do 
    datalist=$datalist"('data/icons/`basename $filepath`','`echo $filepath | sed s=/=xxx123=g`','DATA'),"
done
for filepath in $TMPBUILD/data/images/*;do 
    datalist=$datalist"('data/images/`basename $filepath`','`echo $filepath | sed s=/=xxx123=g`','DATA'),"
done
for filepath in $TMPBUILD/data/bin/*;do 
    datalist=$datalist"('data/bin/`basename $filepath`','`echo $filepath | sed s=/=xxx123=g`','DATA'),"
done
for filepath in $TMPBUILD/uis/*;do 
    datalist=$datalist"('uis/`basename $filepath`','`echo $filepath | sed s=/=xxx123=g`','DATA'),"
done
if [ -d $TMPBUILD/html ]; then
    for filepath in $TMPBUILD/html/*;do
        datalist=$datalist"('doc/html/`basename $filepath`','`echo $filepath | sed s=/=xxx123=g`','DATA'),"
    done
fi
datalist=$datalist"]"
sed -i "s>pyz\ =>$datalist \npyz = >g" $output/diyabc.spec
sed -i 's>xxx123cxxx123>c:\\\\>g' $output/diyabc.spec
sed -i 's>xxx123>\\\\>g' $output/diyabc.spec
python $pyinst $output/diyabc.spec

# copy of needed images
#mkdir $output/dist/data
#cp -r $SOURCEDIR/../data/images $SOURCEDIR/../data/bin $SOURCEDIR/../data/icons  $output/dist/data/
#cp -r $SOURCEDIR/uis $output/dist/
rm -rf $TMPBUILD
sleep 3
mv $output/dist/$APPNAME.exe $output/dist/$APPNAME-$VERSION.exe
mv $output/dist $output/$APPNAME-$VERSION-win$ARCH
rm -rf $output/build $output/$APPNAME.spec
