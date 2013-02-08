#!/bin/bash

function printUsage(){
echo "usage : 
build_all.sh  -a | --arch [amd64 | i386]

"
}

ARGS=$(getopt -o a: -l "arch:" -n "build_all.sh" -- "$@");

#Bad arguments
if [ $? -ne 0 ] || [ $# -eq 0 ];
then
      printUsage
    exit
fi
eval set -- "$ARGS";

while true; do
  case "$1" in
    -a|--arch)
      shift;
      if [ -n "$1" ]; then
        if [ "$1" != "amd64" ] && [ "$1" != "i386" ]; then
            echo "invalid architecture"
            exit
        fi
        ARCH=$1
        shift;
      fi
      ;;
    --)
      shift;
      break;
      ;;
  esac
done

rm *.deb

fakeroot ./build_deb_bin.sh -m ../../Makefile -v ../../gui/version.txt -a $ARCH

fakeroot ./build_deb_doc.sh -t ../../Notice-DIYABC-v2/Notice_DIYABC_principal.tex -g ../../doc/gen_html_doc.sh -v ../../gui/version.txt

fakeroot ./build_deb_gui.sh -s ../../gui/src/

fakeroot ./build_deb_virtual.sh -v ../../gui/version.txt

rm -r ./lastDiyabc
mkdir lastDiyabc
mv *.deb ./lastDiyabc
