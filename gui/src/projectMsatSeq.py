# -*- coding: utf-8 -*-

## @package python_interface.projectMsatSeq
# @author Julien Veyssier
#
# @brief Projets pour créer une table de référence Msat et Sequences

import os
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from projectReftable import ProjectReftable
from geneticData.setGenDataMsatSeq import SetGeneticDataMsatSeq
from utils.data import Data
import os.path
import output
from utils.cbgpUtils import log,isUnixText,dos2unix


## @class ProjectMsatSeq
# @brief Projets pour créer une table de référence Msat et Sequences
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

    def getDataFileFilter(self):
        """ retourne le bon filtre de selection de fichier pour le chargement du datafile
        """
        return "Microsat Sequence datafile (*.mss);;all files (*)"

    def dataFileSelectionAndCopy(self,name=None):
        """ si tout s'est bien passé, on rempli le tableau des locus des genetic data
        """
        if super(ProjectMsatSeq,self).dataFileSelectionAndCopy(name):
            # comme on a lu le datafile, on peut remplir le tableau de locus dans setGeneticData
            self.gen_data_win.fillLocusTableFromData()

    def loadDataFile(self,name):
        """ Charge le fichier de données passé en paramètre. Cette fonction est appelée lors
        de l'ouverture d'un projet existant et lors du choix du fichier de données pour un nouveau projet
        """
        log(2,"Loading datafile '%s'"%name)
        ## conversion de dos à unix
        #if not isUnixText(name):
        #    dos2unix(name)
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
        except Exception as e:
            keep = ""
            if self.ui.dataFileEdit.text() != "":
                keep = "\n\nThe file was not loaded, nothing was changed"
            output.notify(self,"Data file error","%s%s"%(e,keep))
            return False
        return True

    def setGenetic(self):
        """ passe sur l'onglet correspondant
        """
        log(1,"Entering in Genetic Data Setting")
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
        self.ui.groupBox_6.show()
        self.ui.groupBox_7.show()
        self.ui.groupBox_8.show()
        self.ui.setHistoricalButton.setDisabled(False)
        self.ui.setGeneticButton.setDisabled(False)

        try:
            # lecture du meta project
            self.loadMyConf()
            # lecture de conf.hist.tmp
            self.hist_model_win.loadHistoricalConf()
            self.gen_data_win.loadGeneticConf()
            self.loadAnalysis()
        except Exception as e:
            raise Exception("Impossible to load the project configuration\n\n%s"%e)
            output.notify(self,"Load error","Impossible to load the project configuration\n\n%s"%e)

    def freezeGenData(self,yesno=True):
        """ empêche la modification des genetic data tout en laissant
        la possibilité de les consulter
        """
        un=""
        if not yesno: un="un"
        log(2,"%sfreezing Genetic Data"%un)
        self.gen_data_win.ui.clearButton.setDisabled(yesno)
        self.gen_data_win.ui.exitButton.setDisabled(yesno)
        self.gen_data_win.ui.tableWidget.setDisabled(yesno)
        self.gen_data_win.ui.addGroupButton.setDisabled(yesno)
        self.gen_data_win.ui.autoGroupButton.setDisabled(yesno)
        for g in self.gen_data_win.groupList:
            for b in g.findChildren(QPushButton,"rmFromGroupButton"):
                b.setDisabled(yesno)
            for b in g.findChildren(QPushButton,"addToGroupButton"):
                b.setDisabled(yesno)
            for b in g.findChildren(QPushButton,"rmLocusGroupButton"):
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
            self.ui.setGeneticButton.setText("            View")
        else:
            self.gen_data_win.findChild(QPushButton,"okButton").setText("VALIDATE AND SAVE")
            self.ui.setGeneticButton.setText("            Set")

    def loadMyConf(self):
        """ lit le fichier conf.tmp pour charger le fichier de données
        """
        log(2,"Reading '%s' to load datafile"%self.parent.main_conf_name)
        if os.path.exists(self.ui.dirEdit.text()+"/%s"%self.parent.main_conf_name):
            f = open("%s/%s"%(self.dir,self.parent.main_conf_name),"r")
            lines = f.readlines()
            self.dataFileName = lines[0].strip()
            self.ui.dataFileEdit.setText(self.dataFileName)
            # lecture du dataFile pour les infos de Gui Projet
            if os.path.exists("%s/%s"%(self.dir,self.dataFileName)):
                if self.loadDataFile("%s/%s"%(self.dir,self.dataFileName)):
                    # comme on a lu le datafile, on peut remplir le tableau de locus dans setGeneticData
                    self.gen_data_win.fillLocusTableFromData()
                else:
                    raise Exception("Impossible to load the datafile (%s) which is possibly malformed"%self.dataFileName)
            else:
                raise Exception("Datafile doesn't exist (%s)"%self.dataFileName)
        else:
            raise Exception("Main conf file not found (%s)"%self.parent.main_conf_name)

    def getNbSumStats(self):
        return self.gen_data_win.getNbSumStats()

    def getLocusGroups(self):
        """ renvoie une liste des numeros de groupes de locus
        utilisé par setupEstimationBias pour remplir la liste de groupes dans le cadre
        de la redéfinition de sumstats
        """
        l_groups = []
        for gb in self.gen_data_win.groupList:
            if "Micro" in gb.title() or "Sequen" in gb.title():
                l_groups.append(str(gb.title()).split()[1])
        return l_groups

    def redefineSumStatsAnalysis(self,setupEst):
        setupEst.redefineSumStatsMsatSeq()

    def getNextWidget(self,hist_model):
        return hist_model.getNextWidgetMsatSeq()

    def save(self):
        """ sauvegarde du projet -> mainconf, histconf, genconf, theadconf
        Si le gen et hist sont valides, on génère le header
        """
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
            nb_sum_stats = self.getNbSumStats()
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

