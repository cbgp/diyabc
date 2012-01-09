# -*- coding: utf-8 -*-

## @package python_interface.project
# @author Julien Veyssier
#
# @brief Classe mère abstraite de tous les projets

#from subprocess import Popen, PIPE, STDOUT 
import subprocess
import os
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
from historicalModel.setHistoricalModel import SetHistoricalModel
#from uis.viewTextFile_ui import Ui_Frame as ui_viewTextFile
from PyQt4.Qwt5 import *
from PyQt4.Qwt5.qplt import *
from utils.cbgpUtils import log,addLine
import dataPath

formProject,baseProject = uic.loadUiType("uis/Project.ui")

## @class Project
# @brief Classe mère abstraite de tous les projets
class Project(baseProject,formProject):
    """ classe qui représente un projet
    par defaut, un projet est considéré comme nouveau, cad que l'affichage est celui d'un projet vierge
    pour un projet chargé, on modifie l'affichage en conséquence dans loadFromDir
    """
    def __init__(self,name,dir=None,parent=None):
        self.parent=parent
        self.name=name
        self.dir=dir
        # objet data de cbgpUtils
        self.data = None
        self.hist_state_valid = False
        self.gen_state_valid = False

        # utile seulement si on derive de QTabWidget
        super(Project,self).__init__(parent)
        # on peut aussi instancier un tabwidget
        #self.tab = QTabWidget()

        self.createWidgets()

    def createWidgets(self):

        # instanciation du widget project_ui
        #self.ui = Ui_TabWidget()
        #self.ui.setupUi(self)
        self.setupUi(self)
        self.ui = self

        # desactivation des boutons
        self.ui.setHistoricalButton.setDisabled(True)
        self.ui.setGeneticButton.setDisabled(True)
        self.ui.runReftableButton.setDisabled(True)
        self.ui.browseDirButton.setDisabled(True)
        self.ui.newAnButton.setDisabled(True)
        self.ui.dataFileEdit.setReadOnly(True)
        self.ui.dirEdit.setReadOnly(True)
        #self.ui.groupBox.hide()
        self.ui.groupBox_6.hide()
        self.ui.groupBox_7.hide()
        self.ui.groupBox_8.hide()

        self.ui.browseDirButton.hide()
        #self.ui.groupBox.setVisible(False)

        self.ui.ascertSimFrame.hide()

        # creation des onglets "set ..."
        self.hist_model_win = SetHistoricalModel(self)
        self.hist_model_win.ui.okButton.setText("VALIDATE AND SAVE")
        self.hist_model_win.hide()

        self.ui.projNameLabelValue.setText(self.name)

        QObject.connect(self.ui.setHistoricalButton,SIGNAL("clicked()"),self.setHistorical)
        QObject.connect(self.ui.setGeneticButton,SIGNAL("clicked()"),self.setGenetic)
        #QObject.connect(self.ui.browseDataFileButton,SIGNAL("clicked()"),self.dataFileSelection)
        #QObject.connect(self.ui.browseDirButton,SIGNAL("clicked()"),self.dirSelection)

        # inserer image
        self.ui.setHistoricalButton.setIcon(QIcon(dataPath.DATAPATH+"/icons/redcross.png"))
        self.ui.setGeneticButton.setIcon(QIcon(dataPath.DATAPATH+"/icons/redcross.png"))

        self.setTabIcon(0,QIcon(dataPath.DATAPATH+"/icons/redcross.png"))
        self.setTabIcon(1,QIcon(dataPath.DATAPATH+"/icons/gnome-color-browser.png"))


        #self.connect(self.ui.cancelButton, SIGNAL("clicked()"),self.cancelTh)

        self.th = None

        self.ui.verticalLayout_9.setAlignment(Qt.AlignTop)

        ## remplissage du combo liste des analyses deja effectuées
        #self.fillAnalysisCombo()
        #QObject.connect(self.ui.analysisResultsButton,SIGNAL("clicked()"),self.viewAnalysisResult)

        self.stopUiGenReftable()

        self.ui.frame_11.hide()
        self.ui.frame_12.hide()

        self.ui.nbSetsDoneEdit.setStyleSheet("background-color: #DEF8FF")
        self.ui.dirEdit.setStyleSheet("background-color: #DEF8FF")
        self.ui.dataFileEdit.setStyleSheet("background-color: #DEF8FF")

    def initializeRNG(self):
        """ à lancer une fois que le dossier du projet a été créé
        """
        if os.path.exists("%s/RNG_state_0000.bin"%self.dir):
            os.remove("%s/RNG_state_0000.bin"%self.dir)
        executablePath = self.parent.preferences_win.getExecutablePath()
        nbMaxThread = self.parent.preferences_win.getMaxThreadNumber()
        cmd_args_list = [executablePath,"-p", "%s/"%self.dir, "-n", "t:%s"%nbMaxThread]
        log(3,"Command launched for initialization of RNGs of project '%s' : %s"%(self.name," ".join(cmd_args_list)))
        addLine("%s/command.txt"%self.dir,"Command launched for initialization of RNGs of project '%s' : %s\n\n"%(self.name," ".join(cmd_args_list)))
        outfile = "%s/init_rng.out"%(self.dir)
        f = open(outfile,"w")
        p = subprocess.call(cmd_args_list, stdout=f, stdin=subprocess.PIPE, stderr=subprocess.STDOUT)
        f.close()
        log(3,"Initialization of RNGs of project '%s' terminated with returncode : %s"%(self.name,p))

    def returnTo(self,elem):
        self.ui.analysisStack.removeWidget(self.drawAnalysisFrame)
        self.ui.analysisStack.setCurrentWidget(elem)

    def isSnp(self):
        return False

    def setHistorical(self):
        """ passe sur l'onglet correspondant
        """
        log(1,"Entering in Historical Model Setting")
        ## nécéssaire seulement sous windows
        ## on dirait que sous linux, le simple setCurrentWidget rend l'onglet enabled
        #self.setTabEnabled(self.count()-1,True)
        #self.setCurrentWidget(self.hist_model_win)
        self.ui.refTableStack.addWidget(self.hist_model_win)
        self.ui.refTableStack.setCurrentWidget(self.hist_model_win)
        self.setHistValid(False)

    def setNbScenarios(self,nb):
        """ ecrit le nombre de scenarios dans la zone "historical model"
        """
        self.ui.nbScLabel.setText(nb)

    def setNbParams(self,nb):
        """ écrit le nombre de paramètres dans la zone "historical model"
        """ 
        self.ui.nbHistParamLabel.setText(nb)

    def clearHistoricalModel(self):
        """ détruit le modèle historique et en instancie un nouveau
        """
        log(1,"Clearing historical model")
        #self.removeTab(self.indexOf(self.hist_model_win))
        self.ui.refTableStack.removeWidget(self.hist_model_win)
        self.hist_model_win = SetHistoricalModel(self)
        #self.insertTab(2,self.hist_model_win,"Set historical model")
        self.ui.refTableStack.insertWidget(0,self.hist_model_win)
        #self.setCurrentWidget(self.hist_model_win)
        self.ui.refTableStack.setCurrentWidget(self.hist_model_win)

    def setHistValid(self,valid):
        """ met à jour l'état du modèle historique
        et change l'icone du bouton en fonction de sa validité
        """
        self.hist_state_valid = valid
        self.verifyRefTableValid()
        if valid:
            self.ui.setHistoricalButton.setIcon(QIcon(dataPath.DATAPATH+"/icons/ok.png"))
        else:
            self.ui.setHistoricalButton.setIcon(QIcon(dataPath.DATAPATH+"/icons/redcross.png"))

    def setGenValid(self,valid):
        """ met à jour l'état des genetic data
        et change l'icone du bouton en fonction de la validité
        """
        self.gen_state_valid = valid
        self.verifyRefTableValid()
        if valid:
            self.ui.setGeneticButton.setIcon(QIcon(dataPath.DATAPATH+"/icons/ok.png"))
        else:
            self.ui.setGeneticButton.setIcon(QIcon(dataPath.DATAPATH+"/icons/redcross.png"))

    def verifyRefTableValid(self):
        """ Vérifie si tout est valide pour mettre à jour l'icone de l'onglet reference table
        """
        if self.gen_state_valid and self.hist_state_valid:
            self.setTabIcon(0,QIcon(dataPath.DATAPATH+"/icons/ok.png"))
            self.ui.runReftableButton.setDisabled(False)
            self.ui.stopReftableButton.setDisabled(False)
            return True
        else:
            self.setTabIcon(0,QIcon(dataPath.DATAPATH+"/icons/redcross.png"))
            self.ui.runReftableButton.setDisabled(True)
            self.ui.stopReftableButton.setDisabled(True)
            return False

    def stopActivities(self):
        """ diyabc appelle cette methode à la fermeture de l'application
        """
        pass

    def unlock(self):
        pass

