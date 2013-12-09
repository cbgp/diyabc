# -*- coding: utf-8 -*-

## @package python_interface.project
# @author Julien Veyssier
#
# @brief Classe mère abstraite de tous les projets

import subprocess,shutil
import os
import time
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
from historicalModel.setHistoricalModel import SetHistoricalModel
from utils.cbgpUtils import log,addLine
import variables
from variables import UIPATH

formProject,baseProject = uic.loadUiType("%s/Project.ui"%UIPATH)

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
        self.MIN_NUM_DATA_SETS_BY_SCENARIO_FOR_ANALYSIS = 100000

        # utile seulement si on derive de QTabWidget
        super(Project,self).__init__(parent)
        # on peut aussi instancier un tabwidget
        #self.tab = QTabWidget()

        self.createWidgets()
        self.verifyRefTableValid()

    def createWidgets(self):

        self.setupUi(self)
        self.ui = self

        # desactivation des boutons
        self.ui.setHistoricalButton.setDisabled(True)
        self.ui.setGeneticButton.setDisabled(True)
        self.ui.runReftableButton.setDisabled(True)
        self.ui.newAnButton.setDisabled(True)
        self.ui.dataFileEdit.setReadOnly(True)
        self.ui.dirEdit.setReadOnly(True)
        self.ui.groupBox_6.hide()
        self.ui.groupBox_7.hide()
        self.ui.groupBox_8.hide()


        self.ui.ascertSimFrame.hide()

        # creation des onglets "set ..."
        self.hist_model_win = SetHistoricalModel(self)
        self.hist_model_win.ui.okButton.setText("VALIDATE AND SAVE")
        self.hist_model_win.hide()

        self.ui.projNameLabelValue.setText(self.name)

        QObject.connect(self.ui.setHistoricalButton,SIGNAL("clicked()"),self.setHistorical)
        QObject.connect(self.ui.setGeneticButton,SIGNAL("clicked()"),self.setGenetic)

        # Cacher l avertissement sur la taille des reftable tant que la reftable n'est pas calculable
        self.ui.warningNumTextBrowser.hide()


        # inserer image
        self.ui.setHistoricalButton.setIcon(QIcon(variables.ICONPATH+"/redcross.png"))
        self.ui.setGeneticButton.setIcon(QIcon(variables.ICONPATH+"/redcross.png"))

        self.setTabIcon(0,QIcon(variables.ICONPATH+"/redcross.png"))
        self.setTabIcon(1,QIcon(variables.ICONPATH+"/gnome-color-browser.png"))

        self.th = None

        self.ui.verticalLayout_9.setAlignment(Qt.AlignTop)

        self.stopUiGenReftable()

        #self.ui.frame_11.hide()
        self.ui.frame_12.hide()

        self.ui.nbSetsDoneEdit.setStyleSheet("background-color: #DEF8FF")
        self.ui.dirEdit.setStyleSheet("background-color: #DEF8FF")
        self.ui.dataFileEdit.setStyleSheet("background-color: #DEF8FF")

        self.ui.fromToFrame.hide()

    def initializeRNG(self,nbNodes=1,dest=None):
        """ à lancer une fois que le dossier du projet a été créé
        """
        QApplication.setOverrideCursor( Qt.WaitCursor )
        if os.path.exists("%s/RNG_state_0000.bin"%self.dir):
            # si on est en mode "cluster" (DEPRECATED)
            if dest != None:
                shutil.copy("%s/RNG_state_0000.bin"%self.dir,"%s/RNG_save"%self.dir)

            else:
                os.remove("%s/RNG_state_0000.bin"%self.dir)
        executablePath = self.parent.preferences_win.getExecutablePath()
        if executablePath == '':
            raise Exception("Impossible to initialize the RNGs")
        nbMaxThread = self.parent.preferences_win.getMaxThreadNumber()
        # en mode cluster les options sont différentes
        if nbNodes > 1 and dest != None:
            cmd_args_list = [executablePath,"-p", dest, "-n", "c:%s"%nbNodes]
        else:
            cmd_args_list = [executablePath,"-p", "%s/"%self.dir, "-n", "t:%s"%nbMaxThread]
        cmd_args_list_quoted = list(cmd_args_list)
        for i in range(len(cmd_args_list_quoted)):
            if ";" in cmd_args_list_quoted[i] or " " in cmd_args_list_quoted[i] or ":" in cmd_args_list_quoted[i]:
                cmd_args_list_quoted[i] = '"'+cmd_args_list_quoted[i]+'"'
        log(3,"Command launched for initialization of RNGs of project '%s' : %s"%(self.name," ".join(cmd_args_list_quoted)))
        addLine("%s/command.txt"%self.dir,"Command launched for initialization of RNGs of project '%s' : %s\n\n"%(self.name," ".join(cmd_args_list_quoted)))
        outfile = "%s/init_rng.out"%(self.dir)
        f = open(outfile,"w")
        p = subprocess.Popen(cmd_args_list, stdout=f, stdin=subprocess.PIPE, stderr=subprocess.STDOUT)
        #p = subprocess.Popen(cmd_args_list, stdout=f, stderr=subprocess.STDOUT)
        time.sleep(1)
        while (p.poll() == None):
            time.sleep(1)
        f.close()
        if dest != None:
            log(3,"Generation of RNGs of project '%s' for cluster terminated with returncode : %s"%(self.name,p.poll()))
        else:
            log(3,"Initialization of RNGs of project '%s' terminated with returncode : %s"%(self.name,p.poll()))
        QApplication.restoreOverrideCursor()

    def goodRNGSize(self):
        """ Demande a l'exécutable le nombre de thread prévu dans les RNG existants dans le dossier du projet
        et le compare au nombre de thread des préférences
        """
        if not os.path.exists("%s/RNG_state_0000.bin"%self.dir):
            log(3,"RNG binary file cannot be found")
            return False
        if os.path.exists("%s/RNG_state_0000_cores.txt"%self.dir):
            os.remove("%s/RNG_state_0000_cores.txt"%self.dir)
        QApplication.setOverrideCursor( Qt.WaitCursor )
        nbThreadExpected = self.parent.preferences_win.getMaxThreadNumber()

        executablePath = self.parent.preferences_win.getExecutablePath()
        cmd_args_list = [executablePath,"-z", "%s/RNG_state_0000.bin"%self.dir ]
        cmd_args_list_quoted = list(cmd_args_list)
        for i in range(len(cmd_args_list_quoted)):
            if ";" in cmd_args_list_quoted[i] or " " in cmd_args_list_quoted[i] or ":" in cmd_args_list_quoted[i]:
                cmd_args_list_quoted[i] = '"'+cmd_args_list_quoted[i]+'"'
        log(3,"Command launched for RNGs check of project '%s' : %s"%(self.name," ".join(cmd_args_list_quoted)))
        addLine("%s/command.txt"%self.dir,"Command launched for RNGs check of project '%s' : %s\n\n"%(self.name," ".join(cmd_args_list_quoted)))
        outfile = "%s/rng_check.out"%(self.dir)
        f = open(outfile,"w")
        p = subprocess.Popen(cmd_args_list, stdout=f, stdin=subprocess.PIPE, stderr=subprocess.STDOUT)
        time.sleep(1)
        while (p.poll() == None):
            time.sleep(1)
        f.close()
        if not os.path.exists("%s/RNG_state_0000_cores.txt"%self.dir):
            log(3,"RNG info file cannot be found")
            QApplication.restoreOverrideCursor()
            return False
        ff = open("%s/RNG_state_0000_cores.txt"%self.dir,'rU')
        try:
            first_line = ff.readlines()[0].strip()
            nbRNGThread = int(first_line)
        except Exception as e:
            log(3,"Malformed RNG info file")
            QApplication.restoreOverrideCursor()
            ff.close()
            return False
        ff.close()

        log(3,"RNGs check of project '%s' terminated with returncode : %s"%(self.name,p.poll()))
        if (nbThreadExpected <= nbRNGThread):
            log(3,"GOOD RNG check of project '%s'. No need to generate RNGs again (%s needed, %s found)."%(self.name,nbThreadExpected,nbRNGThread))
        else:
            log(3,"BAD RNG check of project '%s'. They should (and will be) be generated again (%s needed, %s found)."%(self.name,nbThreadExpected,nbRNGThread))

        QApplication.restoreOverrideCursor()
        return (nbThreadExpected <= nbRNGThread)

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
        self.ui.refTableStack.addWidget(self.hist_model_win)
        self.ui.refTableStack.setCurrentWidget(self.hist_model_win)
        self.setHistValid(False)

    def setNbScenarios(self,nb):
        """ ecrit le nombre de scenarios dans la zone "historical model"
            ecrit le waning sur le nombre de donnees simulees necessaires pas scenario
        """
        pluriel = ""
        if nb > 1:
            pluriel = "s"
        self.ui.nbScLabel.setText("{0} scenario{1}".format(nb,pluriel))
        warningRichText = str(self.ui.warningNumTextBrowser.toHtml())
        warningRichText = warningRichText.replace('ShouldBeDynamicallyReplacedByNumOfSceanrio', str(nb))
        self.ui.warningNumTextBrowser.setText(warningRichText)

    def setNbParams(self,nb):
        """ écrit le nombre de paramètres dans la zone "historical model"
        """
        pluriel = ""
        if nb > 1:
            pluriel = "s"
        self.ui.nbHistParamLabel.setText("{0} historical parameter{1}".format(nb,pluriel))

    def clearHistoricalModel(self):
        """ détruit le modèle historique et en instancie un nouveau
        """
        log(1,"Clearing historical model")
        self.ui.refTableStack.removeWidget(self.hist_model_win)
        self.hist_model_win = SetHistoricalModel(self)
        self.ui.refTableStack.insertWidget(0,self.hist_model_win)
        self.ui.refTableStack.setCurrentWidget(self.hist_model_win)

    def setHistValid(self,valid):
        """ met à jour l'état du modèle historique
        et change l'icone du bouton en fonction de sa validité
        """
        self.hist_state_valid = valid
        self.verifyRefTableValid()
        if valid:
            self.ui.setHistoricalButton.setIcon(QIcon(variables.ICONPATH+"/ok.png"))
        else:
            self.ui.setHistoricalButton.setIcon(QIcon(variables.ICONPATH+"/redcross.png"))

    def setGenValid(self,valid):
        """ met à jour l'état des genetic data
        et change l'icone du bouton en fonction de la validité
        """
        self.gen_state_valid = valid
        self.verifyRefTableValid()
        if valid:
            self.ui.setGeneticButton.setIcon(QIcon(variables.ICONPATH+"/ok.png"))
        else:
            self.ui.setGeneticButton.setIcon(QIcon(variables.ICONPATH+"/redcross.png"))

    def verifyRefTableValid(self):
        """ Vérifie si tout est valide pour mettre à jour l'icone de l'onglet reference table, activer ou non le button de lancement du calcul de la reftable, affichage ou non des avertissements sur la taille du reftable.
        """
        if self.gen_state_valid and self.hist_state_valid:
            self.setTabIcon(0,QIcon(variables.ICONPATH+"/ok.png"))
            self.ui.runReftableButton.setDisabled(False)
            self.ui.stopReftableButton.setDisabled(False)
            if not hasattr(self, "stopSimulation" ) :
                self.ui.warningNumTextBrowser.show()
            return True
        else:
            self.setTabIcon(0,QIcon(variables.ICONPATH+"/redcross.png"))
            self.ui.runReftableButton.setDisabled(True)
            self.ui.stopReftableButton.setDisabled(True)
            self.ui.warningNumTextBrowser.hide()
            return False

    def stopActivities(self):
        """ diyabc appelle cette methode à la fermeture de l'application
        """
        pass

    def unlock(self):
        pass

