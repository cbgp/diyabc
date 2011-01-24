# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from setMutationModel_ui import Ui_Frame

class SetMutationModel(QFrame):
    def __init__(self,parent=None):
        super(SetMutationModel,self).__init__(parent)
        self.parent=parent
        self.createWidgets()

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

