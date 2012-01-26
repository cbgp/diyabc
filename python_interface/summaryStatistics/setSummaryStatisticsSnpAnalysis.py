# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
#from uis.setSummaryStatisticsMsat_ui import Ui_Frame
from setSummaryStatisticsSnp import SetSummaryStatisticsSnp

class SetSummaryStatisticsSnpAnalysis(SetSummaryStatisticsSnp):
    """ ecran de selection des summary statistics pour les SNP dans le cadre d'une
    analyse de model checking
    """
    def __init__(self,parent=None,stackTarget=None,numGroup=None):
        super(SetSummaryStatisticsSnpAnalysis,self).__init__(parent,numGroup=numGroup)
        self.stackTarget = stackTarget

        self.ui.clearButton.hide()
        self.ui.frame_2.setDisabled(True)

        self.ui.availableEdit.setText(str(self.parent.parent.sum_stat_wins[numGroup].ui.availableEdit.text()))
        self.ui.takenEdit.setText(str(self.parent.parent.sum_stat_wins[numGroup].ui.takenEdit.text()))
        self.ui.fromEdit.setText(str(self.parent.parent.sum_stat_wins[numGroup].ui.fromEdit.text()))

    def exit(self):
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentWidget(self.stackTarget)
        # maj le dico de sum stats de l'analyse
        self.stackTarget.analysis.sumStatsTHDico[self.numGroup] = self.getSumStatsTableHeader(self.getGroupNumero())
        self.stackTarget.analysis.sumStatsConfDico[self.numGroup] = self.getStats()
        #print self.stackTarget.analysis.sumStatsDico

    def getGroupNumero(self):
        allTypes = self.parent.parent.typesOrdered
        presentTypes = self.parent.parent.sum_stat_wins.keys()
        myType = self.numGroup
        num = 1
        for t in allTypes:
            if t == myType:
                return num
            if t in presentTypes:
                num += 1

