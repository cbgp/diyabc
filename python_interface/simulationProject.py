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

        self.sexRatio = None

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
        QObject.connect(self.locusNumberFrame.mxEdit,SIGNAL("textChanged(QString)"),self.checkXYValues)
        QObject.connect(self.locusNumberFrame.myEdit,SIGNAL("textChanged(QString)"),self.checkXYValues)
        QObject.connect(self.locusNumberFrame.sxEdit,SIGNAL("textChanged(QString)"),self.checkXYValues)
        QObject.connect(self.locusNumberFrame.syEdit,SIGNAL("textChanged(QString)"),self.checkXYValues)

        self.gen_data_win = SetGeneticDataSimulation(self)
        self.gen_data_win.hide()

        #self.setHistValid(False)
        #self.setGenValid(False)
        #self.connect(self.ui.runReftableButton, SIGNAL("clicked()"),self.runSimulation)

    def checkXYValues(self,val):
        try:
            mx = int(str(self.locusNumberFrame.mxEdit.text()))
            my = int(str(self.locusNumberFrame.myEdit.text()))
            sx = int(str(self.locusNumberFrame.sxEdit.text()))
            sy = int(str(self.locusNumberFrame.syEdit.text()))
            if mx <= 0 and my <= 0 and sx <= 0 and sy <= 0:
                self.locusNumberFrame.sexRatioEdit.setDisabled(True)
            else:
                self.locusNumberFrame.sexRatioEdit.setDisabled(False)
        except Exception,e:
            self.locusNumberFrame.sexRatioEdit.setDisabled(True)


    def save(self):
        pass

    def setGenetic(self):
        """ passe sur l'onglet correspondant
        """
        log(1,"Entering in Genetic Data Setting")
        self.ui.refTableStack.addWidget(self.locusNumberFrame)
        self.ui.refTableStack.setCurrentWidget(self.locusNumberFrame)
        self.sexRatio = None
        self.setGenValid(False)

    def checkSampleNSetGenetic(self):
        dico_loc_nb = {}
        editList = self.locusNumberFrame.findChildren(QLineEdit)
        editList.remove(self.locusNumberFrame.sexRatioEdit)
        nbXY = 0
        nbpos = 0
        try:
            for le in editList:
                dico_loc_nb[str(le.objectName())[:2]] = int(le.text())
                if int(le.text()) < 0:
                    output.notify(self,"Number of sample error","Number of sample must be positive integers")
                    return
                elif int(le.text()) > 0:
                    nbpos += 1
                    if "x" in str(le.objectName())[:2] or "y" in str(le.objectName())[:2]:
                        nbXY += 1
            if nbpos == 0:
                output.notify(self,"Number of sample error","You must have at leat one sample")
                return
        except Exception,e:
            output.notify(self,"Number of sample error","Input error : \n%s"%e)
            return
        if nbXY > 0:
            try:
                val = float(str(self.locusNumberFrame.sexRatioEdit.text()))
                if val < 0 or val > 100:
                    output.notify(self,"Value error","Sex ratio value must be in [0,1]")
                    return
                self.sexRatio = val
            except Exception,e:
                output.notify(self,"Value error","Sex ratio value must be in [0,1]")
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
            # première ligne
            sexRatioTxt = ""
            if self.sexRatio != None:
                sexRatioTxt = self.sexRatio
            nb_rec_edit = str(self.ui.nbSetsReqEdit.text())
            if nb_rec_edit.isdigit() and int(nb_rec_edit) > 0:
                nb_rec = int(nb_rec_edit)
            else:
                output.notify(self,"Value error","Required number of simulated data sets must be a positive integer")
                return
            print "%s %s %s"%(self.name,nb_rec,sexRatioTxt)
            print self.hist_model_win.getConf()
            print ""
            print self.gen_data_win.getConf().replace(u'\xb5','u')
            fdest = open("%s/headersim.txt"%self.dir,"w")
            fdest.write("%s %s %s\n"%(self.name,nb_rec,sexRatioTxt))
            fdest.write("%s\n\n"%self.hist_model_win.getConf())
            fdest.write("%s"%self.gen_data_win.getConf().replace(u'\xb5','u'))
            fdest.close()




