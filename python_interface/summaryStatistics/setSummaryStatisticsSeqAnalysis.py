# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from setSummaryStatisticsSeq import SetSummaryStatisticsSeq


class SetSummaryStatisticsSeqAnalysis(SetSummaryStatisticsSeq):
    """ ecran de selection des summary statistics pour les sequences dans le cadre d'une
    analyse de model checking
    """
    def __init__(self,parent=None,stackTarget=None,numGroup=0):
        super(SetSummaryStatisticsSeqAnalysis,self).__init__(parent,numGroup=numGroup)
        self.stackTarget = stackTarget

        self.ui.clearButton.hide()

    def exit(self):
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentWidget(self.stackTarget)
        # maj le dico de sum stats de l'analyse
        self.stackTarget.analysis.sumStatsDico[self.numGroup] = self.getSumStatsTableHeader()
        #print self.stackTarget.analysis.sumStatsDico


