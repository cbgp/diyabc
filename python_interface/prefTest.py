# -*- coding: utf-8 -*-

## @package python_interface.preferences
# @author Julien Veyssier
#
# @brief Fenêtre pour gérer les préférences personnelles

import os,sys,platform,multiprocessing
import ConfigParser
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.preferences_ui import Ui_MainWindow
from mutationModel.setMutationModelMsat import SetMutationModelMsat
from mutationModel.setMutationModelSequences import SetMutationModelSequences
import output
import utils.cbgpUtils as utilsPack
from utils.cbgpUtils import log
from prefLib import GenericPreferences

## @class Preferences
# @brief Fenêtre pour gérer les préférences personnelles
class Preferences(GenericPreferences):
    """ Classe principale qui est aussi la fenêtre principale de la GUI
    """
    def __init__(self,parent=None):
        super(Preferences,self).__init__(parent)

        self.tabColor = {"green": "#c3ffa6","blue":"#7373ff","red":"#ffb2a6","yellow":"#ffffb2","pink":"#ffbff2"}

        self.mutmodM = SetMutationModelMsat(self)
        self.mutmodS = SetMutationModelSequences(self)
        self.ui.tabWidget.addTab(self.mutmodM,"MM Microsats")
        self.ui.tabWidget.addTab(self.mutmodS,"MM Sequences")

        self.mutmodM.ui.frame_6.hide()
        self.mutmodM.ui.setMutMsLabel.setText("Default values for mutation model of Microsatellites")
        self.mutmodS.ui.frame_6.hide()
        self.mutmodS.ui.setMutSeqLabel.setText("Default values for mutation model of Sequences")

        self.addCategory("connexion")
        self.addCategory("various")

        self.styles = []
        for i in QStyleFactory.keys():
            self.styles.append(str(i))
            #self.ui.styleCombo.addItem(str(i))
        default = None
        if "Cleanlooks" in self.styles:
            default = "Cleanlooks"
        
        self.addPropCombo("various","style",{i:i for i in self.styles},self.styles,default)

        # initialisation du combo pour le nombre max de thread
        try:
            nb_core = multiprocessing.cpu_count()
        except Exception as e:
            nb_core = 1
            log(3,"Impossible to count core number on this computer")
        self.addPropCombo("various","maxThread",{i:i for i in range(1,nb_core)},range(1,nb_core),str(nb_core))

