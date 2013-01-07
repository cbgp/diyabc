#!/bin/bash

packageDir="/Users/user/lastDiyabc"
diyabc_git="/Users/user/adocs/diyabc"

rm -rf $packageDir
cd $diyabc_git

git checkout .
git pull --rebase 
make clean
make 
cd $diyabc_git/src-JMC-C++/
# modification de l'executable
install_name_tool -change /opt/local/lib/gcc47/libstdc++.6.dylib  @executable_path/lib/libstdc++.6.dylib general
install_name_tool -change /opt/local/lib/gcc47/libgomp.1.dylib  @executable_path/lib/libgomp.1.dylib general
install_name_tool -change /usr/lib/libSystem.B.dylib  @executable_path/lib/libSystem.B.dylib general
install_name_tool -change /opt/local/lib/gcc47/libgcc_s.1.dylib  @executable_path/lib/libgcc_s.1.dylib general
# modification du lien propre des libs
install_name_tool -id @executable_path/lib/libgomp.1.dylib ./lib/libgomp.1.dylib
install_name_tool -id @executable_path/lib/libgcc_s.1.dylib ./lib/libgcc_s.1.dylib
install_name_tool -id @executable_path/lib/libSystem.B.dylib ./lib/libSystem.B.dylib
install_name_tool -id @executable_path/lib/libstdc++.6.dylib ./lib/libstdc++.6.dylib
install_name_tool -id @executable_path/lib/libmathCommon.A.dylib ./lib/libmathCommon.A.dylib
# exemple de modification des liens entre librairies (deja fait auparavant)
#install_name_tool -change /usr/lib/libSystem.B.dylib @executable_path/lib/libSystem.B.dylib
cd $diyabc_git


cp $diyabc_git/src-JMC-C++/general $diyabc_git/python_interface/docs/executables/diyabc-comput-mac-i386

cd $diyabc_git/python_interface/docs/project_builders/

./mac_generation.sh ./pyinstaller_1355/pyinstaller.py ../icons/coccicon.icns $packageDir  ../../diyabc.py



