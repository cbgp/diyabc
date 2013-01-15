#!/bin/sh

# this shell launches a native command $1 from a temporary directory
# nammed by $PAS_TASK_ID env variable specified by ProActiveScheduler
# at Job submission

###if [ $# -lt 1 ]
###then
###echo "you must specify at least the command to launch"
###echo "Usage: launcher.sh executable_to_launch  [parameters...]"
###exit 1
###fi
###
###cd $(dirname $0)
###
###COMMAND=$1
###shift
###
###if [ -z $PAS_TASK_ID ]
###then
###echo "Error : environment var \$PAS_TASK_ID is not defined,"
###echo "Execution aborted"
###exit 1
###fi
###
###if [ -e $PAS_TASK_ID ]
###then
###echo "directory path exists !"
###echo "Execution aborted"
###
###exit 1
###fi
###
###mkdir $PAS_TASK_ID
###if [ $? -ne 0 ]
###then
###echo "directory $PAS_TASK_ID creation failed"
###echo "Execution aborted"
###exit 1
###fi
###
###cd $PAS_TASK_ID
###$COMMAND $@

hostn=`hostname`
cd $1
echo "`hostname`" >> output-$hostn-$2.txt
echo "$1" >> output-$hostn-$2.txt
cat ploup >>  output-$hostn-$2.txt
echo $3 >>  output-$hostn-$2.txt

#lftp -c "connect -u roger,rororo 147.99.65.180 ;  cd pro ; put output.txt -o $RANDOM.txt"
#echo "code erreur : $?"
#echo "gnia gnia" >> output.txt
#mv * ../$RANDOM.txt
#if [ $? == 0 ]
#then
#echo "execution of $COMMAND ok"
#else
#echo  "problem during execution of $COMMAND $@"
#fi
