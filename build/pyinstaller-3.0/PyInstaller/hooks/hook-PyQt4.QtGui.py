#-----------------------------------------------------------------------------
# Copyright (c) 2013, PyInstaller Development Team.
#
# Distributed under the terms of the GNU General Public License with exception
# for distributing bootloader.
#
# The full license is in the file COPYING.txt, distributed with this software.
#-----------------------------------------------------------------------------


hiddenimports = ['sip', 'PyQt4.QtCore', 'PyQt4._qt']

from PyInstaller.utils.hooks import qt4_plugins_binaries

binaries = []
binaries.extend(qt4_plugins_binaries('accessible'))
binaries.extend(qt4_plugins_binaries('iconengines'))
binaries.extend(qt4_plugins_binaries('imageformats'))
binaries.extend(qt4_plugins_binaries('inputmethods'))
binaries.extend(qt4_plugins_binaries('graphicssystems'))
