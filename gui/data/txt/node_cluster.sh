#!/bin/bash
set -o errexit
set -o errtrace
set -o nounset
#set -xv 
echo "host =  `hostname`"

# ===============================================
# check script parameters
function usage {
    echo -e "\tThis script runs DIYABC on a node cluster"
    echo -e "\tUsage : $0 <diyabcPath> <numberOfCores> <numToGenerate> <workingDirectory> <numIdOfTheCurrentJob> <dataFile>"
    echo -e "\texample : "
    echo -e "\t\t $0 \"/usr/local/bin/diyabc\" 1 5000 \"$PWD\" 13 \"dyabcData.mss\""
}

# Test  parameters number
if [ "$#" -lt 5 ]
    then
    echo -e "\nERROR :"
    echo -e "      Require 6  parameters, $# given\n\n"
    usage
    exit 2
fi

DIYABCPATH=$1
NBCORES=$2
NBTOGEN=$3
USERDIR=$4
MYNUMBER=$5
DATAFILE=$6

# Get full path for diyabc, dir and file
DIYABCPATH=`readlink -f "$DIYABCPATH"`
USERDIR=`readlink -f "$USERDIR"`                   
DATAFILE=`readlink -f "$DATAFILE"` 

if [ ! -r "$DIYABCPATH" ]; then
    echo -e "\nERROR :" >&2
    echo -e "      diyabc not found in $DIYABCPATH or you can not read it\n" >&2
    usage
    exit 3
fi

if [ ! -r "$DATAFILE" ]; then
    echo -e "\nERROR :" >&2
    echo -e "      You do not have rights to read $DATAFILE  but you should !\n" >&2
    usage
    exit 3
fi

if [ ! -w "$USERDIR" ]; then
    echo -e "\nERROR :"  >&2
    echo -e "      You do not have the right to write in $USERDIR but you should ! (or dir does)\n" >&2
    usage
    exit 3
fi

echo -e "All parameters accepted"
# ===============================================
# start working 

jobId="`hostname`-n${MYNUMBER}-pid${BASHPID}-${RANDOM}"

# initialisation values for trapActions
errStatus=""
rngFile=""
rngFileName=""
rngLockFile=""
rngFlagFile=""
rngBackupFile=""
scriptMadeTMPDIR="false"

# rollback actions if something fail 
function trapActions(){
    set +e
    set +u
    err=$?
    if [ "$errStatus" = "" ]
        then errStatus=$err
    fi
    echo -e "Job ERROR (trap) : `date`" >&2
    echo -e "Job $MYNUMBER ($jobId) exit with error : $errStatus -> rollback procedure started \n" >&2
    echo -e "\n=====  trap :  ls -al "$TMPDIR" :" >&2
    \ls -al "$TMPDIR" >&2
    echo -e "\n=====  trap :  reftable.txt    ====="  >&2
    \cat "$TMPDIR/reftable.log"  >&2
    echo -e "\n=====  trap :  statobs.txt    ====="  >&2
    \cat "$TMPDIR/statobs.txt"  >&2
    echo -e "\n=====  trap :   diyabc.out    ====="  >&2
    cat "$TMPDIR/diyabc.out"  >&2    
    echo -e "\n\n=====  Trap error : $1\n" >&2
    #rm -f "$rngFile"
    if [ -e "$rngBackupFile" ]; then mv  "$rngBackupFile" "$rngFile"  || echo -e "could not write back $rngFile" >&2 ; fi
    if [ $scriptMadeTMPDIR = "true" ]; then echo 'rm -rf "$TMPDIR"' || echo -e "could not erase $TMPDIR" >&2 ; fi
    for file in "$rngBackupFile" "$rngLockFile" "$rngFlagFile"  
        do
        if [ -e "$file" ]; then rm -f "$file" || echo -e "could not erase $file" >&2 ; fi
    done
    
    exit $errStatus
}

function sumStatsDone(){
    refTableLogFile=$1
    triesNum=0
    triesAuthorized=20
    #if  [ ! -f "$refTableLogFile" ]
    #    then 
    #    errStatus=100
    #    trapActions "`hostname` $0 : No $refTableLogFile found (sumStatsDone function)" 
    #fi
    while [ "$triesNum" -ne "$triesAuthorized" ]
        do
        set +e
        sumStats="`head -n 2 $refTableLogFile | tail -n 1`"
        if [ "$sumStats" -ne "$sumStats" ] 2>/dev/null
            then
            set -e
            echo -e "Not found integer as sum stats number in $refTableLogFile = $sumStats"  >&2
            let triesNum=$triesNum+1
            sleep 5
            continue
        else 
            set -e     
            #echo "$sumStats" >&2
            echo "$sumStats" 
            return 0
        fi
    done
    errStatus=101
    trapActions "`hostname` $0 : no way to found a sum stats integer in $refTableLogFile : $trieNum  tries."
}


# start trap if errors
trap 'trapActions' INT TERM EXIT


#create tmp dir if necessary
set +u
if test -z "$TMPDIR"
    then
	    TMPDIR="/tmp/tmpDiyabc_${jobId}"
	    scriptMadeTMPDIR="true"
fi
set -u

if ! [ -d "$TMPDIR" ]
    then 
        mkdir -p "$TMPDIR"  || (errStatus=1 ; trapActions "unable to create $TMPDIR")
fi

if [ ! -w "$TMPDIR" ]; then
    errStatus=5
    trapActions "You do not have the right to write in $TMPDIR but you should !"
fi


#Choosing one RNG file
rngNotFound=true 
rngFoundCounter=0
while [ $rngNotFound ]
    do 
    if [ "$rngFoundCounter" -eq "1000" ]
        then  
        errStatus=1
        trapActions "No way to found an unused RNG file in $USERDIR (1000 attempt)"
    fi
    rngList=("$USERDIR"/RNG_state*bin)
    index=$RANDOM
    index=$(( $index % ${#rngList[@]} ))
    # start race condition
    if [ -e "${rngList[$index]}" ]
        then
        set +e
        if  ( lockfile -r0 "${rngList[$index]}.lock") 2> /dev/null  
            then
            set -e
            rngFile="${rngList[$index]}"
            rngFlagFile="${rngFile}_`date +"%Y-%m-%d-%Hh%Mm%Ss"`_${jobId}"
            touch "$rngFlagFile"
            rngBackupFile="${rngFile}_backup" 
            cp "$rngFile" "$rngBackupFile"
            #cp "$rngFile" "$TMPDIR/`basename ${rngFile}`_$MYNUMBER"
            mv  "$rngFile" "$TMPDIR" 
            rngFileName=`basename "$rngFile"`     
            rngLockFile="${rngFile}.lock"          
            rngNotFound=false
            echo -e "Using RNG $rngFile "
            break
        fi
    fi
    set -e
    let rngFoundCounter=$rngFoundCounter+1  
    echo -e "$rngFoundCounter try to find an unused RNG file"
    sleep 2
done
#copy files to tmp dir
cp "$DIYABCPATH" "$TMPDIR/"
chmod +x "$TMPDIR/`basename $DIYABCPATH`"
cp header.txt "$TMPDIR/"
cp "$DATAFILE" "$TMPDIR/"

# Computations
rngNum=`echo ${rngFileName:10} | cut -d'.' -f1`
cmd="$TMPDIR/`basename $DIYABCPATH` -t $NBCORES -p $TMPDIR/ -w $rngNum -r $NBTOGEN "
echo -e "Running computations :" 
echo -e "$cmd"
"$TMPDIR/`basename $DIYABCPATH`" -t $NBCORES -p "$TMPDIR/" -w $rngNum -r $NBTOGEN &> "$TMPDIR/diyabc.out" &
cmdPID="$!"
echo $cmdPID > "$rngFlagFile"
sleep 30


while [ -d "/proc/$cmdPID" ]
    do
    set +e
    cp "$TMPDIR"/reftable.log "$USERDIR"/reftable_$MYNUMBER.log
    set -e
    sleep 20
done
cp "$TMPDIR"/reftable.log "$USERDIR"/reftable_$MYNUMBER.log

# Copy log file in user working directory till end of program 
while [ ! "`head -n 2 "$TMPDIR"/reftable.log | tail -n 1`" -ge $NBTOGEN -a -d "/proc/$cmdPID" ]; do
    cp "$TMPDIR"/reftable.log "$USERDIR"/reftable_$MYNUMBER.log
    sleep 10
done
if ! [ "$(sumStatsDone $TMPDIR/reftable.log)" -ge "$NBTOGEN" ]
    then
    echo -e "head -n 2 "$TMPDIR"/reftable.log | tail -n 1 = `head -n 2 "$TMPDIR"/reftable.log | tail -n 1`" >&2
    errStatus=1000
    echo "ERROR ERROR ERROR PID : $cmdPID: `ps -edf | grep $cmdPID`" >&2
    echo "ERROR ERROR ERROR ls /proc/$cmdPID: `ls /proc/$cmdPID`" >&2
    trapActions "program died before normal end : not enough records ($(sumStatsDone $TMPDIR/reftable.log)/$NBTOGEN)" 
fi

#copy results to USERDIR and clean
echo -e "computation finished with succes, bringing back results"
mv "$TMPDIR"/reftable.bin "$USERDIR"/reftable_$MYNUMBER.bin
mv "$TMPDIR"/reftable.log "$USERDIR"/reftable_$MYNUMBER.log
mv "$TMPDIR/$rngFileName" "$rngFile"
echo -e "cleaning temp files"
rm -f "$rngBackupFile" "$rngLockFile" "$rngFlagFile"
if [ $scriptMadeTMPDIR = 'true' ]; then rm -rf "$TMPDIR"; fi
echo -e "End with succes"
echo -e " "

trap - INT TERM EXIT

#END
exit 0

