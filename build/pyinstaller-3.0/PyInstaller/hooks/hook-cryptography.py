#-----------------------------------------------------------------------------
# Copyright (c) 2005-2015, PyInstaller Development Team.
#
# Distributed under the terms of the GNU General Public License with exception
# for distributing bootloader.
#
# The full license is in the file COPYING.txt, distributed with this software.
#-----------------------------------------------------------------------------


"""
Hook for cryptography module from the Python Cryptography Authority.
"""

import os.path
import glob

from PyInstaller.compat import EXTENSION_SUFFIXES
from PyInstaller.utils.hooks import collect_submodules, get_module_file_attribute

# Add the OpenSSL FFI binding modules as hidden imports
hiddenimports = collect_submodules('cryptography.hazmat.bindings.openssl')


# Include the cffi extensions as binaries in a subfolder named like the package.
# The cffi verifier expects to find them inside the package directory for
# the main module. We cannot use hiddenimports because that would add the modules
# outside the package.
binaries = []
cryptography_dir = os.path.dirname(get_module_file_attribute('cryptography'))
for ext in EXTENSION_SUFFIXES:
    ffimods = glob.glob(os.path.join(cryptography_dir, '*_cffi_*%s*' % ext))
    for f in ffimods:
        binaries.append((f, 'cryptography'))
