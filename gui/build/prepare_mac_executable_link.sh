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
