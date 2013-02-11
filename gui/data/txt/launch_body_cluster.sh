
set -o nounset
set -o errexit

#====== functions ========

function progress(){
    sum=0
    jobsTot=$1
    # for each log file, if it exists, do the sum of done values
    for i in $(seq 1 $jobsTot); do
        if [ -e reftable_$i.log ]; then
            let sum=$sum+`head -n 2 reftable_$i.log | tail -n 1`
        fi
    done
    pct=$(( $sum * 100 / $numSimulatedDataSet ))
    echo `nbOk $jobsTot` "/$jobsTot finished $pct% (total : $sum)"
}

function nbOk(){
    nb=0
    # for each log file, check if the computation is terminated
    for jobNum in $(seq 1 $1); do
        if [ -e reftable_$jobNum.log ]; then
            numDone=`head -n 2 reftable_$jobNum.log | tail -n 1`
	        if [ $jobNum -ne $numJobs ]
                    then 
                    if [ $numDone -ge $numSimulatedDataSetByJob ]
                        then
                        let nb=$nb+1
                    fi
                else
                    # case of last computation
                    if [ $numDone -ge $numSimulatedDataSetLastJob ]
                        then
                        let nb=$nb+1
                    fi
            fi
        fi
    done
    echo $nb
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


function submitJobs(){
    # submit jobs to the cluster scheduller
    for jobNum in $(seq 1 $numJobs); do 

        numSimulatedDataSetForThisJob=$numSimulatedDataSetByJob
        if [ "$jobNum" -eq "$numJobs" ]
            then
            numSimulatedDataSetForThisJob=$numSimulatedDataSetLastJob
        fi
        
        ############################## EDIT #########################################
        ######## Modify this line to submit jobs to your cluster scheduler ###############
        # node.sh arguments
        ##1 DIYABCPATH
        ##2 NBTOGEN
        ##3 USERDIR
        ##4 MYNUMBER
        ##5 DATAFILE
        queueName="mem.q"
        MPEnv=""
        if [ "$coresPerJob" -gt 1 ]
            then
            MPEnv="-pe SMP $coresPerJob"
        fi
        
        cmd="qsub -N n${jobNum}_$projectName -q $queueName $MPEnv -cwd node.sh ""'""$diyabcPath""'"" $coresPerJob $numSimulatedDataSetForThisJob ""'""`pwd`""'"" $jobNum  ""'""$PWD/$dataFileName""'"
        echo $cmd
        qsub -N n${jobNum}_$projectName -q $queueName $MPEnv -cwd node.sh "$diyabcPath" $coresPerJob $numSimulatedDataSetForThisJob "$PWD" $jobNum  "$PWD/$dataFileName"
        ############################## END EDIT #####################################
        if [ $? -eq "0" ] 
            then 
            touch runningJobs.lock
        fi 

    done
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
        echo -e "/!\/!\      Becarefull       /!\/!\ "
        echo -e "/!\/!\  `ls -1 | grep RNG_state  | wc -l` old RNG files found. Let's use them instead of generate $maxConcurrentJobs new ones /!\ /!\ " 
        echo -e "/!\/!\  be sure the RNG's have enough cores /!\ /!\ "                
    fi
    submitJobs
fi


while ! [ "`nbOk $numJobs`" -ge "$numJobs" ]; do
    echo `progress $numJobs`
    sleep 30
done
echo `progress $numJobs`



echo "Concatenating reftables :"
cmd="./general -p \\"$PWD/\\" -q  &> concat.out"
echo $cmd
./general -p "$PWD/" -q  &> concat.out

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

