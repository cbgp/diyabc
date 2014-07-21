#!/bin/bash

function printUsage(){
echo "usage : 
#windows_generation.sh  -p  path_to_pyinstaller.py  -i  path_to_icon.ico -o output_path  -m  path_to_main.py -a (32 or 64)
#" >&2
}

#Bad arguments
if [ $? -ne 0 ] || [ $# -eq 0 ];
then
      printUsage
    exit 4
fi
MAINFLAG=0
PYINSTFLAG=0
ICOFLAG=0
OUTFLAG=0
ARCHFLAG=0
#eval set -- "$ARGS";

while getopts :p:i:o:m:a: option; do
  case $option in
    p)
      if [ -n "$OPTARG" ]; then
        if [ -f "$OPTARG" ]; then
            pyinst="$OPTARG"
            PYINSTFLAG=1
        else
            echo "Pyinstaller.py file does not exist" >&2
            exit 4
        fi
      fi
      ;;
    i)
      if [ -n "$OPTARG" ]; then
        if [ -f "$OPTARG" ]; then
            ico="$OPTARG"
            ICOFLAG=1
        else
            echo "Icon $OPTARG file does not exist" >&2
             exit 4
        fi
      fi
      ;;  
    o)
      if [ -n "$OPTARG" ]; then
        if [ -d `dirname "$OPTARG"` ]; then
            output="$OPTARG"
            OUTFLAG=1
        else
            echo "Impossible to use this location for output" >&2
            exit 4
        fi
      fi
      ;;
    m)
      if [ -n "$OPTARG" ]; then
        if [ -f "$OPTARG" ]; then
            pysrc="$OPTARG"
            MAINFLAG=1
        else
            echo "main python file $OPTARG does not exist" >&2
            exit 4
        fi
      fi
      ;;
    a)
      if [ -n "$OPTARG" ]; then
        if [ "$OPTARG" != "64" ] && [ "$OPTARG" != "32" ]; then
            echo "invalid architecture, 64 or 32 expected, $OPTARG given" >&2
            exit 4
        fi
        ARCH=$OPTARG
        ARCHFLAG=1
      fi
      ;;
    \?)
       printUsage
       echo "invalid option -$OPTARG" >&2
       exit 4 
      ;;
     :)
       printUsage
       echo "option -$OPTARG reaquire an argument" >&2
       exit 4
       ;;
  esac
done


if [ "$PYINSTFLAG" == 0 ] && [ "$ICOFLAG" == 0 ] &&[ "$MAINFLAG" == 0 ] && [ "$OUTFLAG" == 0 ] && [ "$ARCHFLAG" == 0 ]; then
    printUsage
    exit
fi


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
#Documention removed   
#if [ -d $TMPBUILD/html ]; then
#    for filepath in $TMPBUILD/html/*;do
#        datalist=$datalist"('doc/html/`basename $filepath`','`echo $filepath | sed s=/=xxx123=g`','DATA'),"
#    done
#fi

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
