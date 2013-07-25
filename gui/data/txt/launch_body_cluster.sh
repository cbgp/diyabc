queueStd="long.q"     # Your cluster standard queue
queuePEname="mp.q"   # Your cluster parallel queue, if this not make sense, just wright again your standart queue name
PEenv='SMP'           # Your cluster parallel environment


set -o nounset
set -o errexit

#====== functions ========

function submitJobs(){
    # submit jobs to the cluster scheduller
    for jobNum in $(seq 1 $numJobs);
        do
        numSimulatedDataSetForThisJob=$numSimulatedDataSetByJob
        if [ "$jobNum" -eq "$numJobs" ]
            then
            numSimulatedDataSetForThisJob=$numSimulatedDataSetLastJob
        fi

############################## EDIT #########################################
######## Modify those lines to submit jobs to your cluster scheduler ###############
# node.sh arguments
##1 DIYABCPATH
##2 NBCORES
##3 NBTOGEN
##4 USERDIR
##5 MYNUMBER
##6 DATAFILE
        MP=""
        queue=$queueStd
        if [ "$coresPerJob" -gt 1 ]
            then
            MP="-pe $PEenv $coresPerJob"
            queue=$queuePEname
        fi

        cmd="qsub -N n${jobNum}_$projectName -q $queue $MP -cwd node.sh ""'""$diyabcPath""'"" $coresPerJob $numSimulatedDataSetForThisJob ""'""`pwd`""'"" $jobNum  ""'""$PWD/$dataFileName""'"
        echo $cmd
        qsub -N n${jobNum}_$projectName -q $queue $MP -cwd node.sh "$diyabcPath" $coresPerJob $numSimulatedDataSetForThisJob "$PWD" $jobNum  "$PWD/$dataFileName"
############################## END EDIT #####################################
        if [ $? -eq "0" ]
            then
            touch runningJobs.lock
        fi
    done
}


function getRecordsDone(){
    refTableLogFile=$1
    triesNum=0
    triesAuthorized=20
    while [ "$triesNum"  -ne "$triesAuthorized" ]
        do
        set  +e
        records="`head -n 2 $refTableLogFile | tail -n 1`" 2> /dev/null
        if [ "$records" -ne "$records" ] 2>/dev/null
            then
            #set -e
            echo -e "Not found integer as record number in $refTableLogFile : $records"  >&2
            let triesNum=$triesNum+1
            sleep 1
            continue
        else 
            set -e     
            echo "$records" 
            return 0
        fi
    done
    set -e
    return 1
}



function progress(){
    jobsTot=$1
    sum=0
    nbFinished=0
    while [ "$nbFinished" -lt "$jobsTot" ]
        do
        sum=0
        nbFinished=0
    # for each log file, if it exists, do the sum of done values
        for jobNum in $(seq 1 $jobsTot)
            do
            if [ -e reftable_$jobNum.log ]; 
                then
                    numDone=0
                    numDone=$(getRecordsDone reftable_$jobNum.log)
	                if [ "$jobNum" -ne "$numJobs" ]
                        then 
                            if [ "$numDone" -ge "$numSimulatedDataSetByJob" ]
                                then
                                let nbFinished=$nbFinished+1
                            fi
                        else
                            # case of last computation
                            if [ "$numDone" -ge "$numSimulatedDataSetLastJob" ]
                                then
                                let nbFinished=$nbFinished+1
                            fi
                    fi
                    let sum=$sum+$numDone
            fi
        done
        pct=$(( $sum * 100 / $numSimulatedDataSet ))
        echo "$nbFinished/$jobsTot finished $pct% (total : $sum)"
        sleep 30
    done  
}


function testJobsAllReadylaunched(){
    # verify if the jobs are already submitted
    lockFilesList=(./*lock)
    reftableFilesList=(./*reftable*)
    if [ ${#lockFilesList[@]} -ne "0" ]  ||  [ ${#reftableFilesList[@]} -ne "0" ]
        then
        return 1
    else 
        return 0
    fi
}

function numRNGFiles(){
    # verify if the RNG's are allready generated
    RNGfilesList=(./*RNG_state*bin)
    return ${#RNGfilesList[@]} 
}

function testRNGallReadyGenerated(){
    # verify if the RNG's are allready generated
    RNGfilesList=(./*RNG_state*bin)
    if [ ${#RNGfilesList[@]} -ne "0" ]
        then
        return 1
    else 
        return 0
    fi
}

function generateRNGs(){
    # Generation of random numbers
    cmd="$diyabcPath -p ./ -n ""'""t:$coresPerJob;c:$maxConcurrentJobs;s:$seed""'"
    echo "Generation of random numbers :" 
    echo $cmd
    "$diyabcPath" -p ./ -n "t:$coresPerJob;c:$maxConcurrentJobs;s:$seed"
}


#===== script core =======

# job parameters
numSimulatedDataSetLastJob=$(( $numSimulatedDataSet % $numSimulatedDataSetByJob ))
if [ "$numSimulatedDataSetLastJob" -eq "0" ] 
    then 
    numJobs=$(( $numSimulatedDataSet / $numSimulatedDataSetByJob ))
    numSimulatedDataSetLastJob=$numSimulatedDataSetByJob
else 
    numJobs=$(( $numSimulatedDataSet / $numSimulatedDataSetByJob + 1 ))
fi

if [ "$seed" -lt "0" ] 
    then
    seed=$RANDOM
fi


if [ "$maxConcurrentJobs" -gt "$numJobs" ]
    then
    maxConcurrentJobs=$numJobs
fi

projectName=${projectName//[-._ \[\]\{\}]/}

# test if the jobs had allready been submitted
if [ `ls -1 | grep lock  | wc -l` -eq "0" ] 
    then 
    # test if the RNG had allready been generated
    if  [ `ls -1 | grep RNG_state  | wc -l` -eq "0" ] 
        then 
        generateRNGs
    else 
        echo -e "/!\/!\      BECAREFULL       /!\/!\ "
        sleep 2
        echo -e "/!\/!\  `ls -1 | grep RNG_state  | wc -l` old RNG files found. Let's use them instead of generate $maxConcurrentJobs new ones /!\ /!\ " 
        sleep 3
        echo -e "/!\/!\  be sure the RNG's have enough cores /!\ /!\ " 
        sleep 5       
    fi
    submitJobs
fi


progress $numJobs


echo "Concatenating reftables :"
cmd="$diyabcPath -p "$PWD/" -q  &> concat.out"
echo $cmd
$diyabcPath -p "$PWD/" -q  &> concat.out

if [ $? -eq "0" ] 
    then 
    echo "*************************************************************"
    echo "All the result files should have been concatenated into reftable.bin"
    echo "See concat.out output file "
    echo "*************************************************************"
    rm runningJobs.lock
fi 

#END
exit 0
