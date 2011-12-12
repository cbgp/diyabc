# -*- coding: utf-8 -*-

## @package python_interface.projectReftable
# @author Julien Veyssier
#
# @brief Projets pour créer une table de référence

import hashlib,pickle,sys
import socket
from socket import *
import time
import os
import shutil
import codecs
import subprocess
import tarfile,stat
#from subprocess import Popen, PIPE, STDOUT 
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui,QtCore
from PyQt4 import uic
from analysis.defineNewAnalysis import DefineNewAnalysis
from analysis.drawEstimationAnalysisResult import DrawEstimationAnalysisResult
from analysis.drawComparisonAnalysisResult import DrawComparisonAnalysisResult
from analysis.drawPCAAnalysisResult import DrawPCAAnalysisResult
from analysis.viewAnalysisParameters import ViewAnalysisParameters
#from uis.viewTextFile_ui import Ui_Frame as ui_viewTextFile
from utils.data import DataSnp,isSNPDatafile
from datetime import datetime 
import os.path
from PyQt4.Qwt5 import *
from PyQt4.Qwt5.qplt import *
import output
from utils.cbgpUtils import log, addLine
import tempfile
import utils.cbgpUtils as cbgpUtils
from project import Project

## @class ProjectReftable
# @brief Projets pour créer une table de référence
class ProjectReftable(Project):
    """ classe qui représente un projet
    par defaut, un projet est considéré comme nouveau, cad que l'affichage est celui d'un projet vierge
    pour un projet chargé, on modifie l'affichage en conséquence dans loadFromDir
    """
    def __init__(self,name,dir=None,parent=None):
        self.dataFileSource = ""
        self.dataFileName = ""
        self.analysisList = []
        self.analysisQueue = []
        self.dicoFrameAnalysis = {}

        self.thAnalysis = None

        super(ProjectReftable,self).__init__(name,dir,parent)

        # manual alignment set
        self.ui.verticalLayout_2.setAlignment(self.ui.newAnButton,Qt.AlignCenter)
        self.ui.verticalLayout_3.setAlignment(self.ui.progressBar,Qt.AlignCenter)

        QObject.connect(self.ui.newAnButton,SIGNAL("clicked()"),self.defineNewAnalysis)
        QObject.connect(self.ui.browseDataFileButton,SIGNAL("clicked()"),self.dataFileSelectionAndCopy)
        QObject.connect(self.ui.browseDirButton,SIGNAL("clicked()"),self.dirCreation)

        self.connect(self.ui.runReftableButton, SIGNAL("clicked()"),self,SLOT("on_btnStart_clicked()"))
        self.connect(self.ui.stopReftableButton, SIGNAL("clicked()"),self.stopRefTableGen)
        #self.connect(self.ui.cancelButton, SIGNAL("clicked()"),self.cancelTh)

    def stopActivities(self):
        self.stopRefTableGen()
        self.stopAnalysis()
        self.unlock()

    def stopRefTableGen(self):
        """ tue le thread de génération et crée le fichier .stop pour 
        arrêter la génération de la reftable
        """
        if os.path.exists("%s"%self.dir):
            f = open("%s/.stop"%self.dir,"w")
            f.write(" ")
            f.close()
            time.sleep(1)
            need_to_start_analysis = (self.th != None and len(self.analysisQueue)>0)
            if self.th != None:
                self.th.terminate()
                self.th.killProcess()
                self.th = None
            #self.ui.reftableProgressLabel.setText("")
            self.stopUiGenReftable()
            if os.path.exists("%s/reftable.log"%(self.dir)):
                os.remove("%s/reftable.log"%(self.dir))
            log(1,"Generation of reference table stopped")
            # on démarre les analyses programmées
            if need_to_start_analysis:
                self.nextAnalysisInQueue()
            self.putRefTableSize()

    def stopAnalysis(self):
        if self.thAnalysis != None:
            analysis = self.thAnalysis.analysis
            analysis.status = "new"
            self.thAnalysis.terminate()
            self.thAnalysis.killProcess()
            self.thAnalysis = None

            # on met à jour l'aspect graphique
            for the_frame in self.dicoFrameAnalysis.keys():
                if analysis == self.dicoFrameAnalysis[the_frame]:
                    break
            the_frame.findChild(QPushButton,"analysisButton").setText("Re-launch")
            the_frame.findChild(QPushButton,"analysisButton").setStyleSheet("background-color: #EFB1B3")
            the_frame.findChild(QPushButton,"analysisStopButton").hide()

            # on met à jour les queue numbers
            for the_frame in self.dicoFrameAnalysis.keys():
                anatmp = self.dicoFrameAnalysis[the_frame]
                if anatmp in self.analysisQueue:
                    index = self.analysisQueue.index(anatmp)
                    the_frame.findChild(QPushButton,"analysisButton").setText("Queued (%s)"%index)
                    the_frame.findChild(QPushButton,"analysisButton").setStyleSheet("background-color: #F4992B")

            self.nextAnalysisInQueue()

    def viewAnalysisResult(self,analysis=None):
        """ en fonction du type d'analyse, met en forme les résultats
        """
        log(1,"Asking results of analysis %s, project %s"%(analysis.name,self.dir))
        #anCat = analysis.category
        dicoCategoryDirName = {
                "estimate" : "estimation",
                "compare" : "comparison",
                "evaluate" : "evaluation",
                "modelChecking" : "modelChecking",
                "pre-ev" : "pca",
                "bias" : "bias"
                }
        anDir = dicoCategoryDirName[analysis.category]
        anDir += "_"
        anDir += analysis.name

        typestr = dicoCategoryDirName[analysis.category]
        if typestr == 'estimation':
            self.drawAnalysisFrame = DrawEstimationAnalysisResult(analysis,anDir,self)
        elif typestr == 'comparison':
            self.drawAnalysisFrame = DrawComparisonAnalysisResult(analysis,anDir,self)
        elif typestr == "pca" or typestr == "modelChecking":
            if os.path.exists("%s/analysis/%s/ACP.txt"%(self.dir,anDir)) or os.path.exists("%s/analysis/%s/mcACP.txt"%(self.dir,anDir)):
                self.drawAnalysisFrame = DrawPCAAnalysisResult(analysis,anDir,self)
            else:
                if typestr == "pca":
                    f = open("%s/analysis/%s/locate.txt"%(self.dir,anDir),'r')
                elif typestr == "modelChecking":
                    f = open("%s/analysis/%s/mclocate.txt"%(self.dir,anDir),'r')
                data = f.read()
                f.close()
                #self.drawAnalysisFrame = QFrame(self)
                self.drawAnalysisFrame = uic.loadUi("uis/viewTextFile.ui")
                self.drawAnalysisFrame.parent = self
                ui = self.drawAnalysisFrame
                log(3,"Viewing analysis results, PCA or modelChecking")
                #ui = ui_viewTextFile()
                #ui.setupUi(self.drawAnalysisFrame)
                ui.dataPlain.setPlainText(data)
                ui.dataPlain.setLineWrapMode(0)
                font = "FreeMono"
                if sys.platform.startswith('win'):
                    font = "Courier New"
                elif "darwin" in sys.platform:
                    font = "Andale Mono"
                ui.dataPlain.setFont(QFont(font,10))
                QObject.connect(ui.okButton,SIGNAL("clicked()"),self.returnToAnalysisList)

        elif typestr == "evaluation":
            f = open("%s/analysis/%s/confidence.txt"%(self.dir,anDir),'r')
            data = f.read()
            f.close()
            #self.drawAnalysisFrame = QFrame(self)
            self.drawAnalysisFrame = uic.loadUi("uis/viewTextFile.ui")
            self.drawAnalysisFrame.parent = self
            ui = self.drawAnalysisFrame
            log(3,"Viewing analysis results, confidence in scenario choice")
            #ui = ui_viewTextFile()
            #ui.setupUi(self.drawAnalysisFrame)
            ui.dataPlain.setPlainText(data)
            ui.dataPlain.setLineWrapMode(0)
            font = "FreeMono"
            if sys.platform.startswith('win'):
                font = "Courier New"
            elif "darwin" in sys.platform:
                font = "Andale Mono"
            ui.dataPlain.setFont(QFont(font,10))
            QObject.connect(ui.okButton,SIGNAL("clicked()"),self.returnToAnalysisList)
        elif typestr == "bias":
            f = open("%s/analysis/%s/bias.txt"%(self.dir,anDir),'r')
            data = f.read()
            f.close()
            #self.drawAnalysisFrame = QFrame(self)
            self.drawAnalysisFrame = uic.loadUi("uis/viewTextFile.ui")
            self.drawAnalysisFrame.parent = self
            ui = self.drawAnalysisFrame
            log(3,"Viewing analysis results, bias")
            #ui = ui_viewTextFile()
            #ui.setupUi(self.drawAnalysisFrame)
            ui.dataPlain.setPlainText(data)
            ui.dataPlain.setLineWrapMode(0)
            font = "FreeMono"
            if sys.platform.startswith('win'):
                font = "Courier New"
            elif "darwin" in sys.platform:
                font = "Andale Mono"
            ui.dataPlain.setFont(QFont(font,10))
            QObject.connect(ui.okButton,SIGNAL("clicked()"),self.returnToAnalysisList)
        #elif typestr == "evaluation":
        #    self.drawAnalysisFrame = DrawEvaluationAnalysisResult(anDir,self)
        self.ui.analysisStack.addWidget(self.drawAnalysisFrame)
        self.ui.analysisStack.setCurrentWidget(self.drawAnalysisFrame)

        if (typestr == "pca" or typestr == "modelChecking")\
        and (os.path.exists("%s/analysis/%s/ACP.txt"%(self.dir,anDir))\
        or os.path.exists("%s/analysis/%s/mcACP.txt"%(self.dir,anDir))):
            self.drawAnalysisFrame.loadACP()

    def returnToAnalysisList(self):
        self.ui.analysisStack.removeWidget(self.drawAnalysisFrame)
        self.ui.analysisStack.setCurrentIndex(0)

    def returnTo(self,elem):
        self.ui.analysisStack.removeWidget(self.drawAnalysisFrame)
        self.ui.analysisStack.setCurrentWidget(elem)

    def defineNewAnalysis(self):
        """ démarre la définition d'une nouvelle analyse
        """
        log(1,"Entering in new analysis definition")
        def_analysis = DefineNewAnalysis(self,self.hist_model_win.getNbScenario())
        #self.addTab(def_analysis,"Define new analysis")

        #self.setTabEnabled(0,False)
        #self.setTabEnabled(1,False)
        #self.setCurrentWidget(def_analysis)
        self.ui.analysisStack.addWidget(def_analysis)
        self.ui.analysisStack.setCurrentWidget(def_analysis)

    def genMasterScript(self):
        """ génération du script à exécuter sur le noeud maitre
        """
        nbToGen = int(self.ui.nbSetsReqEdit.text())
        nbFullQsub = nbToGen / 10000
        nbLastQsub = nbToGen % 10000

        if nbLastQsub == 0:
            nbQsub = nbFullQsub
        else:
            nbQsub = nbFullQsub+1

        res = '\n\
function progress(){\n\
res=0\n\
nbfin=%s\n\
sum=0\n\
# for each log file, if it exists, do the sum of done values\n\
for i in $(seq 1 $1); do\n\
    if [ -e reftable_$i.log ]; then\n\
        let sum=$sum+`head -n 2 reftable_$i.log | tail -n 1`\n\
    fi\n\
done\n\
let pct=$sum*100\n\
let pct=$pct/$nbfin\n\
echo `nbOk $1` "/ $1 finished"\n\
echo " $pct %%"\n\
echo "somme $sum"\n\
}\n\
function nbOk(){\n\
nb=0\n\
# for each log file, check if the computation is terminated\n\
for i in $(seq 1 $1); do\n\
    if [ -e reftable_$i.log ]; then\n\
        numdone=`head -n 2 reftable_$i.log | tail -n 1`\n'%nbToGen

        if nbLastQsub != 0:
            res+='        # case of last computation, less thant 10000\n\
        if [ $i -eq %s ]; then\n\
            if [ $numdone -eq %s ]; then\n\
                let nb=$nb+1\n\
            fi\n\
        else\n\
            if [ $numdone -eq 10000 ]; then\n\
                let nb=$nb+1\n\
            fi\n\
        fi\n'%(nbFullQsub+1,nbLastQsub)
        else:
            res+='        if [ $numdone -eq 10000 ]; then\n\
            let nb=$nb+1\n\
        fi\n'

        res+='    fi\n\
done\n\
echo $nb\n\
}\n\
seed=$RANDOM\n\
for i in $(seq 1 %s); do \n\
let seed=$seed+1\n\
qsub -q short_queue.q -cwd node.sh 10000 `pwd` $i $seed\n\
done;\n'%nbFullQsub
        
        if nbLastQsub != 0:
            res+='let last=$i+1\n\
let seed=$seed+1\n\
qsub -q short_queue.q -cwd node.sh %s `pwd` $last $seed\n'%nbLastQsub
        
        res+='while ! [ "`nbOk %s`" = "%s" ]; do\n\
echo `progress %s`\n\
sleep 3\n\
done\n\
echo `progress %s`\n\
./general -p "`pwd`"/ -q\n\
'%(nbQsub,nbQsub,nbQsub,nbQsub)
        
        return res

    def genNodeScript(self):
        """ génération du script a exécuter sur chaque noeud
        """

        return 'if ! [ -d $TMPDIR ]; then mkdir -p $TMPDIR ; fi \n\
cp general $TMPDIR\n\
cp %s $TMPDIR\n\
cp %s $TMPDIR\n\
$TMPDIR/general -s "$4" -p $TMPDIR/ -r $1 & \n\
while ! [ "`head -n 2 $TMPDIR/reftable.log | tail -n 1`" -eq $1 ]; do\n\
    cp $TMPDIR/reftable.log $2/reftable_$3.log\n\
    sleep 5\n\
done\n\
cp $TMPDIR/reftable.bin $2/reftable_$3.bin\n\
cp $TMPDIR/reftable.log $2/reftable_$3.log\n\
'%(self.parent.reftableheader_name, self.dataFileName)

    def generateComputationTar(self,tarname=None):
        """ génère une archive tar contenant l'exécutable, les scripts node et master,
        le datafile et le reftableheader.
        """
        if tarname == None:
            tarname = str(QFileDialog.getSaveFileName(self,"Saving","Reftable generation archive","(TAR archive) *.tar"))
        if tarname != "":
            #executablePath = str(self.parent.preferences_win.ui.execPathEdit.text())
            executablePath = self.parent.preferences_win.getExecutablePath()
            # generation du master script
            script = self.genMasterScript()
            scmffile = "%s/scmf"%self.dir
            if os.path.exists(scmffile):
                os.remove(scmffile)
            scmf = open(scmffile,'w')
            scmf.write(script)
            scmf.close()
            os.chmod(scmffile,stat.S_IRUSR|stat.S_IWUSR|stat.S_IXUSR|stat.S_IRGRP|stat.S_IWGRP|stat.S_IXGRP|stat.S_IROTH|stat.S_IWOTH|stat.S_IXOTH)
            tar = tarfile.open(tarname,"w")
            # generation du node script
            script = self.genNodeScript()
            scnffile = "%s/scnf"%self.dir
            if os.path.exists(scnffile):
                os.remove(scnffile)
            scnf = open(scnffile,'w')
            scnf.write(script)
            scnf.close()
            os.chmod(scnffile,stat.S_IRUSR|stat.S_IWUSR|stat.S_IXUSR|stat.S_IRGRP|stat.S_IWGRP|stat.S_IXGRP|stat.S_IROTH|stat.S_IWOTH|stat.S_IXOTH)

            # ajout des fichiers dans l'archive
            tarRepName = self.dir.split('/')[-1]
            if os.path.exists(tarname):
                os.remove(tarname)
            tar = tarfile.open(tarname,"w")
            tar.add(scmffile,'%s/launch.sh'%tarRepName)
            tar.add(scnffile,'%s/node.sh'%tarRepName)
            tar.add("%s/%s"%(self.dir,self.parent.reftableheader_name),"%s/%s"%(tarRepName,self.parent.reftableheader_name))
            tar.add(self.dataFileSource,"%s/%s"%(str(tarRepName),str(self.dataFileName)))
            tar.add(executablePath,"%s/general"%tarRepName)
            tar.close()

            # nettoyage des fichiers temporaires de script
            if os.path.exists(scmffile):
                os.remove(scmffile)
            if os.path.exists(scnffile):
                os.remove(scnffile)

        return tarname

    @pyqtSignature("")
    def on_btnStart_clicked(self):
        """ Lance un thread de generation de la reftable
        """
        log(1,"Starting generation of the reference table")

        self.save()
        self.writeRefTableHeader()
        if self.th == None and self.thAnalysis == None:
            self.parent.preferences_win.checkRam()
            if not os.path.exists("%s/reftable.bin"%self.dir) and os.path.exists("%s/reftable.log"%self.dir):
                os.remove("%s/reftable.log"%self.dir)
            try:
                nb_to_gen = int(self.ui.nbSetsReqEdit.text())
            except Exception as e:
                output.notify(self,"value error","Check the value of required number of data sets\n\n%s"%e)
                return
            self.startUiGenReftable()
            # on demarre le thread local ou cluster
            if self.parent.preferences_win.ui.useServerCheck.isChecked():
                #tname = self.generateComputationTar("/tmp/aaaa.tar")
                tname = self.generateComputationTar("%s/aaaa.tar"%tempfile.mkdtemp())
                log(3,"Tar file created in %s"%tname)
                self.th = RefTableGenThreadCluster(self,tname,nb_to_gen)
            else:
                self.th = RefTableGenThread(self,nb_to_gen)
            self.th.connect(self.th,SIGNAL("increment(int,QString)"),self.incProgress)
            self.th.connect(self.th,SIGNAL("refTableProblem(QString)"),self.refTableProblem)
            self.th.connect(self.th,SIGNAL("refTableLog(int,QString)"),self.refTableLog)
            #self.ui.progressBar.connect (self, SIGNAL("canceled()"),self.th,SLOT("cancel()"))
            self.th.start()
        else:
            #if not self.th.isRunning():
            #    self.th = None
            #    self.on_btnStart_clicked()
            output.notify(self,"Impossible to launch","An analysis is processing,\
                    \nimpossible to launch reftable generation")

    def refTableProblem(self,msg):
        output.notify(self,"reftable problem","Something happened during the reftable generation :\n %s"%(msg))
        #self.stopUiGenReftable()
        self.stopRefTableGen()
        self.nextAnalysisInQueue()

    def refTableLog(self,lvl,msg):
        if self.th != None:
            log(lvl,msg)

    def stopUiGenReftable(self):
        self.ui.runReftableButton.setText("Run computations")
        self.ui.runReftableButton.setDisabled(False)
        self.ui.progressBar.hide()

    def startUiGenReftable(self):
        self.ui.runReftableButton.setText("Running ...")
        self.ui.runReftableButton.setDisabled(True)
        self.ui.progressBar.show()
 
    def incProgress(self,done,time_remaining):
        """Incremente la barre de progression de la génération de la reftable
        """
        #done = self.th.nb_done
        nb_to_do = self.th.nb_to_gen
        #time_remaining = self.th.time
        #self.ui.reftableProgressLabel.setText('%s remaining'%time_remaining)
        self.ui.runReftableButton.setText("Running ... %s remaining"%(time_remaining.replace('\n',' ')))
        pc = (float(done)/float(nb_to_do))*100
        if pc > 0 and pc < 1:
            pc = 1
        self.ui.progressBar.setValue(int(pc))
        self.ui.nbSetsDoneEdit.setText("%s"%done)
        # si on a fini, on met à jour l'affichage de la taille de la reftable
        # et on verrouille eventuellement histmodel et gendata
        if int(pc) == 100:
            self.parent.showTrayMessage("DIYABC : reftable","Reference table generation has finished")
            self.putRefTableSize()
            self.stopUiGenReftable()
            self.th = None
            self.nextAnalysisInQueue()

    def cancelTh(self):
        #print 'plop'
        self.emit(SIGNAL("canceled()"))

    def writeRefTableHeader(self):
        """ écriture du header.txt à partir des conf
        """
        if os.path.exists(self.dir+"/%s"%self.parent.main_conf_name) and os.path.exists(self.dir+"/%s"%self.parent.hist_conf_name) and os.path.exists(self.dir+"/%s"%self.parent.gen_conf_name) and os.path.exists(self.dir+"/%s"%self.parent.table_header_conf_name):
            if os.path.exists(self.dir+"/%s"%self.parent.reftableheader_name):
                os.remove("%s/%s" %(self.dir,self.parent.reftableheader_name))
            log(2,"Writing reference table header from the conf files")

            fdest = codecs.open(self.dir+"/%s"%self.parent.reftableheader_name,"w","utf-8")
            for name in [self.parent.main_conf_name,self.parent.hist_conf_name,self.parent.gen_conf_name,self.parent.table_header_conf_name]:
                fo = codecs.open(self.dir+"/%s"%name,"r","utf-8")
                folines = fo.read()
                fo.close()
                if name == self.parent.hist_conf_name:
                    fdest.write(folines.replace(' True\n','\n').replace(' False\n','\n'))
                else:
                    fdest.write(folines)
            fdest.close()
        else:
            output.notify(self,"Header generation problem","One conf file is missing in order to generate the reference table header")

    def dataFileSelectionAndCopy(self,name=None):
        """ dialog pour selectionner le fichier à lire
        il est lu et vérifié. S'il est invalide, on garde la sélection précédente
        S'il est valide, on le copie dans le dossier du projet
        """
        if name == None:
            name = QFileDialog.getOpenFileName(self,"Select datafile")
        if isSNPDatafile(name):
            try:
                data = DataSnp(name)
            except Exception as e:
                output.notify(self,"Data file error","%s"%(e))
                return
            shutil.copy(name,"%s/%s"%(self.dir,name.split('/')[-1]))
            f = codecs.open(self.dir+"/%s"%self.parent.main_conf_name,'w',"utf-8")
            f.write("%s\n"%name.split('/')[-1])
            f.write("0 parameters and 0 summary statistics\n\n")
            f.close()
            self.parent.closeCurrentProject(save=False)
            self.parent.openProject(self.dir)
        elif self.loadDataFile(name):
            # si on a reussi a charger le data file, on vire le bouton browse
            self.ui.browseDataFileButton.hide()
            # et on copie ce datafile dans le dossier projet
            shutil.copy(self.dataFileSource,"%s/%s"%(self.dir,self.dataFileSource.split('/')[-1]))
            self.dataFileName = self.dataFileSource.split('/')[-1]
            #self.ui.groupBox.show()
            self.ui.groupBox_6.show()
            self.ui.groupBox_7.show()
            self.ui.groupBox_8.show()
            # comme on a lu le datafile, on peut remplir le tableau de locus dans setGeneticData
            self.gen_data_win.fillLocusTableFromData()

    def dirCreation(self,path):
        """ selection du repertoire pour un nouveau projet et copie du fichier de données
        """
        log(2,"Creation and files copy of folder '%s'"%path)
        if path != "":
            if not self.parent.isProjDir(path):
                # name_YYYY_MM_DD-num le plus elevé
                dd = datetime.now()
                #num = 36
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
                        #self.ui.groupBox.show()
                        self.ui.setHistoricalButton.setDisabled(False)
                        self.ui.setGeneticButton.setDisabled(False)
                        self.dir = newdir
                        # verrouillage du projet
                        self.lock()
                    except OSError,e:
                        output.notify(self,"Error",str(e))
            else:
                output.notify(self,"Incorrect directory","A project can not be in a project directory")

    def dirSelection(self,name=None):
        """ selection du repertoire pour un nouveau projet et copie du fichier de données
        """
        if name == None:
            qfd = QFileDialog()
            #qfd.setDirectory("~/")
            name = str(qfd.getExistingDirectory())
        if name != "":
            if not self.parent.isProjDir(name):
                # name_YYYY_MM_DD-num le plus elevé
                dd = datetime.now()
                #num = 36
                cd = 100
                while cd > 0 and not os.path.exists(name+"/%s_%i_%i_%i-%i"%(self.name,dd.year,dd.month,dd.day,cd)):
                    cd -= 1
                if cd == 100:
                    output.notify(self,"Error","With this version, you cannot have more than 100 \
                                project directories\nfor the same project name and in the same directory")
                else:
                    newdir = name+"/%s_%i_%i_%i-%i"%(self.name,dd.year,dd.month,dd.day,(cd+1))
                    self.ui.dirEdit.setText(newdir)
                    try:
                        os.mkdir(newdir)
                        self.ui.groupBox.show()
                        self.ui.setHistoricalButton.setDisabled(False)
                        self.ui.setGeneticButton.setDisabled(False)
                        self.dir = newdir
                        shutil.copy(self.dataFileSource,"%s/%s"%(self.dir,self.dataFileSource.split('/')[-1]))
                        self.dataFileName = self.dataFileSource.split('/')[-1]
                        # verrouillage du projet
                        self.lock()
                        # on a reussi a creer le dossier, on vire le bouton browse
                        self.ui.browseDirButton.hide()
                    except OSError,e:
                        output.notify(self,"Error",str(e))
            else:
                output.notify(self,"Incorrect directory","A project can not be in a project directory")

    def addAnalysis(self,analysis):
        """ ajoute, dans la liste d'analyses et dans la GUI , l'analyse passée en paramètre
        """
        log(1,"Adding analysis '%s'"%analysis.name)
        type_analysis = analysis.category
        self.analysisList.append(analysis)

        log(3,u"Computation parameters : %s"%analysis.computationParameters.replace(u'\xb5','u'))
        if type_analysis == "pre-ev":
            #self.addRow("scenario prior combination",analysis[1],"4","new")
            self.addAnalysisGui(analysis,analysis.name,"evaluate scenarios and priors",analysis.computationParameters,analysis.status)
        elif type_analysis == "estimate":
            #self.addRow("parameter estimation","params","5","new")
            self.addAnalysisGui(analysis,analysis.name,"estimate parameters","params",analysis.status)
        elif type_analysis == "bias":
            #self.addRow("bias and precision",str(analysis[2]),"3","new")
            self.addAnalysisGui(analysis,analysis.name,"bias and precision",str(analysis.chosenSc),analysis.status)
        elif type_analysis == "compare":
            #print "\n",analysis[-1],"\n"
            #self.addRow("scenario choice",analysis[2]["de"],"4","new")
            self.addAnalysisGui(analysis,analysis.name,"compare scenarios","%s | %s"%(analysis.candidateScList,analysis.chosenSc),analysis.status)
        elif type_analysis == "evaluate":
            #self.addRow("evaluate confidence","%s | %s"%(analysis[2],analysis[3]),"3","new")
            self.addAnalysisGui(analysis,analysis.name,"confidence in scenario choice","%s | %s"%(analysis.candidateScList,analysis.chosenSc),analysis.status)
        elif type_analysis == "modelChecking":
            self.addAnalysisGui(analysis,analysis.name,"model checking","%s | %s"%(analysis.candidateScList,analysis.chosenSc),analysis.status)

        #self.save()

    def addAnalysisGui(self,analysis,name,atype,params,status):
        """ crée les objets graphiques pour une analyse et les ajoute
        """

        frame_9 = QtGui.QFrame(self.ui.scrollAreaWidgetContents)
        frame_9.setFrameShape(QtGui.QFrame.StyledPanel)
        frame_9.setFrameShadow(QtGui.QFrame.Raised)
        frame_9.setObjectName("frame_9")
        frame_9.setMinimumSize(QtCore.QSize(0, 32))
        frame_9.setMaximumSize(QtCore.QSize(9999, 32))
        horizontalLayout_4 = QtGui.QHBoxLayout(frame_9)
        horizontalLayout_4.setObjectName("horizontalLayout_4")
        analysisRmButton = QtGui.QPushButton("Remove",frame_9)
        analysisRmButton.setMinimumSize(QtCore.QSize(70, 0))
        analysisRmButton.setMaximumSize(QtCore.QSize(70, 16777215))
        analysisRmButton.setObjectName("analysisRmButton")
        horizontalLayout_4.addWidget(analysisRmButton)
        #analysisUpButton = QtGui.QPushButton("up",frame_9)
        #analysisUpButton.setMinimumSize(QtCore.QSize(50, 0))
        #analysisUpButton.setMaximumSize(QtCore.QSize(50, 16777215))
        #analysisUpButton.setObjectName("analysisUpButton")
        #horizontalLayout_4.addWidget(analysisUpButton)
        #analysisDownButton = QtGui.QPushButton("down",frame_9)
        #analysisDownButton.setMinimumSize(QtCore.QSize(50, 0))
        #analysisDownButton.setMaximumSize(QtCore.QSize(50, 16777215))
        #analysisDownButton.setObjectName("analysisDownButton")
        #horizontalLayout_4.addWidget(analysisDownButton)
        analysisNameLabel = QtGui.QLabel(name,frame_9)
        analysisNameLabel.setAlignment(Qt.AlignCenter)
        analysisNameLabel.setMinimumSize(QtCore.QSize(140, 0))
        analysisNameLabel.setMaximumSize(QtCore.QSize(140, 16777215))
        analysisNameLabel.setObjectName("analysisNameLabel")
        analysisNameLabel.setFrameShape(QtGui.QFrame.StyledPanel)
        #analysisNameLabel.setMaximumSize(QtCore.QSize(70, 16777215))
        horizontalLayout_4.addWidget(analysisNameLabel)
        analysisTypeLabel = QtGui.QLabel(atype,frame_9)
        analysisTypeLabel.setAlignment(Qt.AlignCenter)
        analysisTypeLabel.setObjectName("analysisTypeLabel")
        analysisTypeLabel.setFrameShape(QtGui.QFrame.StyledPanel)
        analysisTypeLabel.setMaximumSize(QtCore.QSize(200, 16777215))
        analysisTypeLabel.setMinimumSize(QtCore.QSize(200, 0))
        horizontalLayout_4.addWidget(analysisTypeLabel)
        #analysisParamsLabel = QtGui.QLabel(str(params),frame_9)
        #analysisParamsLabel.setAlignment(Qt.AlignCenter)
        #analysisParamsLabel.setObjectName("analysisParamsLabel")
        #analysisParamsLabel.setFrameShape(QtGui.QFrame.StyledPanel)
        #analysisParamsLabel.setMaximumSize(QtCore.QSize(200, 16777215))
        #analysisParamsLabel.setMinimumSize(QtCore.QSize(200, 0))
        #analysisParamsLabel.setMaximumSize(QtCore.QSize(70, 16777215))
        #horizontalLayout_4.addWidget(analysisParamsLabel)
        analysisParamsButton = QtGui.QPushButton("View values",frame_9)
        analysisParamsButton.setObjectName("analysisParamsLabel")
        analysisParamsButton.setMaximumSize(QtCore.QSize(100, 16777215))
        analysisParamsButton.setMinimumSize(QtCore.QSize(100, 0))
        analysisParamsButton.setMaximumSize(QtCore.QSize(70, 16777215))
        horizontalLayout_4.addWidget(analysisParamsButton)
        #analysisStatusLabel = QtGui.QLabel(status,frame_9)
        #analysisStatusLabel.setMinimumSize(QtCore.QSize(40, 0))
        #analysisStatusLabel.setMaximumSize(QtCore.QSize(40, 16777215))
        #analysisStatusLabel.setAlignment(Qt.AlignCenter)
        #analysisStatusLabel.setFrameShape(QtGui.QFrame.StyledPanel)
        #analysisStatusLabel.setObjectName("analysisStatusLabel")
        #analysisParamsLabel.setMaximumSize(QtCore.QSize(70, 16777215))
        #horizontalLayout_4.addWidget(analysisStatusLabel)
        analysisStatusBar = QtGui.QProgressBar(frame_9)
        analysisStatusBar.setMinimumSize(QtCore.QSize(110, 0))
        analysisStatusBar.setMaximumSize(QtCore.QSize(110, 16777215))
        analysisStatusBar.setAlignment(Qt.AlignCenter)
        #analysisStatusBar.setFrameShape(QtGui.QFrame.StyledPanel)
        analysisStatusBar.setObjectName("analysisStatusBar")
        horizontalLayout_4.addWidget(analysisStatusBar)
        if status == "finished":
            buttonLabel = "View results"
            analysisStatusBar.setValue(100)
        else:
            buttonLabel = "Launch"
            analysisStatusBar.setValue(0)
        analysisButton = QtGui.QPushButton(buttonLabel,frame_9)
        analysisButton.setObjectName("analysisButton")
        analysisButton.setMinimumSize(QtCore.QSize(90, 0))
        analysisButton.setMaximumSize(QtCore.QSize(90, 16777215))
        if status == "finished":
            analysisButton.setStyleSheet("background-color: #79D8FF")
        horizontalLayout_4.addWidget(analysisButton)

        frameStop = QFrame(frame_9)
        frameStop.setFrameShape(QtGui.QFrame.NoFrame)
        frameStop.setObjectName("frameStop")
        frameStop.setMinimumSize(QtCore.QSize(70, 25))
        frameStop.setMaximumSize(QtCore.QSize(70, 25))
        horizontalLayout_stop = QtGui.QHBoxLayout(frameStop)
        horizontalLayout_stop.setObjectName("horizontalLayout_stop")
        horizontalLayout_stop.setContentsMargins(-1, 1, -1, 1)

        analysisStopButton = QtGui.QPushButton(QIcon("docs/icons/stop.png"),"Stop",frame_9)
        analysisStopButton.setObjectName("analysisStopButton")
        analysisStopButton.setMinimumSize(QtCore.QSize(60, 20))
        analysisStopButton.setMaximumSize(QtCore.QSize(60, 20))
        horizontalLayout_stop.addWidget(analysisStopButton)
        analysisStopButton.hide()

        horizontalLayout_4.addWidget(frameStop)

        horizontalLayout_4.setContentsMargins(-1, 1, -1, 1)
        self.ui.verticalLayout_9.addWidget(frame_9)

        self.dicoFrameAnalysis[frame_9] = analysis
        QObject.connect(analysisRmButton,SIGNAL("clicked()"),self.removeAnalysis)
        #QObject.connect(analysisDownButton,SIGNAL("clicked()"),self.moveAnalysisDown)
        #QObject.connect(analysisUpButton,SIGNAL("clicked()"),self.moveAnalysisUp)
        QObject.connect(analysisButton,SIGNAL("clicked()"),self.tryLaunchViewAnalysis)
        QObject.connect(analysisParamsButton,SIGNAL("clicked()"),self.viewAnalysisParameters)
        QObject.connect(analysisStopButton,SIGNAL("clicked()"),self.stopAnalysis)

    def removeAnalysis(self):
        frame = self.sender().parent()
        analysis = self.dicoFrameAnalysis[frame]
        log(1,"Removing analysis '%s'"%analysis.name)
        frame.hide()
        self.analysisList.remove(analysis)

        # on la vire de la queue si elle y était (dans le cas ou elle etait
        # running, elle n'y etait plus)
        if analysis in self.analysisQueue:
            log(1,"'%s' was in the queue, i deleted it"%analysis.name)
            self.analysisQueue.remove(analysis)
        # si elle tournait, on arrete le thread et on lance la suivante
        if self.thAnalysis != None and self.thAnalysis.analysis == analysis:
            log(1,"'%s' was running, i stopped the thread and launched the follower"%analysis.name)
            self.thAnalysis.terminate()
            self.thAnalysis.killProcess()
            self.thAnalysis = None
            self.nextAnalysisInQueue()

        # on met à jour les queue numbers
        for the_frame in self.dicoFrameAnalysis.keys():
            anatmp = self.dicoFrameAnalysis[the_frame]
            if anatmp in self.analysisQueue:
                index = self.analysisQueue.index(anatmp)
                the_frame.findChild(QPushButton,"analysisButton").setText("Queued (%s)"%index)
                the_frame.findChild(QPushButton,"analysisButton").setStyleSheet("background-color: #F4992B")

        del self.dicoFrameAnalysis[frame]
        self.ui.verticalLayout_9.removeWidget(frame)
        #self.save()

    def viewAnalysisParameters(self):
        """ bascule sur une frame qui affiche les valeurs des paramètres
        d'une analyse
        """
        frame = self.sender().parent()
        # on associe l'analyse a sa frame
        analysis = self.dicoFrameAnalysis[frame]
        log(1,"View parameters of analysis '%s'"%analysis.name)
        viewFrame = ViewAnalysisParameters(self,analysis)
        self.ui.analysisStack.addWidget(viewFrame)
        self.ui.analysisStack.setCurrentWidget(viewFrame)


    def tryLaunchViewAnalysis(self):
        """ clic sur le bouton launch/view d'une analyse
        """
        if not os.path.exists("%s/analysis/"%self.dir):
            os.mkdir("%s/analysis/"%self.dir)

        frame = self.sender().parent()
        # on associe l'analyse a sa frame
        analysis = self.dicoFrameAnalysis[frame]
        log(1,"Trying to launch analysis %s (%s)"%(analysis.name,analysis.status))
        if analysis.status == "finished":
            self.viewAnalysisResult(analysis)
        elif analysis.status == "running":
            # on la stoppe
            self.stopAnalysis()
        elif analysis.status == "queued":
            output.notify(self,"Action impossible","This analysis is already queued")
        else:
            # on veut la lancer
            self.analysisQueue.append(analysis)
            if analysis == self.analysisQueue[0] and self.thAnalysis == None and self.th == None:
                # si c'est la premiere, qu'aucune autre ne tourne et que la generation de 
                # reftable est down, on la lance
                self.launchAnalysis(analysis)
            else:
                # sinon on la met en wait
                index = self.analysisQueue.index(analysis)
                log(3,"I try to queue %s at index %s"%(analysis.name,index))
                frame.findChild(QPushButton,"analysisButton").setText("Queued (%s)"%index)
                frame.findChild(QPushButton,"analysisButton").setStyleSheet("background-color: #F4992B")
                analysis.status = "queued"

    def launchAnalysis(self,analysis):            
        """ lance un thread de traitement d'une analyse
        """
        if self.thAnalysis == None:
            # on retrouve la frame
            the_frame = None
            for frame in self.dicoFrameAnalysis.keys():
                if self.dicoFrameAnalysis[frame] == analysis:
                    the_frame = frame
                    break
            the_frame.findChild(QPushButton,"analysisButton").setText("Running")
            the_frame.findChild(QPushButton,"analysisButton").setStyleSheet("background-color: #6DD963")
            the_frame.findChild(QPushButton,"analysisStopButton").show()
            analysis.status = "running"

            log(1,"Launching analysis '%s'"%analysis.name)
            self.save()
            log(3,"Cleaning '%s_progress.txt' file"%analysis.name)
            if os.path.exists("%s/%s_progress.txt"%(self.dir,analysis.name)):
                os.remove("%s/%s_progress.txt"%(self.dir,analysis.name))
            self.thAnalysis = AnalysisThread(self,analysis)
            self.thAnalysis.connect(self.thAnalysis,SIGNAL("analysisProgress(int)"),self.analysisProgress)
            self.thAnalysis.connect(self.thAnalysis,SIGNAL("analysisProblem(QString)"),self.analysisProblem)
            self.thAnalysis.connect(self.thAnalysis,SIGNAL("analysisLog(int,QString)"),self.analysisLog)
            self.thAnalysis.start()
            # on la vire de la queue
            self.analysisQueue.remove(analysis)
            # on met à jour les queue numbers
            for frame in self.dicoFrameAnalysis.keys():
                anatmp = self.dicoFrameAnalysis[frame]
                if anatmp in self.analysisQueue:
                    index = self.analysisQueue.index(anatmp)
                    frame.findChild(QPushButton,"analysisButton").setText("Queued (%s)"%index)
                    frame.findChild(QPushButton,"analysisButton").setStyleSheet("background-color: #F4992B")

    def analysisProblem(self,msg):
        output.notify(self,"analysis problem","Something happened during the analysis %s : %s"%(self.thAnalysis.analysis.name,msg))

        # nettoyage du progress.txt
        aid = self.thAnalysis.analysis.name
        self.thAnalysis.analysis.status = "new"
        if os.path.exists("%s/%s_progress.txt"%(self.dir,aid)):
            os.remove("%s/%s_progress.txt"%(self.dir,aid))

        frame = None
        for fr in self.dicoFrameAnalysis.keys():
            if self.dicoFrameAnalysis[fr] == self.thAnalysis.analysis:
                frame = fr
                break
        frame.findChild(QPushButton,"analysisButton").setText("Re-launch")
        frame.findChild(QPushButton,"analysisButton").setStyleSheet("background-color: #EFB1B3")
        frame.findChild(QPushButton,"analysisStopButton").hide()
        self.thAnalysis.killProcess()
        self.thAnalysis = None

        self.nextAnalysisInQueue()

    def analysisLog(self,lvl,msg):
        """ log pour le thread analysis
        """
        if self.thAnalysis != None:
            log(lvl,msg)

    def analysisProgress(self,prog):
        """ met à jour l'indicateur de progression de l'analyse en cours
        """
        #prog = self.thAnalysis.progress
        frame = None
        for fr in self.dicoFrameAnalysis.keys():
            if self.dicoFrameAnalysis[fr] == self.thAnalysis.analysis:
                frame = fr
                break
        #frame.findChild(QLabel,"analysisStatusLabel").setText("%s%%"%prog)
        if prog < 1 and prog > 0:
            prog = 1
        frame.findChild(QProgressBar,"analysisStatusBar").setValue(int(prog))

        if prog >= 100.0:
            log(3,"Terminating analysis because progression indicated 100 %% (%s)"%prog)
            frame.findChild(QPushButton,"analysisButton").setText("View results")
            frame.findChild(QPushButton,"analysisButton").setStyleSheet("background-color: #79D8FF")
            frame.findChild(QPushButton,"analysisStopButton").hide()
            self.terminateAnalysis()

    def terminateAnalysis(self):
        """ arrête le thread de l'analyse et range les résultats
        dans un dossier
        """
        log(2,"Analysis '%s' is terminated. Starting results move"%self.thAnalysis.analysis.name)
        self.thAnalysis.terminate()
        the_analysis = self.thAnalysis.analysis
        aid = self.thAnalysis.analysis.name
        self.thAnalysis.analysis.status = "finished"
        atype = self.thAnalysis.analysis.category

        # nettoyage du progress.txt
        if os.path.exists("%s/%s_progress.txt"%(self.dir,aid)):
            os.remove("%s/%s_progress.txt"%(self.dir,aid))

        if atype == "estimate":
            if os.path.exists("%s/%s_phistar.txt"%(self.dir,aid))\
                    and os.path.exists("%s/%s_paramstatdens.txt"%(self.dir,aid)):
                # deplacement des fichiers de résultat
                aDirName = "estimation_%s"%aid
                if not os.path.exists("%s/analysis/%s"%(self.dir,aDirName)):
                    os.mkdir("%s/analysis/%s"%(self.dir,aDirName))
                shutil.move("%s/%s_phistar.txt"%(self.dir,aid),"%s/analysis/%s/phistar.txt"%(self.dir,aDirName))
                shutil.move("%s/%s_paramstatdens.txt"%(self.dir,aid),"%s/analysis/%s/paramstatdens.txt"%(self.dir,aDirName))
                shutil.move("%s/%s_mmmq.txt"%(self.dir,aid),"%s/analysis/%s/mmmq.txt"%(self.dir,aDirName))
                #shutil.move("%s/%s_psd.txt"%(self.dir,aid),"%s/analysis/%s/psd.txt"%(self.dir,aDirName))
                #os.remove("%s/%s_progress.txt"%(self.dir,aid))
            else:
                self.thAnalysis.problem = "No output files produced\n"
                if os.path.exists("%s/estimate.out"%(self.dir)):
                    f = open("%s/estimate.out"%(self.dir),"r")
                    lastLine = f.readlines()[-1]
                    f.close()
                    self.thAnalysis.problem += "\n%s"%lastLine
                self.thAnalysis.emit(SIGNAL("analysisProblem(QString)"),self.thAnalysis.problem)
                return
        elif atype == "compare":
            if os.path.exists("%s/%s_compdirect.txt"%(self.dir,aid)) and\
                    os.path.exists("%s/%s_complogreg.txt"%(self.dir,aid)):
                # deplacement des fichiers de résultat
                aDirName = "comparison_%s"%aid
                if not os.path.exists("%s/analysis/%s"%(self.dir,aDirName)):
                    os.mkdir("%s/analysis/%s"%(self.dir,aDirName))
                shutil.move("%s/%s_compdirect.txt"%(self.dir,aid),"%s/analysis/%s/compdirect.txt"%(self.dir,aDirName))
                shutil.move("%s/%s_complogreg.txt"%(self.dir,aid),"%s/analysis/%s/complogreg.txt"%(self.dir,aDirName))
                #os.remove("%s/%s_progress.txt"%(self.dir,aid))
                if os.path.exists("%s/analysis/%s/compdirect.txt"%(self.dir,aDirName)) and os.path.exists("%s/analysis/%s/complogreg.txt"%(self.dir,aDirName)):
                    f = open("%s/analysis/%s/complogreg.txt"%(self.dir,aDirName),'r')
                    g = open("%s/analysis/%s/compdirect.txt"%(self.dir,aDirName),'r')
                    datareg = f.read()
                    datadir = g.readlines()
                    f.close()
                    g.close()
                    dd = datetime.now()
                    date = "%s/%s/%s"%(dd.day,dd.month,dd.year)
                    textToDisplay = "\
                    COMPARISON OF SCENARIOS\n\
                    (%s)\n\n\
        Project directory : %s\n\
        Candidate scenarios : %s\n\
        Number of simulated data sets : %s\n\n\
        Direct approach\n\n"%(date,self.dir,the_analysis.candidateScList,self.ui.nbSetsDoneEdit.text())
                    textDirect = datadir[0].replace("   n   ","closest")
                    #i=-1
                    #while datadir[i].strip() == "":
                    #    i = i - 1
                    #pat = re.compile(r'\s+')
                    #num = int(pat.sub(' ',datadir[i].strip()).split(' ')[0])
                    #interval = num/10
                    i = 10
                    while i < len(datadir):
                        textDirect += datadir[i]
                        i+=10

                    textToDisplay += textDirect
                    textToDisplay += "\n\n Logistic approach\n\n"
                    textToDisplay += datareg
                    dest = open("%s/analysis/%s/compdirlog.txt"%(self.dir,aDirName),'w')
                    dest.write(textToDisplay)
                    dest.close()
            else:
                self.thAnalysis.problem = "No output files produced\n"
                if os.path.exists("%s/compare.out"%(self.dir)):
                    f = open("%s/compare.out"%(self.dir),"r")
                    lastLine = f.readlines()[-1]
                    f.close()
                    self.thAnalysis.problem += "\n%s"%lastLine
                self.thAnalysis.emit(SIGNAL("analysisProblem(QString)"),self.thAnalysis.problem)
                return
        elif atype == "bias":
            if os.path.exists("%s/%s_bias.txt"%(self.dir,aid)):
                log(3,"File %s/%s_bias.txt exists"%(self.dir,aid))
                #print "les fichiers existent"
                # deplacement des fichiers de résultat
                aDirName = "bias_%s"%aid
                if not os.path.exists("%s/analysis/%s"%(self.dir,aDirName)):
                    os.mkdir("%s/analysis/%s"%(self.dir,aDirName))
                shutil.move("%s/%s_bias.txt"%(self.dir,aid),"%s/analysis/%s/bias.txt"%(self.dir,aDirName))
                log(3,"Copy of '%s/%s_bias.txt' to '%s/analysis/%s/bias.txt' done"%(self.dir,aid,self.dir,aDirName))
                #print "déplacement de %s/%s_bias.txt vers %s/analysis/%s/bias.txt"%(self.dir,aid,self.dir,aDirName)
                #os.remove("%s/%s_progress.txt"%(self.dir,aid))
            else:
                self.thAnalysis.problem = "No output files produced\n"
                if os.path.exists("%s/bias.out"%(self.dir)):
                    f = open("%s/bias.out"%(self.dir),"r")
                    lastLine = f.readlines()[-1]
                    f.close()
                    self.thAnalysis.problem += "\n%s"%lastLine
                self.thAnalysis.emit(SIGNAL("analysisProblem(QString)"),self.thAnalysis.problem)
                log(3,"File %s/%s_bias.txt doesn't exist. Results cannot be moved"%(self.dir,aid))
                return
        elif atype == "evaluate":
            if os.path.exists("%s/%s_confidence.txt"%(self.dir,aid)):
                # deplacement des fichiers de résultat
                aDirName = "evaluation_%s"%aid
                if not os.path.exists("%s/analysis/%s"%(self.dir,aDirName)):
                    os.mkdir("%s/analysis/%s"%(self.dir,aDirName))
                shutil.move("%s/%s_confidence.txt"%(self.dir,aid),"%s/analysis/%s/confidence.txt"%(self.dir,aDirName))
                #os.remove("%s/%s_progress.txt"%(self.dir,aid))
            else:
                self.thAnalysis.problem = "No output files produced\n"
                if os.path.exists("%s/evaluate.out"%(self.dir)):
                    f = open("%s/evaluate.out"%(self.dir),"r")
                    lastLine = f.readlines()[-1]
                    f.close()
                    self.thAnalysis.problem += "\n%s"%lastLine
                self.thAnalysis.emit(SIGNAL("analysisProblem(QString)"),self.thAnalysis.problem)
                return
        elif atype == "modelChecking":
            if os.path.exists("%s/%s_mcACP.txt"%(self.dir,aid)) or os.path.exists("%s/%s_mclocate.txt"%(self.dir,aid)):
                # deplacement des fichiers de résultat
                aDirName = "modelChecking_%s"%aid
                if not os.path.exists("%s/analysis/%s"%(self.dir,aDirName)):
                    os.mkdir("%s/analysis/%s"%(self.dir,aDirName))
                if os.path.exists("%s/%s_mclocate.txt"%(self.dir,aid)):
                    shutil.move("%s/%s_mclocate.txt"%(self.dir,aid),"%s/analysis/%s/mclocate.txt"%(self.dir,aDirName))
                if os.path.exists("%s/%s_mcACP.txt"%(self.dir,aid)):
                    shutil.move("%s/%s_mcACP.txt"%(self.dir,aid),"%s/analysis/%s/mcACP.txt"%(self.dir,aDirName))
                #os.remove("%s/%s_progress.txt"%(self.dir,aid))
            else:
                self.thAnalysis.problem = "No output files produced\n"
                if os.path.exists("%s/modelChecking.out"%(self.dir)):
                    f = open("%s/modelChecking.out"%(self.dir),"r")
                    lastLine = f.readlines()[-1]
                    f.close()
                    self.thAnalysis.problem += "\n%s"%lastLine
                self.thAnalysis.emit(SIGNAL("analysisProblem(QString)"),self.thAnalysis.problem)
                return
        elif atype == "pre-ev":
            if os.path.exists("%s/%s_locate.txt"%(self.dir,aid)) or os.path.exists("%s/%s_ACP.txt"%(self.dir,aid)):
                # deplacement des fichiers de résultat
                aDirName = "pca_%s"%aid
                if not os.path.exists("%s/analysis/%s"%(self.dir,aDirName)):
                    os.mkdir("%s/analysis/%s"%(self.dir,aDirName))
                if os.path.exists("%s/%s_locate.txt"%(self.dir,aid)):
                    shutil.move("%s/%s_locate.txt"%(self.dir,aid),"%s/analysis/%s/locate.txt"%(self.dir,aDirName))
                if os.path.exists("%s/%s_ACP.txt"%(self.dir,aid)):
                    shutil.move("%s/%s_ACP.txt"%(self.dir,aid),"%s/analysis/%s/ACP.txt"%(self.dir,aDirName))
            else:
                self.thAnalysis.problem = "No output files produced\n"
                if os.path.exists("%s/pre-ev.out"%(self.dir)):
                    f = open("%s/pre-ev.out"%(self.dir),"r")
                    lastLine = f.readlines()[-1]
                    f.close()
                    self.thAnalysis.problem += "\n%s"%lastLine
                self.thAnalysis.emit(SIGNAL("analysisProblem(QString)"),self.thAnalysis.problem)
                return

        self.thAnalysis = None
        self.save()
        # on lance la suivante
        self.nextAnalysisInQueue()

    def nextAnalysisInQueue(self):
        """ Si il reste des analyses à faire, on lance la premiere
        """
        log(3,"Launching next analysis in queue")
        if len(self.analysisQueue) > 0:
            log(3,"Launching analysis %s because it is the first in the queue"%self.analysisQueue[0].name)
            self.launchAnalysis(self.analysisQueue[0])
        else:
            log(3,"No analysis in queue. Waiting orders")

    def moveAnalysisDown(self):
        """ déplace d'un cran vers le bas une analyse
        """
        frame = self.sender().parent()
        cur_index = self.ui.verticalLayout_9.indexOf(frame)
        nb_items = self.ui.verticalLayout_9.count()
        if cur_index < (nb_items-1):
            self.ui.verticalLayout_9.removeWidget(frame)
            self.ui.verticalLayout_9.insertWidget(cur_index+1,frame)

    def moveAnalysisUp(self):
        """ déplace d'un cran vers le haut une analyse
        """
        frame = self.sender().parent()
        cur_index = self.ui.verticalLayout_9.indexOf(frame)
        if cur_index > 0:
            self.ui.verticalLayout_9.removeWidget(frame)
            self.ui.verticalLayout_9.insertWidget(cur_index-1,frame)

    def addRow(self,atype,params,prio,status):
        """ ajoute une ligne dans le tableau d'analyses
        """
        self.ui.tableWidget.insertRow(self.ui.tableWidget.rowCount())
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,1,QTableWidgetItem("%s"%atype))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,2,QTableWidgetItem("%s"%params))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,3,QTableWidgetItem("%s"%prio))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,4,QTableWidgetItem("%s"%status))

        self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,1).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,1).flags() & ~Qt.ItemIsEditable)
        self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,2).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,2).flags() & ~Qt.ItemIsEditable)
        self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,4).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,4).flags() & ~Qt.ItemIsEditable)

        self.ui.tableWidget.setCellWidget(self.ui.tableWidget.rowCount()-1,5,QPushButton("View"))
            #self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,i).setText("new")
        #self.ui.tableWidget.insertRow(2)

    def putRefTableSize(self):
        """ met à jour l'affichage de la taille de la reftable
        et verrouille les modèles hist et gen si une reftable existe
        """
        size = self.readRefTableSize()
        if (size != None) and (size != 0):
            self.ui.nbSetsDoneEdit.setText("%s"%size)
            self.freezeHistModel()
            self.freezeGenData()
            self.ui.newAnButton.setDisabled(False)
        else:
            # si on n'a pas de reftable, on empêche la définition d'analyse
            self.ui.newAnButton.setDisabled(True)

    def freezeHistModel(self,yesno=True):
        """ empêche la modification du modèle historique tout en laissant
        la possibilité de le consulter
        """
        un=""
        if yesno: un="un"
        log(2,"%sfreezing Historical Model"%un)
        self.hist_model_win.ui.clearButton.setDisabled(yesno)
        self.hist_model_win.ui.exitButton.setDisabled(yesno)
        for e in self.hist_model_win.findChildren(QLineEdit):
            e.setReadOnly(yesno)
        for e in self.hist_model_win.findChildren(QRadioButton):
            e.setDisabled(yesno)
        for e in self.hist_model_win.findChildren(QPushButton,"remove"):
            e.setDisabled(yesno)
        self.hist_model_win.findChild(QPushButton,"addScButton").setDisabled(yesno) 
        self.hist_model_win.findChild(QPushButton,"defPrButton").setDisabled(yesno)
        for param in self.hist_model_win.paramList:
            for e in param.findChildren(QLineEdit):
                e.setReadOnly(yesno)
            for e in param.findChildren(QPushButton):
                e.setDisabled(yesno)
        for e in self.hist_model_win.findChildren(QPlainTextEdit):
            e.setReadOnly(yesno)

        if yesno:
            self.hist_model_win.findChild(QPushButton,"okButton").setText("OK")
            self.ui.setHistoricalButton.setText('            View')
        else:
            self.hist_model_win.findChild(QPushButton,"okButton").setText("VALIDATE AND SAVE")
            self.ui.setHistoricalButton.setText('            Set')

    def readRefTableSize(self):
        """ lit la table de référence binaire pour en extraire la taille et la retourner
        """
        reftablefile = "%s/reftable.bin"%self.dir
        try:
            return cbgpUtils.readRefTableSize(reftablefile)
        except Exception as e:
            return 0

    def readNbRecordsOfScenario(self,numSc):
        """ lit la table de référence binaire pour en extraire la taille et la retourner
        """
        reftablefile = "%s/reftable.bin"%self.dir
        return cbgpUtils.readNbRecordsOfScenario(reftablefile,numSc)

    def saveAnalysis(self):
        """ sauvegarde la liste des analyses dans le dossier du projet
        """
        log(2,"Saving analysis into %s"%self.parent.analysis_conf_name)
        l_to_save = []
        for a in self.analysisList:
            l_to_save.append(a)
        f=open("%s/%s"%(self.dir,self.parent.analysis_conf_name),"wb")
        pickle.dump(l_to_save,f)
        f.close()

    def loadAnalysis(self):
        """ charge les analyses sauvegardées
        """
        log(2,"Loading analysis from %s"%self.parent.analysis_conf_name)
        if os.path.exists("%s/%s"%(self.dir,self.parent.analysis_conf_name)):
            f=open("%s/%s"%(self.dir,self.parent.analysis_conf_name),"rb")
            l = pickle.load(f)
            f.close()
            for a in l:
                if a.status == "running" or a.status == "queued":
                    a.status = "new"
                self.addAnalysis(a)

    def lock(self):
        """ crée le fichier de verrouillage pour empêcher l'ouverture 
        du projet par une autre instance de DIYABC
        """
        log(2,"Locking the project '%s'"%self.dir)
        f = open("%s/.DIYABC_lock"%self.dir,"w")
        f.write("%s"%os.getpid())
        f.close()

    def unlock(self):
        """ supprime le verrouillage sur le projet ssi le verrouillage 
        a été effectué par notre instance de DIYABC
        """
        log(2,"Unlocking the project '%s'"%self.dir)
        if os.path.exists("%s/.DIYABC_lock"%self.dir):
            f = open("%s/.DIYABC_lock"%self.dir)
            pid = f.read()
            f.close()
            # on ne deverrouille que si c'est nous qui avons verrouillé
            if pid == str(os.getpid()):
                os.remove("%s/.DIYABC_lock"%self.dir)

class RefTableGenThreadCluster(QThread):
    """ thread de traitement qui met à jour la progressBar en fonction de l'avancée de
    la génération de la reftable sur le cluster
    """
    def __init__(self,parent,tarname,nb_to_gen):
        super(RefTableGenThreadCluster,self).__init__(parent)
        self.parent = parent
        self.tarname = tarname
        self.nb_done = 0
        self.nb_to_gen = nb_to_gen

    def log(self,lvl,msg):
        """ evite de manipuler les objets Qt dans un thread non principal
        """
        self.emit(SIGNAL("refTableLog(int,QString)"),lvl,msg)

    def killProcess(self):
        pass

    def run (self):
        filename = self.tarname
        host = str(self.parent.parent.preferences_win.ui.serverAddressEdit.text())
        port = int(str(self.parent.parent.preferences_win.ui.serverPortEdit.text()))


        f=open(filename, 'rb')
        s = socket(AF_INET, SOCK_STREAM)
        try:
            s.connect((host, port))
        except error,msg:
            self.log(3,"could not contact any server on diyabc://%s:%s\n%s"%(host, port,msg))
            return

        #size = os.path.getsize(filename)
        #s.send(str(size))

        data = f.read()
        f.close()
        s.send("%s"%str(hashlib.md5(data).hexdigest()))
        size = str(len(data))
        while len(size)<20:
            size+=" "
        s.send("%s"%size)
        time.sleep(1)

        s.sendall(data)
        #s.shutdown(SHUT_WR)


        #while True:
        #    if not data:
        #        break
        #    else:
        #        s.sendall(data)

        self.log(3,"%s Sent!"%filename)
        while True:
            line = s.recv(8000)
            self.log(3,"Line received : %s"%line)
            if len(line.split('somme '))>1:
                self.nb_done = int(line.split('somme ')[1].strip())
                self.emit(SIGNAL("increment(int,QString)"),self.nb_done,"unknown")
            if "durée" in line:
                self.nb_done = int(str(self.parent.ui.nbSetsReqEdit.text()))
                self.emit(SIGNAL("increment(int,QString)"),self.nb_done,"unknown")
            # TODO fix on ne va pas jusqu'à 100 parce qu'on lit la ligne du temps de merge au lieu de la derniere (modifs dans launch)
            if self.nb_done == int(str(self.parent.ui.nbSetsReqEdit.text())):
                break

        s.close()

class RefTableGenThread(QThread):
    """ thread de traitement qui met à jour la progressBar en fonction de l'avancée de
    la génération de la reftable
    """
    def __init__(self,parent,nb_to_gen):
        super(RefTableGenThread,self).__init__(parent)
        self.parent = parent
        self.nb_to_gen = nb_to_gen
        self.cancel = False
        self.time = "unknown time\n"
        self.processus = None

    def log(self,lvl,msg):
        """ evite de manipuler les objets Qt dans un thread non principal
        """
        self.emit(SIGNAL("refTableLog(int,QString)"),lvl,msg)

    def killProcess(self):
        self.log(3,"Attempting to kill reftable generation process")
        if self.processus != None:
            if self.processus.poll() == None:
                self.processus.kill()
                self.log(3,"Killing reftable generation process (pid:%s) DONE"%(self.processus.pid))
            else:
                self.log(3,"Processus %s has already terminated"%self.processus.pid)
        else:
            self.log(3,"No process to kill")

    def run (self):
        # lance l'executable
        #outfile = os.path.expanduser("~/.diyabc/general.out")
        outfile = "%s/general.out"%self.parent.dir
        if os.path.exists(outfile):
            os.remove(outfile)
        fg = open(outfile,"w")
        try:
            self.log(2,"Running the executable for the reference table generation")
            #print "/home/julien/vcs/git/diyabc/src-JMC-C++/gen -r %s -p %s"%(self.nb_to_gen,self.parent.dir)
            #exPath = str(self.parent.parent.preferences_win.ui.execPathEdit.text())
            exPath = self.parent.parent.preferences_win.getExecutablePath()
            particleLoopSize = str(self.parent.parent.preferences_win.particleLoopSizeEdit.text())
            nbMaxThread = self.parent.parent.preferences_win.getMaxThreadNumber()
            cmd_args_list = [exPath,"-p", "%s/"%self.parent.dir, "-r", "%s"%self.nb_to_gen , "-g", "%s"%particleLoopSize ,"-m", "-t", "%s"%nbMaxThread]
            #print " ".join(cmd_args_list)
            time.sleep(1)
            self.log(3,"Command launched : %s"%" ".join(cmd_args_list))
            addLine("%s/command.txt"%self.parent.dir,"Command launched : %s\n\n"%" ".join(cmd_args_list))
            p = subprocess.Popen(cmd_args_list, stdout=fg, stdin=subprocess.PIPE, stderr=subprocess.STDOUT) 
            self.processus = p
        except Exception as e:
            #print "Cannot find the executable of the computation program %s"%e
            self.problem = "Problem during program launch\n%s"%e
            self.emit(SIGNAL("refTableProblem(QString)"),self.problem)
            #output.notify(self.parent(),"computation problem","Cannot find the executable of the computation program")
            fg.close()
            return

        # boucle toutes les secondes pour verifier les valeurs dans le fichier
        self.nb_done = 0
        while self.nb_done < self.nb_to_gen:
            time.sleep(1)
            #self.nb_done += 1
            #print "plop"
            #self.emit(SIGNAL("increment"))
            # lecture 
            if os.path.exists("%s/reftable.log"%(self.parent.dir)):
                #print 'open'
                f = open("%s/reftable.log"%(self.parent.dir),"r")
                lines = f.readlines()
                f.close()
            else:
                lines = ["OK","0"]
            #print lines
            self.log(3,"Line red from reftable.log : %s"%lines)
            try:
                if len(lines) > 1:
                    if lines[0].strip() == "OK":
                        red = int(lines[1])
                        if len(lines)>2:
                            self.time = lines[2]
                        if red > self.nb_done:
                            self.nb_done = red
                            self.emit(SIGNAL("increment(int,QString)"),self.nb_done,self.time)
                    elif lines[0].strip() == "END":
                        red = int(lines[1])
                        self.nb_done = red
                        self.emit(SIGNAL("increment(int,QString)"),self.nb_done,self.time)
                        fg.close()
                        os.remove("%s/reftable.log"%(self.parent.dir))
                        return
                    else:
                        #print "lines != OK"
                        self.log(3,"The line does not contain 'OK'")
                        self.problem = lines[0].strip()
                        self.emit(SIGNAL("refTableProblem(QString)"),self.problem)
                        #output.notify(self,"problem",lines[0])
                        fg.close()
                        return
            except Exception as e:
                # certainement un problème de convertion en int
                self.log(3,"There was an exception during progress file reading : %s"%e)

            # verification de l'arret du programme
            if p.poll() != None:
                fg.close()
                g = open(outfile,"r")
                # TODO lire la fin de la sortie de l'ex
                #data= g.readlines()
                #print "data:%s"%data
                #print "poll:%s"%p.poll()
                g.close()
                if self.nb_done < self.nb_to_gen:
                    self.problem = "Reftable generation program exited anormaly"
                    self.emit(SIGNAL("refTableProblem(QString)"),self.problem)
                    return
            # TODO à revoir ac JM
            #else:
            #    self.problem = "unknown problem"
            #    self.emit(SIGNAL("refTableProblem"))
            #    print "unknown problem"
            #    #output.notify(self,"problem","Unknown problem")
            #    fg.close()
            #    return

        fg.close()

        #for i in range(1000):
        #    if self.cancel: break
        #    time.sleep(0.01)
        #    self.emit(SIGNAL("increment"))
        #    #print "%d "%(i),

    @pyqtSignature("")
    def cancel(self):
        """SLOT to cancel treatment"""
        self.cancel = True

class AnalysisThread(QThread):
    """ classe qui gère l'execution du programme qui effectue une analyse
    """
    def __init__(self,parent,analysis):
        super(AnalysisThread,self).__init__(parent)
        self.parent = parent
        self.analysis = analysis
        self.progress = 0
        self.processus = None

    def log(self,lvl,msg):
        """ evite de manipuler les objets Qt dans un thread non principal
        """
        clean_msg = msg.replace(u'\xb5',u'u')
        self.emit(SIGNAL("analysisLog(int,QString)"),lvl,clean_msg)

    def killProcess(self):
        if self.processus != None:
            if self.processus.poll() == None:
                self.log(3,"Killing analysis process (pid:%s) of analysis %s"%(self.processus.pid,self.analysis.name))
                self.processus.kill()

    def readProgress(self):
        b = ""
        if os.path.exists("%s/%s_progress.txt"%(self.parent.dir,self.analysis.name)):
            a = open("%s/%s_progress.txt"%(self.parent.dir,self.analysis.name),"r")
            lines = a.readlines()
            a.close()
            if len(lines) > 0:
                b = lines[0]
        #print "prog:%s"%b
        self.log(3,"Analysis '%s' progress : %s"%(self.analysis.name,b))
        return b

    def readProblem(self):
        problem = ""
        if os.path.exists("%s/%s_progress.txt"%(self.parent.dir,self.analysis.name)):
            a = open("%s/%s_progress.txt"%(self.parent.dir,self.analysis.name),"r")
            lines = a.readlines()
            a.close()
            if len(lines) > 0:
                problem = lines[0]
        return problem

    def updateProgress(self):
        b = self.readProgress()
        # on gere le cas ou ce ne sont pas des float avec une exception qui passe
        try:
            # on a bougé
            if len(b.split(' ')) > 1:
                t1 = float(b.split(' ')[0])
                t2 = float(b.split(' ')[1])
                self.tmpp = (t1*100/t2)
            if self.tmpp != self.progress:
                #print "on a progressé"
                self.log(3,"The analysis '%s' has progressed (%s%%)"%(self.analysis.name,self.tmpp))
                self.progress = self.tmpp
                self.emit(SIGNAL("analysisProgress(int)"),self.progress)
        except Exception as e:
            return

    def run(self):
        self.log(2,"Running analysis '%s' execution"%self.analysis.name)
        #executablePath = str(self.parent.parent.preferences_win.ui.execPathEdit.text())
        executablePath = self.parent.parent.preferences_win.getExecutablePath()
        nbMaxThread = self.parent.parent.preferences_win.getMaxThreadNumber()
        particleLoopSize = str(self.parent.parent.preferences_win.particleLoopSizeEdit.text())
        params = self.analysis.computationParameters
        if self.analysis.category == "estimate":
            option = "-e"
        elif self.analysis.category == "compare":
            option = "-c"
        elif self.analysis.category == "bias":
            option = "-b"
        elif self.analysis.category == "evaluate":
            option = "-f"
        elif self.analysis.category == "modelChecking":
            option = "-j"
        elif self.analysis.category == "pre-ev":
            # pour cette analyse on attend que l'executable ait fini
            # on ne scrute pas de fichier de progression
            #data = data.replace(u'\xb5','u')
            cmd_args_list = [executablePath,"-p", "%s/"%self.parent.dir, "-d", '%s'%params.replace(u'\xb5','u'), "-i", '%s'%self.analysis.name, "-m", "-t", "%s"%nbMaxThread]
            #print " ".join(cmd_args_list)
            self.log(3,"Command launched for analysis '%s' : %s"%(self.analysis.name," ".join(cmd_args_list)))
            addLine("%s/command.txt"%self.parent.dir,"Command launched for analysis '%s' : %s\n\n"%(self.analysis.name," ".join(cmd_args_list)))
            outfile = "%s/pre-ev.out"%self.parent.dir
            f = open(outfile,"w")
            p = subprocess.call(cmd_args_list, stdout=f, stdin=subprocess.PIPE, stderr=subprocess.STDOUT) 
            #f.close()
            #print "call ok"
            self.log(3,"Call procedure success")

            f = open(outfile,"r")
            #data= f.read()
            f.close()
            self.progress = 100
            self.emit(SIGNAL("analysisProgress(int)"),self.progress)
            return

        # pour toutes les autres analyses le schema est le même
        cmd_args_list = [executablePath,"-p", "%s/"%self.parent.dir, "%s"%option, '%s'%params, "-i", '%s'%self.analysis.name,"-g" ,"%s"%particleLoopSize , "-m", "-t", "%s"%nbMaxThread]
        #print " ".join(cmd_args_list)
        self.log(3,"Command launched for analysis '%s' : %s"%(self.analysis.name," ".join(cmd_args_list)))
        addLine("%s/command.txt"%self.parent.dir,"Command launched for analysis '%s' : %s\n\n"%(self.analysis.name," ".join(cmd_args_list)))
        outfile = "%s/%s.out"%(self.parent.dir,self.analysis.category)
        f = open(outfile,"w")
        p = subprocess.Popen(cmd_args_list, stdout=f, stdin=subprocess.PIPE, stderr=subprocess.STDOUT) 
        self.processus = p
        #f.close()
        #print "popen ok"
        self.log(3,"Popen procedure success")


        # la scrutation de la progression est identique pour toutes les analyses
        self.progress = 1
        self.tmpp = 1
        self.emit(SIGNAL("analysisProgress(int)"),self.progress)
        while True:
            self.updateProgress()
            # verification de l'arret du programme
            if p.poll() != None:
                f.close()
                g = open(outfile,"r")
                #data= g.read()
                #print "data:%s"%data
                #print "poll:%s"%p.poll()
                outlines = g.readlines()
                probOut = ""
                if len(outlines) > 0:
                    probOut = outlines[-1]
                g.close()
                # on attend un peu pour etre sur que l'ecriture de la progression a été effectué
                time.sleep(2)
                self.updateProgress()
                if self.progress < 100:
                    redProblem = self.readProblem()
                    self.problem = "Analysis program exited before the end of the analysis (%s%%)\n%s\n%s"%(self.progress,redProblem,probOut)
                    self.emit(SIGNAL("analysisProblem(QString)"),self.problem)
                    return
            time.sleep(2)

