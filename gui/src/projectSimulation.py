# -*- coding: utf-8 -*-

## @package python_interface.projectSimulation
# @author Julien Veyssier
#
# @brief Projets pour simuler des données

import time
import os
import subprocess
from project import Project
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
from historicalModel.setHistoricalModelSimulation import SetHistoricalModelSimulation
from geneticData.setGenDataSimulation import SetGeneticDataSimulation
import os.path
import output
from utils.cbgpUtils import log
from datetime import datetime
import variables
from variables import UIPATH

## @class ProjectSimulation
# @brief Projets pour simuler des données
class ProjectSimulation(Project):
    """ classe qui représente un projet de simulation
    """
    def __init__(self,name,dir=None,parent=None):
        super(ProjectSimulation,self).__init__(name,dir,parent)

        self.dico_loc_nb = None
        self.sexRatio = None

        
        self.ui.projNameLabel.setText("Data file generic name :")
        self.ui.dirLabel.setText("Target directory :")
        self.ui.dirEdit.setText("%s"%self.dir)
        self.ui.runReftableButton.setText("Run data sets simulation")
        self.ui.nbSetsReqLabel.setText("Total number of data sets to be simulated :")
        fontLabel = QFont()
        fontLabel.setPointSize(14)
        self.ui.nbSetsReqLabel.setFont(fontLabel)
        self.ui.nbSetsReqLabel.setIndent(-1)
        self.ui.dataFileLabel.hide()
        self.ui.browseDataFileButton.hide()
        self.ui.dataFileEdit.hide()
        self.ui.dataFileInfoTitleLabel.hide()
        self.ui.dataFileInfoLabel.hide()
        self.ui.warningNumTextBrowser.hide()
        self.ui.dataSetsLabel.hide()
        self.ui.separationFrame.hide()

        self.ui.nbSetsDoneLabel.hide()
        self.ui.nbSetsDoneEdit.hide()

        self.ui.groupBox_6.show()
        self.ui.groupBox_7.show()
        self.ui.groupBox_8.show()

        self.ui.nbMicrosatLabel.setText("")
        self.ui.nbSequencesLabel.setText("")

        self.ui.removeTab(1)
        self.ui.setTabText(0,QString("Simulate data sets"))

        self.ui.setHistoricalButton.setDisabled(False)
        self.ui.setGeneticButton.setDisabled(False)

        self.hist_model_win = SetHistoricalModelSimulation(self)
        self.hist_model_win.hide()

        self.locusNumberFrame = uic.loadUi("%s/setLocusNumber.ui"%UIPATH)
        self.locusNumberFrame.setParent(self)
        self.locusNumberFrame.hide()
        QObject.connect(self.locusNumberFrame.okButton,SIGNAL("clicked()"),self.checkSampleNSetGenetic)

        self.connect(self.ui.runReftableButton, SIGNAL("clicked()"),self,SLOT("launchSimulation()"))
        self.connect(self.ui.stopReftableButton, SIGNAL("clicked()"),self.stopSimulation)

        self.ui.genDataLabel.setText('Genetic data')
        self.ui.progressBar.hide()

        self.dirCreation()

    def dirCreation(self):
        """ on crée un dossier
        """
        path = "{0}/{1}".format(self.dir,self.name)
        # name_YYYY_MM_DD-num le plus elevé
        dd = datetime.now()
        cd = 100
        while cd > 0 and not os.path.exists(path+"_%i_%i_%i-%i"%(dd.year,dd.month,dd.day,cd)):
            cd -= 1
        if cd == 100:
            output.notify(self,"Error","With this version, you cannot have more than 100 \
                        project directories\nfor the same project name and in the same directory")
        else:
            newdir = path+"_%i_%i_%i-%i"%(dd.year,dd.month,dd.day,(cd+1))
            self.ui.dirEdit.setText(newdir)
            try:
                os.mkdir(newdir)
                self.ui.setHistoricalButton.setDisabled(False)
                self.ui.setGeneticButton.setDisabled(False)
                self.dir = newdir
            except OSError,e:
                output.notify(self,"Error",str(e))
        self.initializeRNG()

    def canBeCloned(self):
        return False

    def canBeDeleted(self):
        return False

    def writeHeaderSim(self):
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
        fdest = open("%s/headersim.txt"%self.dir,"w")
        fdest.write("%s %s %s\n"%(self.name.replace(' ','_'),nb_rec,sexRatioTxt))
        fdest.write("%s\n\n"%self.hist_model_win.getConf())
        fdest.write(self.getLocusDescription())
        fdest.close()

    def stopUiGenReftable(self):
        self.ui.runReftableButton.setText("Run computations")
        self.ui.runReftableButton.setDisabled(False)

    def startUiGenReftable(self):
        self.ui.runReftableButton.setText("Running ...")
        self.ui.runReftableButton.setDisabled(True)

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

    def returnToMe(self):
        self.ui.refTableStack.removeWidget(self.ui.refTableStack.currentWidget())
        self.ui.refTableStack.setCurrentIndex(0)

    @pyqtSignature("")
    def launchSimulation(self):
        try:
            self.writeHeaderSim()
        except Exception as e:
            output.notify(self,"header writing error","%s"%e)
            return
        try:
            nb_to_gen = int(self.ui.nbSetsReqEdit.text())
        except Exception as e:
            output.notify(self,"value error","Check the value of required number of data sets\n\n%s"%e)
            return
        self.th = SimulationThread(self,nb_to_gen)
        self.th.connect(self.th,SIGNAL("simulationTerminated"),self.simulationTerminated)
        self.th.connect(self.th,SIGNAL("simulationProblem(QString)"),self.simulationProblem)
        self.th.connect(self.th,SIGNAL("simulationLog(int,QString)"),self.simulationLog)
        self.th.start()
        self.startUiGenReftable()

    def simulationProblem(self,msg):
        output.notify(self,"Simulation problem","Something happened during the simulation :\n %s"%(msg))
        self.stopSimulation()

    def simulationLog(self,lvl,msg):
        if self.th != None:
            log(lvl,str(msg))

    def simulationTerminated(self):
        """ Reception du signal de fin de simulation
        """
        self.parent.systrayHandler.showTrayMessage("DIYABC : simulation","Simulation has finished")
        self.stopUiGenReftable()
        self.ui.runReftableButton.setText("Simulation terminated.\nFiles created in target directory")
        self.th = None

    def stopSimulation(self):
        if self.th != None:
            self.th.terminate()
            self.th.killProcess()
            self.th = None
        if os.path.exists("%s/simulation.out"%(self.dir)):
            os.remove("%s/simulation.out"%(self.dir))
        log(1,"Simulation stopped")

class SimulationThread(QThread):
    """ thread de traitement qui met à jour la progressBar en fonction de l'avancée de
    la génération de la reftable
    """
    def __init__(self,parent,nb_to_gen):
        super(SimulationThread,self).__init__(parent)
        self.parent = parent
        self.nb_to_gen = nb_to_gen
        self.processus = None

        self.logmsg = ""
        self.loglvl = 3

    def log(self,lvl,msg):
        """ evite de manipuler les objets Qt dans un thread non principal
        """
        self.loglvl = lvl
        self.logmsg = msg
        clean_msg = msg.replace(u'\xb5',u'u')
        self.emit(SIGNAL("simulationLog(int,QString)"),lvl,clean_msg)

    def killProcess(self):
        self.log(3,"Attempting to kill simulation process")
        if self.processus != None:
            if self.processus.poll() == None:
                self.processus.kill()
                self.log(3,"Killing simulation process (pid:%s) DONE"%(self.processus.pid))

    def run(self):
        # lance l'executable
        outfile = "%s/simulation.out"%self.parent.dir
        if os.path.exists(outfile):
            os.remove(outfile)
        if os.path.exists("%s/progress.txt"%self.parent.dir):
            os.remove("%s/progress.txt"%self.parent.dir)
        fg = open(outfile,"w")
        try:
            self.log(2,"Running the executable for the simulation")
            exPath = self.parent.parent.preferences_win.getExecutablePath()
            nbMaxThread = self.parent.parent.preferences_win.getMaxThreadNumber()
            cmd_args_list = [exPath,"-p", "%s/"%self.parent.dir, "-k", "-m", "-t", "%s"%nbMaxThread]
            self.log(3,"Command launched : %s"%" ".join(cmd_args_list))
            p = subprocess.Popen(cmd_args_list, stdout=fg, stdin=subprocess.PIPE, stderr=subprocess.STDOUT) 
            self.processus = p
        except Exception as e:
            self.problem = "Problem during program launch \n%s"%e
            self.emit(SIGNAL("simulationProblem(QString)"),self.problem)
            fg.close()
            return

        # boucle toutes les secondes pour verifier les valeurs dans le fichier
        finished = False
        while not finished:
            time.sleep(1)
            # verification de l'arret du programme
            if p.poll() != None:
                # lecture 
                if os.path.exists("%s/progress.txt"%(self.parent.dir)):
                    #print 'open'
                    f = open("%s/progress.txt"%(self.parent.dir),"r")
                    lines = f.readlines()
                    f.close()
                    finished = True
                    self.log(2,"Simulation terminated normaly")
                    self.emit(SIGNAL("simulationTerminated"))
                else:
                    fg.close()
                    fout = open(outfile,'r')
                    lastline = fout.readlines()
                    if len(lastline) > 0:
                        lastline = lastline[-1]
                    fout.close()
                    self.problem = "Simulation program exited anormaly\n%s"%lastline
                    self.emit(SIGNAL("simulationProblem(QString)"),self.problem)
                    return
                    
                fg.close()
        fg.close()

class ProjectSimulationMsatSeq(ProjectSimulation):
    """ classe qui représente un projet de simulation SNP
    """
    def __init__(self,name,dir=None,parent=None):
        super(ProjectSimulationMsatSeq,self).__init__(name,dir,parent)

        QObject.connect(self.locusNumberFrame.mhEdit,SIGNAL("textChanged(QString)"),self.haploidChanged)
        QObject.connect(self.locusNumberFrame.shEdit,SIGNAL("textChanged(QString)"),self.haploidChanged)
        self.locusNumberFrame.mxEdit.setText('1')
        self.locusNumberFrame.mxEdit.setText('0')

        self.gen_data_win = SetGeneticDataSimulation(self)
        self.gen_data_win.hide()

    def haploidChanged(self,srt):
        try:
            mh = int(str(self.locusNumberFrame.mhEdit.text()))
            sh = int(str(self.locusNumberFrame.shEdit.text()))
            if sh > 0 or mh > 0:
                for ed in [self.locusNumberFrame.sxEdit,self.locusNumberFrame.mxEdit,self.locusNumberFrame.myEdit,self.locusNumberFrame.syEdit,self.locusNumberFrame.maEdit,self.locusNumberFrame.saEdit]:
                    ed.setDisabled(True)
                    ed.setText("0")
                self.locusNumberFrame.sexRatioEdit.setDisabled(True)
            else:
                for ed in [self.locusNumberFrame.sxEdit,self.locusNumberFrame.mxEdit,self.locusNumberFrame.myEdit,self.locusNumberFrame.syEdit,self.locusNumberFrame.maEdit,self.locusNumberFrame.saEdit]:
                    ed.setDisabled(False)
                self.locusNumberFrame.sexRatioEdit.setDisabled(False)
        except Exception as e:
            pass

    def checkSampleNSetGenetic(self):
        dico_loc_nb = {}
        editList = self.locusNumberFrame.findChildren(QLineEdit)
        editList.remove(self.locusNumberFrame.sexRatioEdit)
        nbXY = 0
        nbHap = 0
        nbDip = 0
        nbpos = 0
        nbmic = 0
        nbseq = 0
        try:
            for le in editList:
                val = int(le.text())
                dico_loc_nb[str(le.objectName())[:2]] = val
                if val < 0:
                    output.notify(self,"Number of sample error","Number of sample must be positive integers")
                    return
                elif val > 0:
                    nbpos += 1
                    if str(le.objectName())[0] == "s":
                        nbseq += val
                    if str(le.objectName())[0] == "m":
                        nbmic += val
                    if "x" in str(le.objectName())[:2] or "y" in str(le.objectName())[:2]:
                        nbXY += 1
                    elif "d" in str(le.objectName())[:2]:
                        nbDip += 1
                    elif "h" in str(le.objectName())[:2]:
                        nbHap += 1
            if nbpos == 0:
                output.notify(self,"Number of sample error","You must have at leat one sample")
                return
        except Exception as e:
            output.notify(self,"Number of sample error","Input error : \n%s"%e)
            return
        mh = int(str(self.locusNumberFrame.mhEdit.text()))
        sh = int(str(self.locusNumberFrame.shEdit.text()))
        # on ne prend en compte le sexratio que si l'on a pas d'haploid
        if sh == 0 and mh == 0:
            try:
                val = float(str(self.locusNumberFrame.sexRatioEdit.text()))
                if val < 0 or val > 1:
                    output.notify(self,"Value error","Sex ratio value must be in ]0,1[")
                    return
                self.sexRatio = val
            except Exception as e:
                output.notify(self,"Value error","Sex ratio value must be in ]0,1[")
                return

        # verification : a-t-on modifié le nb de loci ?
        if self.dico_loc_nb != None:
            changed = False
            for key in self.dico_loc_nb:
                if self.dico_loc_nb[key] != dico_loc_nb[key]:
                    changed = True
                    break
        else:
            changed = True

        self.dico_loc_nb = dico_loc_nb

        log(3,"Numbers of locus : %s"%dico_loc_nb)
        if changed:
            # on vide les gen data
            self.gen_data_win = SetGeneticDataSimulation(self)
            self.gen_data_win.hide()
            self.gen_data_win.fillLocusTable(dico_loc_nb)
            self.parent.updateDoc(self.gen_data_win)

        self.ui.refTableStack.removeWidget(self.ui.refTableStack.currentWidget())
        self.ui.refTableStack.addWidget(self.gen_data_win)
        self.ui.refTableStack.setCurrentWidget(self.gen_data_win)

        self.ui.nbMicrosatLabel.setText("{0} microsatellites loci".format(nbmic))
        self.ui.nbSequencesLabel.setText("{0} DNA sequences loci".format(nbseq))

    def getLocusDescription(self):
        return "%s"%self.gen_data_win.getConf().replace(u'\xb5','u')

class ProjectSimulationSnp(ProjectSimulation):
    """ classe qui représente un projet de simulation SNP
    """
    def __init__(self,name,dir=None,parent=None):
        super(ProjectSimulationSnp,self).__init__(name,dir,parent)

        self.locusNumberFrame.frame_8.hide()
        self.locusNumberFrame.leftBlocLabel.setText("Types of SNP loci \n H loci are not compatible with A, X and Y loci.")
        #self.locusNumberFrame.frame.setMaximumSize(QSize(300,1000))
        #self.locusNumberFrame.frame.setMinimumSize(QSize(300,0))

        #self.ui.ascertSimFrame.show()
        #self.ui.frame_11.show()

        QObject.connect(self.locusNumberFrame.mhEdit,SIGNAL("textChanged(QString)"),self.haploidChanged)
        self.ui.nbSequencesLabel.hide()
        self.ui.nbSumStatsLabel.hide()

    def haploidChanged(self,srt):
        try:
            mh = int(str(self.locusNumberFrame.mhEdit.text()))
            if mh > 0:
                for ed in [self.locusNumberFrame.mxEdit,self.locusNumberFrame.myEdit,self.locusNumberFrame.maEdit]:
                    ed.setDisabled(True)
                    ed.setText("0")
                self.locusNumberFrame.sexRatioEdit.setDisabled(True)
            else:
                for ed in [self.locusNumberFrame.mxEdit,self.locusNumberFrame.myEdit,self.locusNumberFrame.maEdit]:
                    ed.setDisabled(False)
                self.locusNumberFrame.sexRatioEdit.setDisabled(False)
        except Exception as e:
            pass

    def checkSampleNSetGenetic(self):
        dico_loc_nb = {}
        editList = [self.locusNumberFrame.maEdit,
                    self.locusNumberFrame.mhEdit,
                    self.locusNumberFrame.mxEdit,
                    self.locusNumberFrame.myEdit,
                    self.locusNumberFrame.mmEdit ]
        nbXY = 0
        nbHap = 0
        nbDip = 0
        nbpos = 0
        nbtot = 0
        try:
            for le in editList:
                val = int(le.text())
                dico_loc_nb[str(le.objectName())[:2]] = val
                nbtot += val
                if int(le.text()) < 0:
                    output.notify(self,"Number of sample error","Number of sample must be positive integers")
                    return
                elif int(le.text()) > 0:
                    nbpos += 1
                    if "x" in str(le.objectName())[:2] or "y" in str(le.objectName())[:2]:
                        nbXY += 1
                    elif "d" in str(le.objectName())[:2]:
                        nbDip += 1
                    elif "h" in str(le.objectName())[:2]:
                        nbHap += 1
            if nbpos == 0:
                output.notify(self,"Number of sample error","You must have at leat one sample")
                return
        except Exception as e:
            output.notify(self,"Number of sample error","Input error : \n%s"%e)
            return
        mh = int(str(self.locusNumberFrame.mhEdit.text()))
        sh = int(str(self.locusNumberFrame.shEdit.text()))
        # on ne prend en compte le sexratio que si l'on a pas d'haploid
        if mh == 0:
            try:
                val = float(str(self.locusNumberFrame.sexRatioEdit.text()))
                if val < 0 or val > 1:
                    output.notify(self,"Value error","Sex ratio value must be in [0,1]")
                    return
                self.sexRatio = val
            except Exception as e:
                output.notify(self,"Value error","Sex ratio value must be in [0,1]")
                return

        self.dico_loc_nb = dico_loc_nb

        log(3,"Numbers of locus : %s"%dico_loc_nb)
        self.returnToMe()
        self.setGenValid(True)
        self.ui.nbMicrosatLabel.setText("{0} SNP loci".format(nbtot))

    def getLocusDescription(self):
        nbLocis = 0
        for l in self.dico_loc_nb.keys():
            nbLocis+=self.dico_loc_nb[l]
        str_locis = "loci description ({0})\n".format(nbLocis)
        numGroup = 0
        for cat in ["A","H","X","Y","M"]:
            if self.dico_loc_nb.has_key("m"+cat.lower()) and self.dico_loc_nb["m"+cat.lower()] != 0:
                numGroup+=1
                str_locis += "{0} <{1}> [P] G{2}\n".format(self.dico_loc_nb["m"+cat.lower()],cat,numGroup)
        return str_locis


