#!/bin/bash

function printUsage(){
echo "usage : 
build_deb_bin_32_only.sh  (-m | --makefile) path_to_makefile (-v | --version)  path_to_version.txt 

You have to give all the parameters

"
}

ARGS=$(getopt -o m:v: -l "makefile:version:" -n "build_deb_bin_32_only.sh" -- "$@");

#Bad arguments
if [ $? -ne 0 ] || [ $# -eq 0 ];
then
      printUsage
    exit
fi
MAKEFLAG=0
VERSIONFLAG=0
eval set -- "$ARGS";

while true; do
  case "$1" in
    -m|--makefile)
      shift;
      if [ -n "$1" ]; then
        if [ -f "$1" ]; then
            MAKEPATH=$1
            MAKEFLAG=1
        else
            echo "Makefile does not exist"
            exit
        fi
        shift;
      fi
      ;;
    -v|--version)
      shift;
      if [ -n "$1" ]; then
        if [ -f "$1" ]; then
            VERSIONFILE=$1
            VERSIONFLAG=1
        else
            echo "Version file does not exist"
            exit
        fi
        shift;
      fi
      ;;
    --)
      shift;
      break;
      ;;
  esac
done

if [ $MAKEFLAG == 0 ] && [ $VERSIONFLAG == 0 ]; then
    printUsage
    exit
fi

fakeroot ./build_deb_bin.sh -m $1 -v $2 -a i386
mkdir lastDiyabc
mv *i386*deb lastDiyabc
