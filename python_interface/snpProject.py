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
from utils.data import DataSnp
from datetime import datetime 
import os.path
import output
from output import log
import tempfile
import re

class SnpProject(Project):
    """ classe qui représente un projet de simulation
    """
    def __init__(self,name,dir=None,parent=None):
        super(SnpProject,self).__init__(name,dir,parent)


        self.gen_data_win = SetGeneticDataSimulation(self)
        self.gen_data_win.hide()

    def setGenetic(self):
        """ passe sur l'onglet correspondant
        """
        log(1,"NYI")
        #self.ui.refTableStack.addWidget(self.locusNumberFrame)
        #self.ui.refTableStack.setCurrentWidget(self.locusNumberFrame)
        #self.sexRatio = None
        #self.setGenValid(False)

    def returnToMe(self):
        self.ui.refTableStack.removeWidget(self.ui.refTableStack.currentWidget())
        self.ui.refTableStack.setCurrentIndex(0)

    def loadMyConf(self):
        """ lit le fichier conf.tmp pour charger le fichier de données
        """
        log(2,"Reading '%s' to load datafile"%self.parent.main_conf_name)
        if os.path.exists(self.ui.dirEdit.text()+"/%s"%self.parent.main_conf_name):
            f = open("%s/%s"%(self.dir,self.parent.main_conf_name),"r")
            lines = f.readlines()
            self.dataFileName = lines[0].strip()
            self.ui.dataFileEdit.setText(lines[0].strip())
            # lecture du dataFile pour les infos de Gui Projet
            if self.loadDataFile("%s/%s"%(self.dir,lines[0].strip())):
                return True
            ## comme on a lu le datafile, on peut remplir le tableau de locus dans setGeneticData
            #self.gen_data_win.fillLocusTableFromData()
        return False

    def loadDataFile(self,name):
        """ Charge le fichier de données passé en paramètre. Cette fonction est appelée lors
        de l'ouverture d'un projet existant et lors du choix du fichier de données pour un nouveau projet
        """
        log(2,"Loading datafile '%s'"%name)

        try:
            self.data = DataSnp(name)
            typestr = ""
            for ty in self.data.ntypeloc.keys():
                typestr += " %s : %s,"%(ty,self.data.ntypeloc[ty])
            typestr = typestr[:-1]
            self.ui.dataFileInfoLabel.setText("%s loci (SNP)\n%s individuals in %s samples\n%s" % (self.data.nloc,self.data.nindtot,self.data.nsample,typestr))
            self.ui.dataFileEdit.setText(name)
            self.dataFileSource = name
            self.ui.browseDirButton.setDisabled(False)

        except Exception,e:
            keep = ""
            if self.ui.dataFileEdit.text() != "":
                #keep = "\n\nKeeping previous selected file"
                keep = "\n\nThe file was not loaded, nothing was changed"
            output.notify(self,"Data file error","%s%s"%(e,keep))
            return False
        return True

    def save(self):
        """ sauvegarde du projet -> mainconf, histconf, genconf, theadconf
        Si le gen et hist sont valides, on génère le header
        """
        #print "je me save"
        log(2,"Saving project '%s'"%self.dir)
        self.parent.showStatus("Saving project %s"%self.name)

        if self.dir != None and self.dataFileName != "":
            # save meta project
            if os.path.exists(self.dir+"/%s"%self.parent.main_conf_name):
                os.remove("%s/%s"%(self.dir,self.parent.main_conf_name))

            f = codecs.open(self.dir+"/%s"%self.parent.main_conf_name,'w',"utf-8")
            f.write("%s\n"%self.dataFileName)
            # recup du nombre de params (depuis historical model et les mutational qui varient)
            nb_param = self.hist_model_win.getNbParam()
            #nb_param += self.gen_data_win.getNbParam()
            #nb_sum_stats = self.gen_data_win.getNbSumStats()
            f.write("%s parameters and %s summary statistics\n\n"%(nb_param,0))
            f.close()

            # save hist conf
            self.hist_model_win.writeHistoricalConfFromGui()
            # save gen conf
            #self.gen_data_win.writeGeneticConfFromGui()
            # save th conf et production du reftable header
            #if self.gen_state_valid and self.hist_state_valid:
            #    self.writeThConf()
            #    self.writeRefTableHeader()
            self.saveAnalysis()
            self.parent.clearStatus()
            self.parent.showStatus("Project %s successfully saved"%self.name,2000)
        else:
            output.notify(self,"Saving is impossible","Project %s is not yet completly created"%self.name)
            self.parent.clearStatus()
