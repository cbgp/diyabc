# -*- coding: utf-8 -*-

import hashlib,pickle,array,sys
import socket
from socket import *
import time
import os
import shutil
import codecs
import subprocess
import tarfile,stat
from project import Project
#from subprocess import Popen, PIPE, STDOUT 
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui,QtCore
from PyQt4 import uic
#from uis.project_ui import *
from setHistoricalModelSimulation import SetHistoricalModelSimulation
from setGenDataSimulation import SetGeneticDataSimulation
#from mutationModel.setMutationModelMsat import SetMutationModelMsat
#from mutationModel.setMutationModelSequences import SetMutationModelSequences
#from summaryStatistics.setSummaryStatisticsMsat import SetSummaryStatisticsMsat
#from summaryStatistics.setSummaryStatisticsSeq import SetSummaryStatisticsSeq
from utils.data import Data
from datetime import datetime 
import os.path
import output
from output import log
import tempfile

class SimulationProject(Project):
    """ classe qui représente un projet de simulation
    """
    def __init__(self,name,dir=None,parent=None):
        super(SimulationProject,self).__init__(name,dir,parent)

        self.ui.projNameLabel.setText("Data file generic name :")
        self.ui.label.setText("Target directory :")
        self.ui.dirEdit.setText("%s"%self.dir)
        self.ui.label_4.hide()
        self.ui.browseDataFileButton.hide()
        self.ui.dataFileEdit.hide()
        self.ui.label_10.hide()
        self.ui.dataFileInfoLabel.hide()

        self.ui.label_8.hide()
        self.ui.nbSetsDoneEdit.hide()

        self.ui.groupBox_6.show()
        self.ui.groupBox_7.show()
        self.ui.groupBox_8.show()

        self.ui.removeTab(1)
        self.ui.setTabText(0,QString("Simulate data sets"))

        self.ui.setHistoricalButton.setDisabled(False)
        self.ui.setGeneticButton.setDisabled(False)

        self.hist_model_win = SetHistoricalModelSimulation(self)
        self.hist_model_win.hide()

        self.locusNumberFrame = uic.loadUi("uis/setLocusNumber.ui")
        self.locusNumberFrame.parent = self
        QObject.connect(self.locusNumberFrame.okButton,SIGNAL("clicked()"),self.checkSampleNSetGenetic)

        self.gen_data_win = SetGeneticDataSimulation(self)
        self.gen_data_win.hide()

        #self.setHistValid(False)
        #self.setGenValid(False)
        #self.connect(self.ui.runReftableButton, SIGNAL("clicked()"),self.runSimulation)

    def save(self):
        pass

    def setGenetic(self):
        """ passe sur l'onglet correspondant
        """
        log(1,"Entering in Genetic Data Setting")
        self.ui.refTableStack.addWidget(self.locusNumberFrame)
        self.ui.refTableStack.setCurrentWidget(self.locusNumberFrame)
        self.setGenValid(False)

    def checkSampleNSetGenetic(self):
        dico_loc_nb = {}
        editList = self.locusNumberFrame.findChildren(QLineEdit)
        nbpos = 0
        try:
            for le in editList:
                dico_loc_nb[str(le.objectName())[:2]] = int(le.text())
                if int(le.text()) < 0:
                    output.notify(self,"Number of sample error","Number of sample must be positive integers")
                    return
                elif int(le.text()) > 0:
                    nbpos += 1
            if nbpos == 0:
                output.notify(self,"Number of sample error","You must have at leat one sample")
                return
        except Exception,e:
            output.notify(self,"Number of sample error","Input error : \n%s"%e)
            return
        log(3,"Numbers of locus : %s"%dico_loc_nb)
        # on vide les gen data
        self.gen_data_win = SetGeneticDataSimulation(self)
        self.gen_data_win.hide()
        self.gen_data_win.fillLocusTable(dico_loc_nb)

        self.ui.refTableStack.removeWidget(self.ui.refTableStack.currentWidget())
        self.ui.refTableStack.addWidget(self.gen_data_win)
        self.ui.refTableStack.setCurrentWidget(self.gen_data_win)


    def returnToMe(self):
        self.ui.refTableStack.removeWidget(self.ui.refTableStack.currentWidget())
        self.ui.refTableStack.setCurrentIndex(0)

    @pyqtSignature("")
    def on_btnStart_clicked(self):
            #if self.verifyRefTableValid():
            print self.hist_model_win.getConf()
            print ""
            print self.gen_data_win.getConf().replace(u'\xb5','u')



