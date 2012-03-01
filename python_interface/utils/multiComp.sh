# usage : multiComp.sh execPath directory motifavant motifaprès

BINPATH=$1
DIRPATH=$2
BEF=$3
AFT=$4
DATAFILENAME=$5

cp $DIRPATH/$DATAFILENAME $DIRPATH/datafileSAVE
if [ -e $DIRPATH/$DATAFILENAME.bin ] ; then
    echo rm $DIRPATH/$DATAFILENAME.bin
    rm $DIRPATH/$DATAFILENAME.bin
fi

for i in `ls "$DIRPATH" | grep -e "$BEF.*$AFT" | grep -vi result` ; do
    echo mv $DIRPATH/$i $DIRPATH/$DATAFILENAME
    mv $DIRPATH/$i $DIRPATH/$DATAFILENAME

    echo "$BINPATH" -p "$DIRPATH" -c "s:1,2;n:200000;d:500;l:5000;m:10;f:0" -i "$i" -g 100 -m -t 7
    "$BINPATH" -p "$DIRPATH" -c "s:1,2;n:200000;d:500;l:5000;m:10;f:0" -i "$i" -g 100 -m -t 7 > /tmp/plop

    echo mv $DIRPATH/$DATAFILENAME $DIRPATH/$i
    mv $DIRPATH/$DATAFILENAME $DIRPATH/$i

    if [ -e $DIRPATH/$DATAFILENAME.bin ] ; then
        echo rm $DIRPATH/$DATAFILENAME.bin
        rm $DIRPATH/$DATAFILENAME.bin
    fi
    echo ""
done
RESULTDIR="$DIRPATH"/results_"$BEF$AFT"_`date '+%T'`
mkdir $RESULTDIR
mv "$DIRPATH"/*complogreg* $RESULTDIR
mv "$DIRPATH"/*compdirect* $RESULTDIR
mv "$DIRPATH"/*progress.txt $RESULTDIR
mv $DIRPATH/datafileSAVE $DIRPATH/$DATAFILENAME
