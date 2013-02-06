# -*- coding: utf-8 -*-

## @package python_interface.projectSnp
# @author Julien Veyssier
#
# @brief Projets pour créer une table de référence SNP

import os
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from projectReftable import ProjectReftable
from summaryStatistics.setSummaryStatisticsSnp import SetSummaryStatisticsSnp
from utils.data import DataSnp
import os.path
import output
from controlAscertBias import ControlAscertBias
import variables
from utils.cbgpUtils import log,isUnixText,dos2unix

## @class ProjectSnp
# @brief Projets pour créer une table de référence SNP
class ProjectSnp(ProjectReftable):
    """ classe qui représente un projet de simulation
    """
    def __init__(self,name,dir=None,parent=None):
        super(ProjectSnp,self).__init__(name,dir,parent)

        self.ui.nbMicrosatLabel.hide()
        self.setGenValid(False)
        self.ui.genDataLabel.setText("Summary statistics")

        self.typesOrdered = ["A","H","X","Y","M"]

        self.ui.frame_11.show()
        #self.ui.frame_12.show()

        self.ascert_frame = ControlAscertBias(self)
        self.ascert_frame.hide()
        self.ascert_state_valid = False
        self.setAscertValid(False)

        QObject.connect(self.ui.ascertButton,SIGNAL("clicked()"),self.setAscertBias)

    def getDataFileFilter(self):
        return "SNP datafile (*.snp);;all files (*)"

    def dataFileSelectionAndCopy(self,name=None):
        """ si tout s'est bien passé, on sauve
        """
        if super(ProjectSnp,self).dataFileSelectionAndCopy(name):
            self.updateNbStats()
            self.save()

    def verifyRefTableValid(self):
        """ Vérifie si tout est valide pour mettre à jour l'icone de l'onglet reference table
        """
        if self.gen_state_valid and self.hist_state_valid and self.ascert_state_valid:
            self.setTabIcon(0,QIcon(variables.ICONPATH+"/ok.png"))
            self.ui.runReftableButton.setDisabled(False)
            self.ui.stopReftableButton.setDisabled(False)
            return True
        else:
            self.setTabIcon(0,QIcon(variables.ICONPATH+"/redcross.png"))
            self.ui.runReftableButton.setDisabled(True)
            self.ui.stopReftableButton.setDisabled(True)
            return False

    def setAscertBias(self):
        log(1,"Entering in ControlAscertBias")
        self.setAscertValid(False)
        self.ui.refTableStack.addWidget(self.ascert_frame)
        self.ui.refTableStack.setCurrentWidget(self.ascert_frame)

    def setAscertValid(self,yn):
        self.ascert_state_valid = yn
        self.verifyRefTableValid()
        if yn:
            self.ui.ascertButton.setIcon(QIcon(variables.ICONPATH+"/ok.png"))
        else:
            self.ui.ascertButton.setIcon(QIcon(variables.ICONPATH+"/redcross.png"))

    def isSnp(self):
        return True

    def setGenetic(self):
        """ initie la définition des summary statistics
        """
        log(1,"Entering in Summary statistics")
        ty = str(self.typeCombo.currentText())
        self.ui.refTableStack.addWidget(self.sum_stat_wins[ty])
        self.ui.refTableStack.setCurrentWidget(self.sum_stat_wins[ty])
        self.setGenValid(False)

    def getNbSumStats(self):
        nb = 0
        for ty in self.sum_stat_wins.keys():
            (nstat,stat_txt) = self.sum_stat_wins[ty].getSumConf()
            nb += int(nstat)
        if self.ascert_frame.ascYesRadio.isChecked():
            nb += 1
        return nb

    def getNbTotalTaken(self):
        nb = 0
        for ty in self.sum_stat_wins.keys():
            nb += int(self.sum_stat_wins[ty].ui.takenEdit.text())
        return nb

    def updateNbStats(self):
        """ met à jour les informations graphiques sur les
        summary stats : le nombre total de stats et le nombre
        total de locus concernés par des stats
        """
        nb = self.getNbSumStats()
        nbTaken = self.getNbTotalTaken()
        if nb > 0:
            self.setGenValid(True)
        if nb > 1:
            plur='s'
        else:
            plur=''
        self.ui.nbSumStatsLabel.setText("Total : %s summary statistic%s"%(nb,plur))
        self.ui.nbSequencesLabel.setText("Total : %s taken locus"%(nbTaken))

    def getLocusGroups(self):
        """ renvoie une liste des groupes de locus
        utilisé par setupEstimationBias pour remplir la liste de groupes dans le cadre
        de la redéfinition de sumstats
        """
        return self.sum_stat_wins.keys()

    def redefineSumStatsAnalysis(self,setupEst):
        """ methode ping pong pour sélectionner la bonne methode
        de redefinition d'un groupe de sumstats
        """
        setupEst.redefineSumStatsSnp()

    def getNextWidget(self,hist_model):
        """ méthode ping pong pour connaitre l'objet qui suit un
        historical model fixed ou drawn
        """
        return hist_model.getNextWidgetSnp()

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
        for ty in self.sum_stat_wins.keys():
            self.sum_stat_wins[ty].validate(silent=True)
        self.ascert_frame.validate(silent=True)

    def returnToMe(self):
        self.ui.refTableStack.removeWidget(self.ui.refTableStack.currentWidget())
        self.ui.refTableStack.setCurrentIndex(0)

    def loadSumStatsConf(self):
        """ charge les summary statistics depuis le fichier gen_conf_name
        """
        if os.path.exists(self.dir):
            if os.path.exists("%s/%s"%(self.dir,self.parent.gen_conf_name)):
                f = codecs.open("%s/%s"%(self.dir,self.parent.gen_conf_name),"r","utf-8")
                lines = f.readlines()
                f.close()
                nl = 1
                diconb = {}
                dicoFrom = {}
                dicoGrTy = {}
                dicoStatLines = {}
                while (lines[nl].strip() != ""):
                    ty = lines[nl].split('<')[1].split('>')[0]
                    numGr = int(lines[nl].strip().split()[2][1:])
                    diconb[ty] = lines[nl].strip().split()[0]
                    dicoFrom[ty] = lines[nl].strip().split()[4]
                    dicoGrTy[numGr] = ty
                    self.sum_stat_wins[ty].ui.takenEdit.setText(diconb[ty])
                    self.sum_stat_wins[ty].ui.fromEdit.setText(dicoFrom[ty])
                    nl += 1
                nl += 2
                while nl < len(lines) and lines[nl].strip() != "":
                    numGroup = int(lines[nl].strip().split()[1][1:])
                    ty = dicoGrTy[numGroup]
                    statlines = []
                    nl += 1
                    while nl < len(lines) and "group" not in lines[nl]:
                        statlines.append( lines[nl] )
                        nl += 1
                    dicoStatLines[ty] = statlines

                for ty in dicoStatLines.keys():
                    self.sum_stat_wins[ty].setSumConf(dicoStatLines[ty])

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
            self.ascert_frame.loadAscertConf()
            self.loadSumStatsConf()
            self.loadAnalysis()
        except Exception as e:
            raise Exception("Impossible to read the project configuration\n\n%s"%e)
            output.notify(self,"Load error","Impossible to read the project configuration\n\n%s"%e)

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
                    ## comme on a lu le datafile, on peut remplir le tableau de locus dans setGeneticData
                    pass
                else:
                    raise Exception("Impossible to load the datafile (%s) because it is possibly malformed"%self.dataFileName)
            else:
                raise Exception("Datafile doesn't exist (%s)"%self.dataFileName)
        else:
            raise Exception("Main conf file not found (%s)"%self.parent.main_conf_name)

    def loadDataFile(self,name):
        """ Charge le fichier de données passé en paramètre. Cette fonction est appelée lors
        de l'ouverture d'un projet existant et lors du choix du fichier de données pour un nouveau projet
        """
        log(2,"Loading datafile '%s'"%name)
        ## conversion de dos à unix
        #if not isUnixText(name):
        #    dos2unix(name)
        try:
            self.data = DataSnp(name)
            typestr = ""
            for ty in self.data.ntypeloc.keys():
                typestr += " %s : %s,"%(ty,self.data.ntypeloc[ty])
            typestr = typestr[:-1]
            self.ui.dataFileInfoLabel.setText("%s loci (SNP)\n%s individuals in %s samples\n%s" % (self.data.nloc,self.data.nindtot,self.data.nsample,typestr))
            self.ui.dataFileEdit.setText(name)
            self.dataFileSource = name

        except Exception as e:
            keep = ""
            if self.ui.dataFileEdit.text() != "":
                keep = "\n\nThe file was not loaded, nothing was changed"
            output.notify(self,"Data file error","%s%s"%(e,keep))
            return False

        # on declare les sumstats apres avoir chargé le datafile car c'est nécessaire
        # feinte pour que le parent.parent renvoie au projet
        self.dummy = QFrame()
        self.dummy.parent = self
        self.sum_stat_wins = {}
        for ty in self.data.ntypeloc.keys():
            self.sum_stat_wins[ty] = SetSummaryStatisticsSnp(parent=self.dummy,numGroup=ty)
            self.sum_stat_wins[ty].hide()

        # selection du type de snp pour sumstats
        self.typeCombo = QComboBox(self)
        for ty in self.typesOrdered:
            if ty in self.data.ntypeloc.keys():
                self.typeCombo.addItem(ty)
        self.ui.horizontalLayout_6.addWidget(QLabel("for locus type :"))
        self.ui.horizontalLayout_6.addWidget(self.typeCombo)

        return True

    def clearSummaryStats(self):
        self.sum_stat_wins = {}
        for ty in self.data.ntypeloc.keys():
            self.sum_stat_wins[ty] = SetSummaryStatisticsSnp(parent=self.dummy,numGroup=ty)
            self.sum_stat_wins[ty].hide()


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
            f.write("%s parameters and %s summary statistics\n\n"%(nb_param,self.getNbSumStats()))
            f.close()

            # save hist conf
            self.hist_model_win.writeHistoricalConfFromGui()
            # save gen conf
            self.writeGeneticConfFromGui()
            # save th conf et production du reftable header
            if self.gen_state_valid and self.hist_state_valid:
                self.writeThConf()
                self.writeAscertConf()
                self.writeRefTableHeader()
            self.saveAnalysis()
            self.parent.clearStatus()
            self.parent.showStatus("Project %s successfully saved"%self.name,2000)
        else:
            output.notify(self,"Saving is impossible","Project %s is not yet completly created"%self.name)
            self.parent.clearStatus()

    def writeAscertConf(self):
        log(2,"Writing ascertainment in %s"%self.parent.ascertainment_conf_name)

        ascert_string = u""
        if self.ascert_frame.ascYesRadio.isChecked():
            ascert_string += "%s\n"%str(self.ascert_frame.asc1ValEdit.text()).strip()
            ascert_string += "%s\n"%str(self.ascert_frame.asc2ValEdit.text()).strip()
            ascert_string += "%s\n"%str(self.ascert_frame.asc3ValEdit.text()).strip()
        else:
            ascert_string = u"\n"


        if os.path.exists(self.dir+"/%s"%self.parent.ascertainment_conf_name):
            os.remove("%s/%s"%(self.dir,self.parent.ascertainment_conf_name))
        f = codecs.open(self.dir+"/%s"%self.parent.ascertainment_conf_name,'w',"utf-8")
        f.write("%s"%(ascert_string))
        f.close()

    def writeRefTableHeader(self):
        """ écriture du header.txt à partir des conf
        """
        super(ProjectSnp,self).writeRefTableHeader()

        if os.path.exists(self.dir+"/%s"%self.parent.ascertainment_conf_name):
            f = codecs.open(self.dir+"/%s"%self.parent.ascertainment_conf_name,"r","utf-8")
            ascert_string = f.read()
            f.close()
        else:
            output.notify(self,"Header generation problem","Impossible to add ascertainment part to header : ascertainment file doesn't exist")
            return

        if os.path.exists(self.dir+"/%s"%self.parent.reftableheader_name):
            log(2,"Adding ascertainment part to header")

            fdest = codecs.open(self.dir+"/%s"%self.parent.reftableheader_name,"a","utf-8")
            fdest.write("\n\n%s"%ascert_string)
            fdest.close()
        else:
            output.notify(self,"Header generation problem","Impossible to add ascertainment part to header : header doesn't exist")

    def writeThConf(self):
        """ ecrit le header du tableau de resultat qui sera produit par les calculs
        il contient, les paramètres historicaux,  les summary stats
        """
        log(2,"Writing last part of the header (the parameter table header) in %s"%self.parent.table_header_conf_name)
        hist_params_txt = self.hist_model_win.getParamTableHeader()
        sum_stats_txt = self.getSumStatsTableHeader()
        if os.path.exists(self.dir+"/%s"%self.parent.table_header_conf_name):
            os.remove("%s/%s"%(self.dir,self.parent.table_header_conf_name))
        f = codecs.open(self.dir+"/%s"%self.parent.table_header_conf_name,'w',"utf-8")
        f.write("scenario%s%s"%(hist_params_txt,sum_stats_txt))
        f.close()
        
    def getSumStatsTableHeader(self):
        """ retourne la partie sumstats du table header
        """
        result = u""
        numGroup = 1
        for ty in self.typesOrdered:
            if ty in self.sum_stat_wins.keys():
                sums_txt = self.sum_stat_wins[ty].getSumStatsTableHeader(numGroup)
                result += sums_txt
                numGroup += 1
        if self.ascert_frame.ascYesRadio.isChecked():
            result += output.centerHeader("PPL",14)
        return result

    def writeGeneticConfFromGui(self):
        """ Sauvegarde des summary statistics dans le fichier gen_conf_name
        """
        locidesc = ""
        statsdesc = ""
        numGr = 1
        totNbStat = 0
        if self.ascert_frame.ascYesRadio.isChecked():
            totNbStat += 1
        for ty in self.typesOrdered:
            if ty in self.sum_stat_wins.keys():
                (nstat,statstr) = self.sum_stat_wins[ty].getSumConf()
                totNbStat += nstat
                if nstat > 0:
                    locidesc += "%s <%s> G%s from %s\n"%(self.sum_stat_wins[ty].ui.takenEdit.text(),ty,numGr,self.sum_stat_wins[ty].ui.fromEdit.text())
                    statsdesc += "group G%s (%s)\n%s"%(numGr,nstat,statstr)
                    numGr += 1
        res = "loci description (%s)\n"%(numGr - 1)
        res += locidesc

        res += "\ngroup summary statistics (%s)\n"%(totNbStat)
        if self.ascert_frame.ascYesRadio.isChecked():
            nb = int(statsdesc.split('group G')[-1].split(')')[0].split('(')[1])
            num = int(statsdesc.split('group G')[-1].split()[0])
            statsdesc = statsdesc.replace('group G%s (%s)'%(num,nb),'group G%s (%s)'%(num,nb+1))
            statsdesc += "PPL\n"
        res += statsdesc
        res += "\n"

        if os.path.exists(self.dir+"/%s"%self.parent.gen_conf_name):
            os.remove("%s/%s" % (self.dir,self.parent.gen_conf_name))

        f = codecs.open(self.dir+"/%s"%self.parent.gen_conf_name,'w',"utf-8")
        f.write(res)
        f.close()

    def freezeHistModel(self,yesno=True):
        super(ProjectSnp,self).freezeHistModel(yesno)

        self.ascert_frame.ui.scrollArea.setDisabled(yesno)
        if yesno:
            if "Set " in str(self.ui.ascertButton.text()):
                print "1"
                self.ui.ascertButton.setText(str(self.ui.ascertButton.text()).replace("Set ","View "))
            elif "View " not in str(self.ui.ascertButton.text()):
                print "2"
                self.ui.ascertButton.setText("View "+str(self.ui.ascertButton.text()))
        else:
            if "View " in str(self.ui.ascertButton.text()):
                print "3"
                self.ui.ascertButton.setText(str(self.ui.ascertButton.text()).replace("View ","Set "))
            elif "Set " not in str(self.ui.ascertButton.text()):
                print "4"
                self.ui.ascertButton.setText("Set "+str(self.ui.ascertButton.text()))

    def freezeGenData(self,yesno=True):
        try:
            for g in self.sum_stat_wins.values():
                for e in g.findChildren(QLineEdit):
                    e.setDisabled(yesno)
                for e in g.findChildren(QPushButton):
                    e.setDisabled(yesno)
                for e in g.findChildren(QCheckBox):
                    e.setDisabled(yesno)
                g.ui.clearButton.setDisabled(yesno)
                g.ui.exitButton.setDisabled(yesno)
                g.ui.okButton.setDisabled(False)
                g.ui.addAdmixButton.setDisabled(yesno)
            if yesno:
                self.ui.setGeneticButton.setText("            View")
            else:
                self.ui.setGeneticButton.setText("            Set")
        except AttributeError as e:
            log(4,"Fail to freeze gen data")

