# -*- coding: utf-8 -*-

import os
import codecs
#from subprocess import Popen, PIPE, STDOUT 
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from projectReftable import ProjectReftable
from setGenDataMsatSeq import SetGeneticDataMsatSeq
from utils.data import Data
import os.path
import output
from utils.cbgpUtils import log


class ProjectMsatSeq(ProjectReftable):
    """ classe qui représente un projet
    par defaut, un projet est considéré comme nouveau, cad que l'affichage est celui d'un projet vierge
    pour un projet chargé, on modifie l'affichage en conséquence dans loadFromDir
    """
    def __init__(self,name,dir=None,parent=None):
        super(ProjectMsatSeq,self).__init__(name,dir,parent)

        self.gen_data_win = SetGeneticDataMsatSeq(self)
        self.gen_data_win.ui.okButton.setText("VALIDATE AND SAVE")
        self.gen_data_win.hide()

    def loadDataFile(self,name):
        """ Charge le fichier de données passé en paramètre. Cette fonction est appelée lors
        de l'ouverture d'un projet existant et lors du choix du fichier de données pour un nouveau projet
        """
        log(2,"Loading datafile '%s'"%name)
        new_data = Data(name)
        try:
            new_data.loadfromfile()
            # on ne garde le data que si le load n'a pas déclenché d'exception
            self.data = new_data
            microsat = ""
            sequences = ""
            et = ""
            if self.data.nloc_mic > 0:
                microsat = "%s microsat"%self.data.nloc_mic
            if self.data.nloc_seq > 0:
                sequences = "%s sequences"%self.data.nloc_seq
            if self.data.nloc_mic > 0 and self.data.nloc_seq > 0:
                et = " and "
            self.ui.dataFileInfoLabel.setText("%s loci (%s%s%s)\n%s individuals in %s samples" % (self.data.nloc,microsat,et,sequences,self.data.nindtot,self.data.nsample))
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

    def setGenetic(self):
        """ passe sur l'onglet correspondant
        """
        log(1,"Entering in Genetic Data Setting")
        #self.setTabEnabled(0,False)
        #self.setTabEnabled(1,False)
        #self.addTab(self.gen_data_win,"Set genetic data")
        #self.setCurrentWidget(self.gen_data_win)
        self.ui.refTableStack.addWidget(self.gen_data_win)
        self.ui.refTableStack.setCurrentWidget(self.gen_data_win)
        self.setGenValid(False)

    def loadFromDir(self):
        """ charge les infos à partir du répertoire self.dir
        """
        log(2,"Launching load procedures")
        # GUI
        self.ui.dirEdit.setText(self.dir)
        self.ui.browseDataFileButton.setDisabled(True)
        self.ui.browseDataFileButton.hide()
        self.ui.browseDirButton.hide()
        #self.ui.groupBox.show()
        self.ui.groupBox_6.show()
        self.ui.groupBox_7.show()
        self.ui.groupBox_8.show()
        self.ui.setHistoricalButton.setDisabled(False)
        self.ui.setGeneticButton.setDisabled(False)

        # lecture du meta project
        if self.loadMyConf():
            # lecture de conf.hist.tmp
            self.hist_model_win.loadHistoricalConf()
            self.gen_data_win.loadGeneticConf()
            self.loadAnalysis()
        else:
            raise Exception("Impossible to read the project configuration")
            output.notify(self,"Load error","Impossible to read the project configuration")

    def freezeGenData(self,yesno=True):
        """ empêche la modification des genetic data tout en laissant
        la possibilité de les consulter
        """
        un=""
        if yesno: un="un"
        log(2,"%sfreezing Genetic Data"%un)
        self.gen_data_win.ui.clearButton.setDisabled(yesno)
        self.gen_data_win.ui.exitButton.setDisabled(yesno)
        self.gen_data_win.ui.tableWidget.setDisabled(yesno)
        self.gen_data_win.ui.addGroupButton.setDisabled(yesno)
        for g in self.gen_data_win.groupList:
            for b in g.findChildren(QPushButton,"rmFromButton"):
                b.setDisabled(yesno)
            for b in g.findChildren(QPushButton,"addToButton"):
                b.setDisabled(yesno)
            for b in g.findChildren(QPushButton,"rmButton"):
                b.setDisabled(yesno)

            for e in self.gen_data_win.setMutation_dico[g].findChildren(QLineEdit):
                e.setDisabled(yesno)
            for e in self.gen_data_win.setMutation_dico[g].findChildren(QRadioButton):
                e.setDisabled(yesno)
            self.gen_data_win.setMutation_dico[g].ui.clearButton.setDisabled(yesno)
            self.gen_data_win.setMutation_dico[g].ui.exitButton.setDisabled(yesno)

            for e in self.gen_data_win.setMutationSeq_dico[g].findChildren(QLineEdit):
                e.setDisabled(yesno)
            for e in self.gen_data_win.setMutationSeq_dico[g].findChildren(QRadioButton):
                e.setDisabled(yesno)
            self.gen_data_win.setMutationSeq_dico[g].ui.clearButton.setDisabled(yesno)
            self.gen_data_win.setMutationSeq_dico[g].ui.exitButton.setDisabled(yesno)

            for e in self.gen_data_win.setSum_dico[g].findChildren(QLineEdit):
                e.setDisabled(yesno)
            for e in self.gen_data_win.setSum_dico[g].findChildren(QPushButton):
                e.setDisabled(yesno)
            for e in self.gen_data_win.setSum_dico[g].findChildren(QCheckBox):
                e.setDisabled(yesno)
            self.gen_data_win.setSum_dico[g].ui.clearButton.setDisabled(yesno)
            self.gen_data_win.setSum_dico[g].ui.exitButton.setDisabled(yesno)
            self.gen_data_win.setSum_dico[g].ui.okButton.setDisabled(False)
            self.gen_data_win.setSum_dico[g].ui.addAdmixButton.setDisabled(yesno)

            for e in self.gen_data_win.setSumSeq_dico[g].findChildren(QLineEdit):
                e.setDisabled(yesno)
            for e in self.gen_data_win.setSumSeq_dico[g].findChildren(QPushButton):
                e.setDisabled(yesno)
            for e in self.gen_data_win.setSumSeq_dico[g].findChildren(QCheckBox):
                e.setDisabled(yesno)
            self.gen_data_win.setSumSeq_dico[g].ui.clearButton.setDisabled(yesno)
            self.gen_data_win.setSumSeq_dico[g].ui.exitButton.setDisabled(yesno)
            self.gen_data_win.setSumSeq_dico[g].ui.okButton.setDisabled(False)
            self.gen_data_win.setSumSeq_dico[g].ui.addAdmixButton.setDisabled(yesno)

        if yesno:
            self.gen_data_win.findChild(QPushButton,"okButton").setText("OK")
        else:
            self.gen_data_win.findChild(QPushButton,"okButton").setText("VALIDATE AND SAVE")

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
            self.loadDataFile("%s/%s"%(self.dir,lines[0].strip()))
            # comme on a lu le datafile, on peut remplir le tableau de locus dans setGeneticData
            self.gen_data_win.fillLocusTableFromData()
            return True
        return False

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
            nb_param += self.gen_data_win.getNbParam()
            nb_sum_stats = self.gen_data_win.getNbSumStats()
            f.write("%s parameters and %s summary statistics\n\n"%(nb_param,nb_sum_stats))
            f.close()

            # save hist conf
            self.hist_model_win.writeHistoricalConfFromGui()
            # save gen conf
            self.gen_data_win.writeGeneticConfFromGui()
            # save th conf et production du reftable header
            if self.gen_state_valid and self.hist_state_valid:
                self.writeThConf()
                self.writeRefTableHeader()
            self.saveAnalysis()
            self.parent.clearStatus()
            self.parent.showStatus("Project %s successfully saved"%self.name,2000)
        else:
            output.notify(self,"Saving is impossible","Project %s is not yet completly created"%self.name)
            self.parent.clearStatus()

    def writeThConf(self):
        """ ecrit le header du tableau de resultat qui sera produit par les calculs
        il contient, les paramètres historicaux, les paramètres mutationnels, les summary stats
        """
        log(2,"Writing last part of the header (the parameter table header) in %s"%self.parent.table_header_conf_name)
        hist_params_txt = self.hist_model_win.getParamTableHeader()
        mut_params_txt = self.gen_data_win.getParamTableHeader()
        sum_stats_txt = self.gen_data_win.getSumStatsTableHeader()
        if os.path.exists(self.dir+"/%s"%self.parent.table_header_conf_name):
            os.remove("%s/%s"%(self.dir,self.parent.table_header_conf_name))
        f = codecs.open(self.dir+"/%s"%self.parent.table_header_conf_name,'w',"utf-8")
        f.write("scenario%s%s%s"%(hist_params_txt,mut_params_txt,sum_stats_txt))
        f.close()

    def checkAll(self):
        """ vérification du modèle historique et mutationnel
        cette fonction est appelée au chargement du projet pour restituer l'etat du projet
        """
        log(2,"Checking validity of Historical Model and Genetic Data")
        # historical model : 
        self.hist_model_win.definePriors(silent=True)
        if self.hist_model_win.checkAll(silent=True):
            self.setHistValid(True)
            self.hist_model_win.majProjectGui()
        # mutation model : plus facile d'utiliser directement la validation
        self.gen_data_win.validate(silent=True)

