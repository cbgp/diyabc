#!/bin/bash

function printUsage(){
echo "usage : 
build_deb.sh (-s | --src) path_to_main_source_dir

"
}

ARGS=$(getopt -o s: -l "src:" -n "build_deb_gui.sh" -- "$@");

#Bad arguments
if [ $? -ne 0 ] || [ $# -eq 0 ];
then
      printUsage
    exit
fi
eval set -- "$ARGS";

while true; do
  case "$1" in
    -s|--src)
      shift;
      if [ -n "$1" ]; then
        if [ -d $1 ]; then
            SOURCEDIR=$1
        else
            echo "Directory does not exist"
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

VERSIONFILE="$SOURCEDIR/../version.txt"
VERSION="`head -n 1 $VERSIONFILE`"
BUILDDATE=`LANG=en_EN.utf8 date +%d-%m-%Y`

PACKAGEDIR=diyabc-gui\_$VERSION\_all

# template copy 
echo $PACKAGEDIR
cp -rp diyabc-gui-pkg-template/ $PACKAGEDIR

# control file edition
sed -i "s/Version: X/Version: $VERSION/" $PACKAGEDIR/DEBIAN/control
sed -i "s/>=X/>=$VERSION/g" $PACKAGEDIR/DEBIAN/control

mkdir -p $PACKAGEDIR/usr/share/icons/diyabc/
mkdir -p $PACKAGEDIR/usr/share/images/diyabc/
mkdir -p $PACKAGEDIR/usr/local/src/diyabc/

# for git errors
mkdir -p $PACKAGEDIR/usr/bin 
chmod 755 $PACKAGEDIR/DEBIAN/*

# file copy
cd $SOURCEDIR/..
./clean.sh
cd -
cp -rp $SOURCEDIR/*.py $SOURCEDIR/../clean.sh $SOURCEDIR/analysis $SOURCEDIR/uis $SOURCEDIR/../utils $SOURCEDIR/summaryStatistics $SOURCEDIR/mutationModel $SOURCEDIR/historicalModel $SOURCEDIR/geneticData $PACKAGEDIR/usr/local/src/diyabc/
chmod -R 755 $PACKAGEDIR/usr
chmod 644 $PACKAGEDIR/usr/local/src/diyabc/*.py $PACKAGEDIR/usr/local/src/diyabc/analysis/*.py $PACKAGEDIR/usr/local/src/diyabc/mutationModel/*.py $PACKAGEDIR/usr/local/src/diyabc/geneticData/*.py $PACKAGEDIR/usr/local/src/diyabc/historicalModel/*.py $PACKAGEDIR/usr/local/src/diyabc/summaryStatistics/*.py $PACKAGEDIR/usr/local/src/diyabc/utils/*.py $PACKAGEDIR/usr/local/src/diyabc/utils/*.sh $PACKAGEDIR/usr/local/src/diyabc/utils/configobj/*.py  $PACKAGEDIR/usr/local/src/diyabc/uis/* $PACKAGEDIR/usr/share/menu/diyabc-gui $PACKAGEDIR/usr/share/applications/diyabc.desktop $PACKAGEDIR/usr/share/doc/diyabc-gui/copyright
chmod 755 $PACKAGEDIR/usr/local/src/diyabc/utils/matplotlib_example.py $PACKAGEDIR/usr/local/src/diyabc/diyabc.py
# gestion du man
sed -i "s/BUILDDATE/$BUILDDATE/" $PACKAGEDIR/usr/share/man/man1/diyabc-gui.1
sed -i "s/VersionX/Version $VERSION/" $PACKAGEDIR/usr/share/man/man1/diyabc-gui.1
gzip -9 $PACKAGEDIR/usr/share/man/man1/diyabc-gui.1
chmod 644 $PACKAGEDIR/usr/share/man/man1/diyabc-gui.1.gz
# version modification
sed -i "s/VERSION='development version'/VERSION='$VERSION'/" $PACKAGEDIR/usr/local/src/diyabc/variables.py
sed -i "s/01\/01\/1970/$BUILDDATE/" $PACKAGEDIR/usr/local/src/diyabc/variables.py
sed -i "s/VVERSION/$VERSION/" $PACKAGEDIR/usr/share/menu/diyabc-gui
sed -i "s/VVERSION/$VERSION/" $PACKAGEDIR/usr/share/applications/diyabc.desktop
git log | head -n 100 > $PACKAGEDIR/usr/share/doc/diyabc-gui/changelog
gzip -9 $PACKAGEDIR/usr/share/doc/diyabc-gui/changelog
chmod 644 $PACKAGEDIR/usr/share/doc/diyabc-gui/changelog.gz

#sed -i "s/ICON/\/usr\/share\/icons\/diyabc\/coccicon.png/" $PACKAGEDIR/usr/share/applications/diyabc.desktop

mkdir -p $PACKAGEDIR/usr/share/diyabc/txt
cp -rp $SOURCEDIR/../data/txt/*  $PACKAGEDIR/usr/share/diyabc/txt/
cp -rp $SOURCEDIR/../data/images/* $PACKAGEDIR/usr/share/images/diyabc/
cp -r $SOURCEDIR/../data/icons/* $PACKAGEDIR/usr/share/icons/diyabc/
chmod 644 $PACKAGEDIR/usr/share/icons/diyabc/* $PACKAGEDIR/usr/share/images/diyabc/* $PACKAGEDIR/usr/share/diyabc/txt/*

# generation of the launch script placed in /usr/bin
echo "#!/bin/bash
cd /usr/local/src/diyabc/
python /usr/local/src/diyabc/diyabc.py \$@" > $PACKAGEDIR/usr/bin/diyabc-gui
chmod 755 $PACKAGEDIR/usr/bin/diyabc-gui
# change owner:group to root
#echo "chown -R 0:0 /usr/local/bin/diyabc /usr/local/src/diyabc /usr/share/applications/diyabc.desktop /usr/share/menu/diyabc" >> $PACKAGEDIR/DEBIAN/postinst
chmod +x $PACKAGEDIR/usr/bin/diyabc-gui

cp -r $PACKAGEDIR /tmp
cd /tmp/$PACKAGEDIR
#md5sum `find . -type f | awk '/.\// { print substr($0, 3) }'` >DEBIAN/md5sums
find . -type f ! -regex '.*.hg.*' ! -regex '.*?debian-binary.*' ! -regex '.*?DEBIAN.*' -printf '%P ' | xargs md5sum > DEBIAN/md5sums
cd -
chown -R root:root /tmp/$PACKAGEDIR

dpkg-deb -b /tmp/$PACKAGEDIR
# signature GPG
cd /tmp/
#ar x $PACKAGEDIR.deb
#cat debian-binary control.tar.gz data.tar.gz > /tmp/combined-contents
#gpg -abs --local-user diyabc -o _gpgorigin /tmp/combined-contents
#ar rc $PACKAGEDIR.deb  _gpgorigin debian-binary control.tar.gz data.tar.gz
#debsigs --sign=origin --default-key=9273791D $PACKAGEDIR.deb
#dpkg-sig -s builder -k 9273791D $PACKAGEDIR.deb
cd -

mv /tmp/$PACKAGEDIR.deb ./
if $clean; then
    rm -rf /tmp/$PACKAGEDIR $PACKAGEDIR /tmp/_gpgorigin
else
    echo "The package directory was not removed"
fi

