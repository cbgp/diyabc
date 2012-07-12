#!/bin/bash

function printUsage(){
echo "usage : 
build_deb_diyabc-git.sh  version_file [clean_flag]
"
}

if [ $# -eq 0 ] ; then
    printUsage
    exit
fi
if [ $# -eq 2 ] ; then
    clean=false
else
    clean=true
fi



VERSIONFILE=$1

VERSION="`head -n 1 $VERSIONFILE`"
BUILDDATE=`LANG=en_EN.utf8 date +%d-%m-%Y`


# template copy 
echo $PACKAGEDIR
cp -rp diyabc-git/ $PACKAGEDIR

# control file edition
sed -i "s/Version: X/Version: $VERSION/" $PACKAGEDIR/DEBIAN/control

chmod 755 diyabc-git/DEBIAN/*

# version modification
sed -i "s/VVERSION/$VERSION/" $PACKAGEDIR/usr/local/share/menu/diyabc
sed -i "s/VVERSION/$VERSION/" $PACKAGEDIR/usr/local/share/applications/diyabc.desktop
    sed -i "s/ NAMEVERSION//" $PACKAGEDIR/usr/local/share/menu/diyabc
    sed -i "s/ICON/\/usr\/local\/src\/diyabc\/docs\/icons\/coccicon.png/" $PACKAGEDIR/usr/share/applications/diyabc-git.desktop
    sed -i "s/ICON/\/usr\/local\/src\/diyabc\/docs\/icons\/coccicon.png/" $PACKAGEDIR/usr/share/applications/diyabc-git-update.desktop    
    sed -i "s/ICON/\/usr\/local\/src\/diyabc\/docs\/icons\/coccicon.png/" $PACKAGEDIR/usr/share/applications/diyabc-git-zip-package.desktop    
    
    sed -i "s/ NAMEVERSION//" $PACKAGEDIR/usr/local/share/applications/diyabc-git.desktop
    sed -i "s/ NAMEVERSION//" $PACKAGEDIR/usr/local/share/applications/diyabc-git-update.desktop    
    sed -i "s/ NAMEVERSION//" $PACKAGEDIR/usr/local/share/applications/diyabc-git-package.desktop    
    
    sed -i "s/DIYABCEXEC/\/usr\/local\/bin\/diyabc-git/" $PACKAGEDIR/usr/local/share/applications/diyabc-git.desktop
    sed -i "s/DIYABCEXEC/\/usr\/local\/bin\/diyabc-git-update/" $PACKAGEDIR/usr/local/share/applications/diyabc-git-update.desktop  
    sed -i "s/DIYABCEXEC/\/usr\/local\/bin\/diyabc-git-zip-package/" $PACKAGEDIR/usr/local/share/applications/diyabc-git-zip-package.desktop     
     
    sed -i "s/COMMANDVERSION//" $PACKAGEDIR/usr/local/share/menu/diyabc-git
    sed -i "s/COMMANDVERSION//" $PACKAGEDIR/usr/local/share/menu/diyabc-git-update  
    sed -i "s/COMMANDVERSION//" $PACKAGEDIR/usr/local/share/menu/diyabc-git-zip-package  

    # change owner:group to root
    chmod +x $PACKAGEDIR/usr/local/bin/diyabc-git
    chmod +x $PACKAGEDIR/usr/local/bin/diyabc-git-update
    chmod +x $PACKAGEDIR/usr/local/bin/diyabc-git-zip-package
    chmod +x $PACKAGEDIR/usr/local/bin/diyabc-git-init          


cp -r $PACKAGEDIR /tmp
chown -R root:root /tmp/$PACKAGEDIR

dpkg-deb -b /tmp/$PACKAGEDIR
mv /tmp/$PACKAGEDIR.deb ./
if $clean; then
    rm -rf /tmp/$PACKAGEDIR $PACKAGEDIR
else
    echo "The package directory was not removed"
fi

