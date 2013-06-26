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

        self.ui.availableALabel.setText(str(self.parent.parent.sum_stat_wins.ui.availableALabel.text()))
        self.ui.availableHLabel.setText(str(self.parent.parent.sum_stat_wins.ui.availableHLabel.text()))
        self.ui.availableXLabel.setText(str(self.parent.parent.sum_stat_wins.ui.availableXLabel.text()))
        self.ui.availableYLabel.setText(str(self.parent.parent.sum_stat_wins.ui.availableYLabel.text()))
        self.ui.availableMLabel.setText(str(self.parent.parent.sum_stat_wins.ui.availableMLabel.text()))
        
        self.ui.takenAEdit.setText(str(self.parent.parent.sum_stat_wins.ui.takenAEdit.text()))
        self.ui.takenHEdit.setText(str(self.parent.parent.sum_stat_wins.ui.takenHEdit.text()))
        self.ui.takenXEdit.setText(str(self.parent.parent.sum_stat_wins.ui.takenXEdit.text()))
        self.ui.takenYEdit.setText(str(self.parent.parent.sum_stat_wins.ui.takenYEdit.text()))
        self.ui.takenMEdit.setText(str(self.parent.parent.sum_stat_wins.ui.takenMEdit.text()))
               
        self.ui.fromEdit.setText(str(self.parent.parent.sum_stat_wins.ui.fromEdit.text()))

    def exit(self):
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentWidget(self.stackTarget)
        # maj le dico de sum stats de l'analyse
        self.stackTarget.analysis.sumStatsTHDico[self.numGroup] = self.getSumStatsTableHeader(self.getGroupNumero())
        self.stackTarget.analysis.sumStatsConfDico[self.numGroup] = self.getSumConf()
        #print self.stackTarget.analysis.sumStatsDico

    def getGroupNumero(self):
        # there is only one group for SNP
        return 1

