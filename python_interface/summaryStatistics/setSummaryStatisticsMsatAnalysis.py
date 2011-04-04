# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from uis.setSummaryStatisticsMsat_ui import Ui_Frame
from setSummaryStatisticsMsat import SetSummaryStatisticsMsat

class SetSummaryStatisticsMsatAnalysis(SetSummaryStatisticsMsat):
    """ ecran de selection des summary statistics pour les microsats dans le cadre d'une
    analyse de model checking
    """
    def __init__(self,parent=None,stackTarget=None):
        super(SetSummaryStatisticsMsatAnalysis,self).__init__(parent)
        self.stackTarget = stackTarget

        self.ui.clearButton.hide()

    def exit(self):
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentWidget(self.stackTarget)
        # TODO : maj le dico de sum stats de l'analyse
        print "je sors de sumseqana"


