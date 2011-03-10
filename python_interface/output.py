# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *

debug = True

def notify(parent,title,message):
    if debug:
        print message
    else:
        QMessageBox.information(parent,title,message)


