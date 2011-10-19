# -*- coding: utf-8 -*-

import os
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from projectReftable import ProjectReftable
#from uis.project_ui import *
from summaryStatistics.setSummaryStatisticsSnp import SetSummaryStatisticsSnp
#from mutationModel.setMutationModelMsat import SetMutationModelMsat
#from mutationModel.setMutationModelSequences import SetMutationModelSequences
#from summaryStatistics.setSummaryStatisticsMsat import SetSummaryStatisticsMsat
#from summaryStatistics.setSummaryStatisticsSeq import SetSummaryStatisticsSeq
from utils.data import DataSnp
import os.path
import output
from utils.cbgpUtils import log

class ProjectSnp(ProjectReftable):
    """ classe qui représente un projet de simulation
    """
    def __init__(self,name,dir=None,parent=None):
        super(ProjectSnp,self).__init__(name,dir,parent)

        self.ui.nbMicrosatLabel.hide()
        self.ui.nbSequencesLabel.hide()
        # TODO suppr cette ligne
        self.setGenValid(False)
        #QObject.connect(self.ui.setSumSnpButton,SIGNAL("clicked()"),self.setSumStat)
        self.ui.label_15.setText("Summary statistics")

    def setGenetic(self):
        """ initie la définition des summary statistics
        """
        log(1,"Entering in Summary statistics")
        self.ui.refTableStack.addWidget(self.sum_stat_win)
        self.ui.refTableStack.setCurrentWidget(self.sum_stat_win)
        self.setGenValid(False)

    #def setGenValid(self,valid):
    #    """ met à jour l'état des genetic data
    #    et change l'icone du bouton en fonction de la validité
    #    """
    #    self.gen_state_valid = valid
    #    self.verifyRefTableValid()
    #    if valid:
    #        self.ui.setSumSnpButton.setIcon(QIcon("docs/icons/ok.png"))
    #    else:
    #        self.ui.setSumSnpButton.setIcon(QIcon("docs/icons/redcross.png"))

    def checkAll(self):
        """ vérification du modèle historique et mutationnel
        cette fonction est appelée au chargement du projet pour restituer l'etat du projet
        """
        log(2,"Checking validity of Historical Model and Sum Stats")
        # historical model : 
        self.hist_model_win.definePriors(silent=True)
        if self.hist_model_win.checkAll(silent=True):
            self.setHistValid(True)
            self.hist_model_win.majProjectGui()
        # mutation model : plus facile d'utiliser directement la validation
        self.sum_stat_win.validate(silent=True)

    def returnToMe(self):
        self.ui.refTableStack.removeWidget(self.ui.refTableStack.currentWidget())
        self.ui.refTableStack.setCurrentIndex(0)

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
            self.sum_stat_win.loadSumStatsConf()
            self.loadAnalysis()
        else:
            raise Exception("Impossible to read the project configuration")
            output.notify(self,"Load error","Impossible to read the project configuration")

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

        # on declare les sumstats apres avoir chargé le datafile car c'est nécessaire
        self.dummy = QFrame()
        self.dummy.parent = self
        # feinte pour que le parent.parent renvoie au projet
        self.sum_stat_win = SetSummaryStatisticsSnp(parent=self.dummy)
        self.sum_stat_win.hide()
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

    def freezeGenData(self,yesno=True):
        # TODO
        pass
