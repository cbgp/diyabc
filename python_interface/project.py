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
from subprocess import Popen, PIPE, STDOUT 
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui,QtCore
from PyQt4 import uic
#from uis.project_ui import *
from setHistoricalModel import SetHistoricalModel
from setGenDataRefTable import SetGeneticDataRefTable
from mutationModel.setMutationModelMsat import SetMutationModelMsat
from mutationModel.setMutationModelSequences import SetMutationModelSequences
from summaryStatistics.setSummaryStatisticsMsat import SetSummaryStatisticsMsat
from summaryStatistics.setSummaryStatisticsSeq import SetSummaryStatisticsSeq
from analysis.defineNewAnalysis import DefineNewAnalysis
from analysis.drawEstimationAnalysisResult import DrawEstimationAnalysisResult
from analysis.drawComparisonAnalysisResult import DrawComparisonAnalysisResult
from analysis.drawPCAAnalysisResult import DrawPCAAnalysisResult
from analysis.viewAnalysisParameters import ViewAnalysisParameters
#from uis.viewTextFile_ui import Ui_Frame as ui_viewTextFile
from utils.data import Data
from datetime import datetime 
import os.path
from PyQt4.Qwt5 import *
from PyQt4.Qwt5.qplt import *
import output

formProject,baseProject = uic.loadUiType("uis/Project.ui")

class Project(baseProject,formProject):
    """ classe qui représente un projet
    par defaut, un projet est considéré comme nouveau, cad que l'affichage est celui d'un projet vierge
    pour un projet chargé, on modifie l'affichage en conséquence dans loadFromDir
    """
    def __init__(self,name,dir=None,parent=None):
        self.parent=parent
        self.name=name
        self.dir=dir
        self.dataFileSource = ""
        self.dataFileName = ""
        self.hist_state_valid = False
        self.gen_state_valid = False
        self.data = None
        self.analysisList = []
        self.dicoFrameAnalysis = {}

        self.thAnalysis = None

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
        self.ui.groupBox.hide()
        self.ui.browseDirButton.hide()
        #self.ui.groupBox.setVisible(False)

        # creation des onglets "set ..."
        self.hist_model_win = SetHistoricalModel(self)
        self.hist_model_win.ui.okButton.setText("VALIDATE AND SAVE")
        self.hist_model_win.hide()
        #self.addTab(self.hist_model_win,"Set historical model")
        #self.setTabEnabled(2,False)

        self.gen_data_win = SetGeneticDataRefTable(self)
        self.gen_data_win.ui.okButton.setText("VALIDATE AND SAVE")
        self.gen_data_win.hide()
        #self.addTab(self.gen_data_win,"Set genetic data")
        #self.setTabEnabled(3,False)


        # manual alignment set
        #self.ui.verticalLayout.setAlignment(self.ui.horizontalFrame,Qt.AlignTop)
        #self.ui.verticalLayout.setAlignment(self.ui.groupBox,Qt.AlignTop)
        self.ui.verticalLayout_2.setAlignment(self.ui.newAnButton,Qt.AlignCenter)
        self.ui.verticalLayout_3.setAlignment(self.ui.progressBar,Qt.AlignCenter)
        self.ui.projNameLabelValue.setText(self.name)
        #self.ui.tableWidget.setColumnWidth(1,150)
        #self.ui.tableWidget.setColumnWidth(2,300)
        #self.ui.tableWidget.setColumnWidth(3,70)

        QObject.connect(self.ui.newAnButton,SIGNAL("clicked()"),self.defineNewAnalysis)
        #QObject.connect(self.ui.tableWidget,SIGNAL("cellClicked(int,int)"),self.clcl)
        QObject.connect(self.ui.setHistoricalButton,SIGNAL("clicked()"),self.setHistorical)
        QObject.connect(self.ui.setGeneticButton,SIGNAL("clicked()"),self.setGenetic)
        #QObject.connect(self.ui.browseDataFileButton,SIGNAL("clicked()"),self.dataFileSelection)
        QObject.connect(self.ui.browseDataFileButton,SIGNAL("clicked()"),self.dataFileSelectionAndCopy)
        #QObject.connect(self.ui.browseDirButton,SIGNAL("clicked()"),self.dirSelection)
        QObject.connect(self.ui.browseDirButton,SIGNAL("clicked()"),self.dirCreation)

        # inserer image
        self.ui.setHistoricalButton.setIcon(QIcon("docs/redcross.png"))
        self.ui.setGeneticButton.setIcon(QIcon("docs/redcross.png"))

        self.setTabIcon(0,QIcon("docs/redcross.png"))
        self.setTabIcon(1,QIcon("/usr/share/pixmaps/baobab.xpm"))


        #for i in range(self.ui.tableWidget.columnCount()):
        #    for j in range(self.ui.tableWidget.rowCount()):
        #        it = QTableWidgetItem("%i,%i"%(j,i))
        #        self.ui.tableWidget.setItem(j,i,it)

        self.connect(self.ui.runReftableButton, SIGNAL("clicked()"),self,SLOT("on_btnStart_clicked()"))
        self.connect(self.ui.stopReftableButton, SIGNAL("clicked()"),self.stopRefTableGen)
        #self.connect(self.ui.cancelButton, SIGNAL("clicked()"),self.cancelTh)

        self.th = None

        self.ui.verticalLayout_9.setAlignment(Qt.AlignTop)

        ## remplissage du combo liste des analyses deja effectuées
        #self.fillAnalysisCombo()
        #QObject.connect(self.ui.analysisResultsButton,SIGNAL("clicked()"),self.viewAnalysisResult)

    def stopRefTableGen(self):
        """ tue le thread de génération et crée le fichier .stop pour 
        arrêter la génération de la reftable
        """
        f = open("%s/.stop"%self.dir,"w")
        f.write(" ")
        f.close()
        if self.th != None:
            self.th.terminate()
            self.th = None
        self.ui.reftableProgressLabel.setText("")
        if os.path.exists("%s/reftable.log"%(self.dir)):
            os.remove("%s/reftable.log"%(self.dir))

    def viewAnalysisResult(self,analysis=None):
        """ en fonction du type d'analyse, met en forme les résultats
        """
        anCat = analysis.category
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
                print "yeepa"
                #ui = ui_viewTextFile()
                #ui.setupUi(self.drawAnalysisFrame)
                ui.dataPlain.setPlainText(data)
                ui.dataPlain.setLineWrapMode(0)
                font = "FreeMono"
                if sys.platform.startswith('win'):
                    font = "Courier New"
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
            print "yeepi"
            #ui = ui_viewTextFile()
            #ui.setupUi(self.drawAnalysisFrame)
            ui.dataPlain.setPlainText(data)
            ui.dataPlain.setLineWrapMode(0)
            font = "FreeMono"
            if sys.platform.startswith('win'):
                font = "Courier New"
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
            print "yeepo"
            #ui = ui_viewTextFile()
            #ui.setupUi(self.drawAnalysisFrame)
            ui.dataPlain.setPlainText(data)
            ui.dataPlain.setLineWrapMode(0)
            font = "FreeMono"
            if sys.platform.startswith('win'):
                font = "Courier New"
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
        def_analysis = DefineNewAnalysis(self)
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
for i in $(seq 1 %s); do \n\
qsub -q short_queue.q -cwd node.sh 10000 `pwd` $i\n\
done;\n'%nbFullQsub
        
        if nbLastQsub != 0:
            res+='let last=$i+1\n\
qsub -q short_queue.q -cwd node.sh %s `pwd` $last\n'%nbLastQsub
        
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
$TMPDIR/general -s "$3" -p $TMPDIR/ -r $1 & \n\
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
        self.save()
        self.writeRefTableHeader()
        if self.th == None:
            if not os.path.exists("%s/reftable.bin"%self.dir) and os.path.exists("%s/reftable.log"%self.dir):
                os.remove("%s/reftable.log"%self.dir)
            try:
                nb_to_gen = int(self.ui.nbSetsReqEdit.text())
            except Exception,e:
                output.notify(self,"value error","Check the value of required number of data sets")
                return
            # on demarre le thread local ou cluster
            if self.parent.preferences_win.ui.serverCheck.isChecked():
                tname = self.generateComputationTar("/tmp/aaaa.tar")
                self.th = RefTableGenThreadCluster(self,tname,nb_to_gen)
            else:
                self.th = RefTableGenThread(self,nb_to_gen)
            self.th.connect(self.th,SIGNAL("increment"),self.incProgress)
            self.th.connect(self.th,SIGNAL("refTableProblem"),self.refTableProblem)
            #self.ui.progressBar.connect (self, SIGNAL("canceled()"),self.th,SLOT("cancel()"))
            self.th.start()
        else:
            # si l'ancien thread a fini, on en relance un
            if not self.th.isRunning():
                self.th = None
                self.on_btnStart_clicked()
            #self.cancelTh()

    def refTableProblem(self):
        output.notify(self,"reftable problem","Something happened during the reftable generation : %s"%(self.th.problem))
 
    def incProgress(self):
        """Incremente la barre de progression de la générationd e la reftable
        """
        done = self.th.nb_done
        nb_to_do = self.th.nb_to_gen
        time_remaining = self.th.time
        self.ui.reftableProgressLabel.setText('%s remaining'%time_remaining)
        pc = (float(done)/float(nb_to_do))*100
        self.ui.progressBar.setValue(int(pc))
        self.ui.nbSetsDoneEdit.setText("%s"%done)
        # si on a fini, on met à jour l'affichage de la taille de la reftable
        # et on verrouille eventuellement histmodel et gendata
        if int(pc) == 100:
            self.putRefTableSize()

    def cancelTh(self):
        #print 'plop'
        self.emit(SIGNAL("canceled()"))

    def writeRefTableHeader(self):
        """ écriture du header.txt à partir des conf
        """
        if os.path.exists(self.dir+"/%s"%self.parent.main_conf_name) and os.path.exists(self.dir+"/%s"%self.parent.hist_conf_name) and os.path.exists(self.dir+"/%s"%self.parent.gen_conf_name) and os.path.exists(self.dir+"/%s"%self.parent.table_header_conf_name):
            if os.path.exists(self.dir+"/%s"%self.parent.reftableheader_name):
                os.remove("%s/%s" %(self.dir,self.parent.reftableheader_name))
            f1 = codecs.open(self.dir+"/%s"%self.parent.main_conf_name,"r","utf-8")
            f1lines = f1.read()
            f1.close()
            f2 = codecs.open(self.dir+"/%s"%self.parent.hist_conf_name,"r","utf-8")
            f2lines = f2.read()
            f2.close()
            f3 = codecs.open(self.dir+"/%s"%self.parent.gen_conf_name,"r","utf-8")
            f3lines = f3.read()
            f3.close()
            f4 = codecs.open(self.dir+"/%s"%self.parent.table_header_conf_name,"r","utf-8")
            f4lines = f4.read()
            f4.close()

            f = codecs.open(self.dir+"/%s"%self.parent.reftableheader_name,"w","utf-8")
            f.write(f1lines)
            f.write(f2lines)
            f.write(f3lines)
            f.write(f4lines)
            f.close()
        else:
            output.notify(self,"Header generation problem","One conf file is missing in order to generate the reference table header")

    def dataFileSelectionAndCopy(self,name=None):
        """ dialog pour selectionner le fichier à lire
        il est lu et vérifié. S'il est invalide, on garde la sélection précédente
        S'il est valide, on le copie dans le dossier du projet
        """
        if name == None:
            qfd = QFileDialog()
            qfd.setDirectory(self.ui.dirEdit.text())
            name = qfd.getOpenFileName()
        if self.loadDataFile(name):
            # si on a reussi a charger le data file, on vire le bouton browse
            self.ui.browseDataFileButton.hide()
            # et on copie ce datafile dans le dossier projet
            shutil.copy(self.dataFileSource,"%s/%s"%(self.dir,self.dataFileSource.split('/')[-1]))
            self.dataFileName = self.dataFileSource.split('/')[-1]
            self.ui.groupBox.show()
            # comme on a lu le datafile, on peut remplir le tableau de locus dans setGeneticData
            self.gen_data_win.fillLocusTableFromData()

    def loadDataFile(self,name):
        """ Charge le fichier de données passé en paramètre. Cette fonction est appelée lors
        de l'ouverture d'un projet existant et lors du choix du fichier de données pour un nouveau projet
        """
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

    def dirCreation(self,path):
        """ selection du repertoire pour un nouveau projet et copie du fichier de données
        """
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
        type_analysis = analysis.category
        self.analysisList.append(analysis)

        print analysis.computationParameters
        if type_analysis == "pre-ev":
            #self.addRow("scenario prior combination",analysis[1],"4","new")
            self.addAnalysisGui(analysis,analysis.name,"scenario prior combination",analysis.computationParameters,analysis.status)
        elif type_analysis == "estimate":
            print "\n",analysis.computationParameters,"\n"
            #self.addRow("parameter estimation","params","5","new")
            self.addAnalysisGui(analysis,analysis.name,"parameter estimation","params",analysis.status)
        elif type_analysis == "bias":
            #self.addRow("bias and precision",str(analysis[2]),"3","new")
            self.addAnalysisGui(analysis,analysis.name,"bias and precision",str(analysis.chosenSc),analysis.status)
        elif type_analysis == "compare":
            #print "\n",analysis[-1],"\n"
            #self.addRow("scenario choice",analysis[2]["de"],"4","new")
            self.addAnalysisGui(analysis,analysis.name,"scenario choice","%s | %s"%(analysis.candidateScList,analysis.chosenSc),analysis.status)
        elif type_analysis == "evaluate":
            #self.addRow("evaluate confidence","%s | %s"%(analysis[2],analysis[3]),"3","new")
            self.addAnalysisGui(analysis,analysis.name,"evaluate confidence","%s | %s"%(analysis.candidateScList,analysis.chosenSc),analysis.status)
        elif type_analysis == "modelChecking":
            self.addAnalysisGui(analysis,analysis.name,"model checking","%s | %s"%(analysis.candidateScList,analysis.chosenSc),analysis.status)

        self.save()

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
        analysisRmButton = QtGui.QPushButton("remove",frame_9)
        analysisRmButton.setMinimumSize(QtCore.QSize(50, 0))
        analysisRmButton.setMaximumSize(QtCore.QSize(50, 16777215))
        analysisRmButton.setObjectName("analysisRmButton")
        horizontalLayout_4.addWidget(analysisRmButton)
        analysisUpButton = QtGui.QPushButton("up",frame_9)
        analysisUpButton.setMinimumSize(QtCore.QSize(50, 0))
        analysisUpButton.setMaximumSize(QtCore.QSize(50, 16777215))
        analysisUpButton.setObjectName("analysisUpButton")
        horizontalLayout_4.addWidget(analysisUpButton)
        analysisDownButton = QtGui.QPushButton("down",frame_9)
        analysisDownButton.setMinimumSize(QtCore.QSize(50, 0))
        analysisDownButton.setMaximumSize(QtCore.QSize(50, 16777215))
        analysisDownButton.setObjectName("analysisDownButton")
        horizontalLayout_4.addWidget(analysisDownButton)
        analysisNameLabel = QtGui.QLabel(name,frame_9)
        analysisNameLabel.setAlignment(Qt.AlignCenter)
        analysisNameLabel.setMinimumSize(QtCore.QSize(100, 0))
        analysisNameLabel.setMaximumSize(QtCore.QSize(100, 16777215))
        analysisNameLabel.setObjectName("analysisNameLabel")
        analysisNameLabel.setFrameShape(QtGui.QFrame.StyledPanel)
        analysisNameLabel.setMaximumSize(QtCore.QSize(70, 16777215))
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
            buttonLabel = "view"
            analysisStatusBar.setValue(100)
        else:
            buttonLabel = "launch"
            analysisStatusBar.setValue(0)
        analysisButton = QtGui.QPushButton(buttonLabel,frame_9)
        analysisButton.setObjectName("analysisButton")
        analysisButton.setMinimumSize(QtCore.QSize(70, 0))
        analysisButton.setMaximumSize(QtCore.QSize(70, 16777215))
        horizontalLayout_4.addWidget(analysisButton)
        horizontalLayout_4.setContentsMargins(-1, 1, -1, 1)
        self.ui.verticalLayout_9.addWidget(frame_9)

        self.dicoFrameAnalysis[frame_9] = analysis
        QObject.connect(analysisRmButton,SIGNAL("clicked()"),self.removeAnalysis)
        QObject.connect(analysisDownButton,SIGNAL("clicked()"),self.moveAnalysisDown)
        QObject.connect(analysisUpButton,SIGNAL("clicked()"),self.moveAnalysisUp)
        QObject.connect(analysisButton,SIGNAL("clicked()"),self.launchViewAnalysis)
        QObject.connect(analysisParamsButton,SIGNAL("clicked()"),self.viewAnalysisParameters)

    def removeAnalysis(self):
        frame = self.sender().parent()
        frame.hide()
        self.analysisList.remove(self.dicoFrameAnalysis[frame])
        del self.dicoFrameAnalysis[frame]
        self.ui.verticalLayout_9.removeWidget(frame)
        self.save()

    def viewAnalysisParameters(self):
        """ bascule sur une frame qui affiche les valeurs des paramètres
        d'une analyse
        """
        frame = self.sender().parent()
        # on associe l'analyse a sa frame
        analysis = self.dicoFrameAnalysis[frame]
        viewFrame = ViewAnalysisParameters(self,analysis)
        self.ui.analysisStack.addWidget(viewFrame)
        self.ui.analysisStack.setCurrentWidget(viewFrame)


    def launchViewAnalysis(self):
        """ clic sur le bouton launch/view d'une analyse
        """
        if not os.path.exists("%s/analysis/"%self.dir):
            os.mkdir("%s/analysis/"%self.dir)

        frame = self.sender().parent()
        # on associe l'analyse a sa frame
        analysis = self.dicoFrameAnalysis[frame]
        if analysis.status == "finished":
            self.viewAnalysisResult(analysis)
        else:
            if self.thAnalysis == None:
                self.launchAnalysis(analysis)
                frame.findChild(QPushButton,"analysisButton").setText("Running")
                analysis.status = "running"

    def launchAnalysis(self,analysis):            
        """ lance un thread de traitement d'une analyse
        """
        self.save()
        self.thAnalysis = AnalysisThread(self,analysis)
        self.thAnalysis.connect(self.thAnalysis,SIGNAL("analysisProgress"),self.analysisProgress)
        self.thAnalysis.connect(self.thAnalysis,SIGNAL("analysisProblem"),self.analysisProblem)
        self.thAnalysis.start()

    def analysisProblem(self):
        output.notify(self,"analysis problem","Something happened during the analysis %s : %s"%(self.thAnalysis.analysis.name,self.thAnalysis.problem))

        # nettoyage du progress.txt
        aid = self.thAnalysis.analysis.name
        if os.path.exists("%s/%s_progress.txt"%(self.dir,aid)):
            os.remove("%s/%s_progress.txt"%(self.dir,aid))

        frame = None
        for fr in self.dicoFrameAnalysis.keys():
            if self.dicoFrameAnalysis[fr] == self.thAnalysis.analysis:
                frame = fr
                break
        frame.findChild(QPushButton,"analysisButton").setText("re-launch")
        self.thAnalysis = None

    def analysisProgress(self):
        """ met à jour l'indicateur de progression de l'analyse en cours
        """
        prog = self.thAnalysis.progress
        frame = None
        for fr in self.dicoFrameAnalysis.keys():
            if self.dicoFrameAnalysis[fr] == self.thAnalysis.analysis:
                frame = fr
                break
        #frame.findChild(QLabel,"analysisStatusLabel").setText("%s%%"%prog)
        frame.findChild(QProgressBar,"analysisStatusBar").setValue(int(prog))

        if int(prog) >= 100:
            print "terminate analysis"
            frame.findChild(QPushButton,"analysisButton").setText("View")
            self.terminateAnalysis()

    def terminateAnalysis(self):
        """ arrête le thread de l'analyse et range les résultats
        dans un dossier
        """
        self.thAnalysis.terminate()
        aid = self.thAnalysis.analysis.name
        self.thAnalysis.analysis.status = "finished"
        atype = self.thAnalysis.analysis.category
        self.thAnalysis = None
        print "type = %s"%(atype) 

        # nettoyage du progress.txt
        if os.path.exists("%s/%s_progress.txt"%(self.dir,aid)):
            os.remove("%s/%s_progress.txt"%(self.dir,aid))

        if atype == "estimate":
            if os.path.exists("%s/%s_phistar.txt"%(self.dir,aid))\
                    and os.path.exists("%s/%s_paramstatdens.txt"%(self.dir,aid)):
                # deplacement des fichiers de résultat
                aDirName = "estimation_%s"%aid
                os.mkdir("%s/analysis/%s"%(self.dir,aDirName))
                shutil.move("%s/%s_phistar.txt"%(self.dir,aid),"%s/analysis/%s/phistar.txt"%(self.dir,aDirName))
                shutil.move("%s/%s_paramstatdens.txt"%(self.dir,aid),"%s/analysis/%s/paramstatdens.txt"%(self.dir,aDirName))
                #shutil.move("%s/%s_psd.txt"%(self.dir,aid),"%s/analysis/%s/psd.txt"%(self.dir,aDirName))
                #os.remove("%s/%s_progress.txt"%(self.dir,aid))

        elif atype == "compare":
            if os.path.exists("%s/%s_compdirect.txt"%(self.dir,aid)) and\
                    os.path.exists("%s/%s_complogreg.txt"%(self.dir,aid)):
                # deplacement des fichiers de résultat
                aDirName = "comparison_%s"%aid
                os.mkdir("%s/analysis/%s"%(self.dir,aDirName))
                shutil.move("%s/%s_compdirect.txt"%(self.dir,aid),"%s/analysis/%s/compdirect.txt"%(self.dir,aDirName))
                shutil.move("%s/%s_complogreg.txt"%(self.dir,aid),"%s/analysis/%s/complogreg.txt"%(self.dir,aDirName))
                #os.remove("%s/%s_progress.txt"%(self.dir,aid))
        elif atype == "bias":
            if os.path.exists("%s/%s_bias.txt"%(self.dir,aid)):
                #print "les fichiers existent"
                # deplacement des fichiers de résultat
                aDirName = "bias_%s"%aid
                os.mkdir("%s/analysis/%s"%(self.dir,aDirName))
                shutil.move("%s/%s_bias.txt"%(self.dir,aid),"%s/analysis/%s/bias.txt"%(self.dir,aDirName))
                #print "déplacement de %s/%s_bias.txt vers %s/analysis/%s/bias.txt"%(self.dir,aid,self.dir,aDirName)
                #os.remove("%s/%s_progress.txt"%(self.dir,aid))
        elif atype == "evaluate":
            if os.path.exists("%s/%s_confidence.txt"%(self.dir,aid)):
                # deplacement des fichiers de résultat
                aDirName = "evaluation_%s"%aid
                os.mkdir("%s/analysis/%s"%(self.dir,aDirName))
                shutil.move("%s/%s_confidence.txt"%(self.dir,aid),"%s/analysis/%s/confidence.txt"%(self.dir,aDirName))
                #os.remove("%s/%s_progress.txt"%(self.dir,aid))
        elif atype == "modelChecking":
            if os.path.exists("%s/%s_mcACP.txt"%(self.dir,aid)) or os.path.exists("%s/%s_mclocate.txt"%(self.dir,aid)):
                # deplacement des fichiers de résultat
                aDirName = "modelChecking_%s"%aid
                os.mkdir("%s/analysis/%s"%(self.dir,aDirName))
                if os.path.exists("%s/%s_mclocate.txt"%(self.dir,aid)):
                    shutil.move("%s/%s_mclocate.txt"%(self.dir,aid),"%s/analysis/%s/mclocate.txt"%(self.dir,aDirName))
                if os.path.exists("%s/%s_mcACP.txt"%(self.dir,aid)):
                    shutil.move("%s/%s_mcACP.txt"%(self.dir,aid),"%s/analysis/%s/mcACP.txt"%(self.dir,aDirName))
                #os.remove("%s/%s_progress.txt"%(self.dir,aid))
        elif atype == "pre-ev":
            if os.path.exists("%s/%s_locate.txt"%(self.dir,aid)) or os.path.exists("%s/%s_ACP.txt"%(self.dir,aid)):
                # deplacement des fichiers de résultat
                aDirName = "pca_%s"%aid
                os.mkdir("%s/analysis/%s"%(self.dir,aDirName))
                if os.path.exists("%s/%s_locate.txt"%(self.dir,aid)):
                    shutil.move("%s/%s_locate.txt"%(self.dir,aid),"%s/analysis/%s/locate.txt"%(self.dir,aDirName))
                if os.path.exists("%s/%s_ACP.txt"%(self.dir,aid)):
                    shutil.move("%s/%s_ACP.txt"%(self.dir,aid),"%s/analysis/%s/ACP.txt"%(self.dir,aDirName))

        self.save()

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

    def setHistorical(self):
        """ passe sur l'onglet correspondant
        """
        #self.addTab(self.hist_model_win,"Set historical model")

        #self.setTabEnabled(0,False)
        #self.setTabEnabled(1,False)
        ## nécéssaire seulement sous windows
        ## on dirait que sous linux, le simple setCurrentWidget rend l'onglet enabled
        #self.setTabEnabled(self.count()-1,True)
        #self.setCurrentWidget(self.hist_model_win)
        self.ui.refTableStack.addWidget(self.hist_model_win)
        self.ui.refTableStack.setCurrentWidget(self.hist_model_win)
        self.setHistValid(False)

    def setGenetic(self):
        """ passe sur l'onglet correspondant
        """
        #self.setTabEnabled(0,False)
        #self.setTabEnabled(1,False)
        #self.addTab(self.gen_data_win,"Set genetic data")
        #self.setCurrentWidget(self.gen_data_win)
        self.ui.refTableStack.addWidget(self.gen_data_win)
        self.ui.refTableStack.setCurrentWidget(self.gen_data_win)
        self.setGenValid(False)

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
        #self.removeTab(self.indexOf(self.hist_model_win))
        self.ui.refTableStack.removeWidget(self.hist_model_win)
        self.hist_model_win = SetHistoricalModel(self)
        #self.insertTab(2,self.hist_model_win,"Set historical model")
        self.ui.refTableStack.insertWidget(0,self.hist_model_win)
        #self.setCurrentWidget(self.hist_model_win)
        self.ui.refTableStack.setCurrentWidget(self.hist_model_win)

    def loadFromDir(self):
        """ charge les infos à partir du répertoire self.dir
        """
        # GUI
        self.ui.dirEdit.setText(self.dir)
        self.ui.browseDataFileButton.setDisabled(True)
        self.ui.browseDataFileButton.hide()
        self.ui.browseDirButton.hide()
        self.ui.groupBox.show()
        self.ui.setHistoricalButton.setDisabled(False)
        self.ui.setGeneticButton.setDisabled(False)

        # lecture du meta project
        if self.loadMyConf():
            # lecture de conf.hist.tmp
            self.hist_model_win.loadHistoricalConf()
            self.gen_data_win.loadGeneticConf()
            self.loadAnalysis()
        else:
            output.notify(self,"Load error","Impossible to read the project configuration")

    def putRefTableSize(self):
        """ met à jour l'affichage de la taille de la reftable
        et verrouille les modèles hist et gen si une reftable existe
        """
        size = self.readRefTableSize()
        if size != None:
            self.ui.nbSetsDoneEdit.setText("%s"%size)
            self.freezeHistModel()
            self.freezeGenData()
            self.ui.newAnButton.setDisabled(False)
        else:
            # si on n'a pas de reftable, on empêche la définition d'analyse
            self.ui.newAnButton.setDisabled(True)

    def freezeGenData(self,yesno=True):
        """ empêche la modification des genetic data tout en laissant
        la possibilité de les consulter
        """
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
            for e in self.gen_data_win.setSum_dico[g].findChildren(QCheckBox):
                e.setDisabled(yesno)
            self.gen_data_win.setSum_dico[g].ui.clearButton.setDisabled(yesno)
            self.gen_data_win.setSum_dico[g].ui.exitButton.setDisabled(yesno)
            self.gen_data_win.setSum_dico[g].ui.addAdmixButton.setDisabled(yesno)

            for e in self.gen_data_win.setSumSeq_dico[g].findChildren(QLineEdit):
                e.setDisabled(yesno)
            for e in self.gen_data_win.setSumSeq_dico[g].findChildren(QCheckBox):
                e.setDisabled(yesno)
            self.gen_data_win.setSumSeq_dico[g].ui.clearButton.setDisabled(yesno)
            self.gen_data_win.setSumSeq_dico[g].ui.exitButton.setDisabled(yesno)
            self.gen_data_win.setSumSeq_dico[g].ui.addAdmixButton.setDisabled(yesno)

    def freezeHistModel(self,yesno=True):
        """ empêche la modification du modèle historique tout en laissant
        la possibilité de le consulter
        """
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

    def readRefTableSize(self):
        """ lit la table de référence binaire pour en extraire la taille et la retourner
        """
        reftablefile = "%s/reftable.bin"%self.dir
        if os.path.exists(reftablefile):
            binint = array.array('i')
            f = open(reftablefile,'rb')
            binint.read(f,1)
            f.close()
            rtSize = binint[0]
            return rtSize
        else:
            return None


    def loadMyConf(self):
        """ lit le fichier conf.tmp pour charger le fichier de données
        """
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


    def saveAnalysis(self):
        """ sauvegarde la liste des analyses dans le dossier du projet
        """
        l_to_save = []
        for a in self.analysisList:
            l_to_save.append(a)
        f=open("%s/%s"%(self.dir,self.parent.analysis_conf_name),"wb")
        pickle.dump(l_to_save,f)
        f.close()
        print "save analysis"

    def loadAnalysis(self):
        """ charge les analyses suavegardées
        """
        if os.path.exists("%s/%s"%(self.dir,self.parent.analysis_conf_name)):
            f=open("%s/%s"%(self.dir,self.parent.analysis_conf_name),"rb")
            l = pickle.load(f)
            f.close()
            for a in l:
                self.addAnalysis(a)

    def save(self):
        """ sauvegarde du projet -> mainconf, histconf, genconf, theadconf
        Si le gen et hist sont valides, on génère le header
        """
        #print "je me save"
        if self.dir != None:
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
        else:
            output.notify(self,"Saving is impossible","Choose a directory before saving the project")

    def writeThConf(self):
        """ ecrit le header du tableau de resultat qui sera produit par les calculs
        il contient, les paramètres historicaux, les paramètres mutationnels, les summary stats
        """
        hist_params_txt = self.hist_model_win.getParamTableHeader()
        mut_params_txt = self.gen_data_win.getParamTableHeader()
        sum_stats_txt = self.gen_data_win.getSumStatsTableHeader()
        if os.path.exists(self.dir+"/%s"%self.parent.table_header_conf_name):
            os.remove("%s/%s"%(self.dir,self.parent.table_header_conf_name))
        f = codecs.open(self.dir+"/%s"%self.parent.table_header_conf_name,'w',"utf-8")
        f.write("scenario%s%s%s"%(hist_params_txt,mut_params_txt,sum_stats_txt))
        f.close()


    def setHistValid(self,valid):
        """ met à jour l'état du modèle historique
        et change l'icone du bouton en fonction de sa validité
        """
        self.hist_state_valid = valid
        self.verifyRefTableValid()
        if valid:
            self.ui.setHistoricalButton.setIcon(QIcon("docs/ok.png"))
        else:
            self.ui.setHistoricalButton.setIcon(QIcon("docs/redcross.png"))

    def setGenValid(self,valid):
        """ met à jour l'état des genetic data
        et change l'icone du bouton en fonction de la validité
        """
        self.gen_state_valid = valid
        self.verifyRefTableValid()
        if valid:
            self.ui.setGeneticButton.setIcon(QIcon("docs/ok.png"))
        else:
            self.ui.setGeneticButton.setIcon(QIcon("docs/redcross.png"))

    def verifyRefTableValid(self):
        """ Vérifie si tout est valide pour mettre à jour l'icone de l'onglet reference table
        """
        if self.gen_state_valid and self.hist_state_valid:
            self.setTabIcon(0,QIcon("docs/ok.png"))
            self.ui.runReftableButton.setDisabled(False)
        else:
            self.setTabIcon(0,QIcon("docs/redcross.png"))
            self.ui.runReftableButton.setDisabled(True)

    def lock(self):
        """ crée le fichier de verrouillage pour empêcher l'ouverture 
        du projet par une autre instance de DIYABC
        """
        f = open("%s/.DIYABC_lock"%self.dir,"w")
        f.write("%s"%os.getpid())
        f.close()

    def unlock(self):
        """ supprime le verrouillage sur le projet ssi le verrouillage 
        a été effectué par notre instance de DIYABC
        """
        if os.path.exists("%s/.DIYABC_lock"%self.dir):
            f = open("%s/.DIYABC_lock"%self.dir)
            pid = f.read()
            f.close()
            # on ne deverrouille que si c'est nous qui avons verrouillé
            if pid == str(os.getpid()):
                os.remove("%s/.DIYABC_lock"%self.dir)

    def checkAll(self):
        """ vérification du modèle historique et mutationnel
        cette fonction est appelée au chargement du projet pour restituer l'etat du projet
        """
        # historical model : 
        self.hist_model_win.definePriors(silent=True)
        if self.hist_model_win.checkAll(silent=True):
            self.setHistValid(True)
            self.hist_model_win.majProjectGui()
        # mutation model : plus facile d'utiliser directement la validation
        self.gen_data_win.validate(silent=True)

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


    def run (self):
        filename = self.tarname
        host = str(self.parent.parent.preferences_win.ui.addrEdit.text())
        port = int(str(self.parent.parent.preferences_win.ui.portEdit.text()))


        f=open(filename, 'rb')
        s = socket(AF_INET, SOCK_STREAM)
        try:
            s.connect((host, port))
        except error,msg:
            print "could not contact any server on %s:%s"%(host, port)
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

        print filename, " Sent!\n"
        while True:
            line = s.recv(8000)
            print line
            if len(line.split('somme '))>1:
                self.nb_done = int(line.split('somme ')[1].strip())
                self.emit(SIGNAL("increment"))
            if "durée" in line:
                self.nb_done = int(str(self.parent.ui.nbSetsReqEdit.text()))
                self.emit(SIGNAL("increment"))
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

    def run (self):
        # lance l'executable
        try:
            #print "/home/julien/vcs/git/diyabc/src-JMC-C++/gen -r %s -p %s"%(self.nb_to_gen,self.parent.dir)
            #exPath = str(self.parent.parent.preferences_win.ui.execPathEdit.text())
            exPath = self.parent.parent.preferences_win.getExecutablePath()
            nbMaxThread = self.parent.parent.preferences_win.getMaxThreadNumber()
            cmd_args_list = [exPath,"-p", "%s/"%self.parent.dir, "-r", "%s"%self.nb_to_gen, "-m", "-t", "%s"%nbMaxThread]
            print " ".join(cmd_args_list)
            #outfile = os.path.expanduser("~/.diyabc/general.out")
            outfile = "%s/general.out"%self.parent.dir
            if os.path.exists(outfile):
                os.remove(outfile)
            fg = open(outfile,"w")
            p = subprocess.Popen(cmd_args_list, stdout=fg, stdin=PIPE, stderr=STDOUT) 
        except Exception,e:
            print "Cannot find the executable of the computation program %s"%e
            self.problem = "Cannot find the executable of the computation program \n%s"%e
            self.emit(SIGNAL("refTableProblem"))
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
            print lines
            if len(lines) > 1:
                if lines[0].strip() == "OK":
                    red = int(lines[1])
                    if len(lines)>2:
                        self.time = lines[2]
                    if red > self.nb_done:
                        self.nb_done = red
                        self.emit(SIGNAL("increment"))
                elif lines[0].strip() == "END":
                    red = int(lines[1])
                    self.nb_done = red
                    self.emit(SIGNAL("increment"))
                    fg.close()
                    os.remove("%s/reftable.log"%(self.parent.dir))
                    return
                else:
                    print "lines != OK"
                    self.problem = lines[0].strip()
                    self.emit(SIGNAL("refTableProblem"))
                    #output.notify(self,"problem",lines[0])
                    fg.close()
                    return
            # verification de l'arret du programme
            if p.poll() != None:
                fg.close()
                g = open(outfile,"r")
                data= g.readlines()
                #print "data:%s"%data
                #print "poll:%s"%p.poll()
                g.close()
                if self.nb_done < self.nb_to_gen:
                    self.problem = "Reftable generation program exited anormaly"
                    self.emit(SIGNAL("refTableProblem"))
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

    def readProgress(self):
        if os.path.exists("%s/%s_progress.txt"%(self.parent.dir,self.analysis.name)):
            a = open("%s/%s_progress.txt"%(self.parent.dir,self.analysis.name),"r")
            b = a.readlines()[0]
            a.close()
        else:
            b = ""
        print "prog:%s"%b
        return b

    def updateProgress(self):
        b = self.readProgress()
        # on a bougé
        if len(b.split(' ')) > 1:
            t1 = float(b.split(' ')[0])
            t2 = float(b.split(' ')[1])
            self.tmpp = int(t1*100/t2)
        if self.tmpp != self.progress:
            print "on a progressé"
            self.progress = self.tmpp
            self.emit(SIGNAL("analysisProgress"))

    def run(self):
        #executablePath = str(self.parent.parent.preferences_win.ui.execPathEdit.text())
        executablePath = self.parent.parent.preferences_win.getExecutablePath()
        nbMaxThread = self.parent.parent.preferences_win.getMaxThreadNumber()
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
            cmd_args_list = [executablePath,"-p", "%s/"%self.parent.dir, "-d", '%s'%params, "-i", '%s'%self.analysis.name, "-m", "-t", "%s"%nbMaxThread]
            print " ".join(cmd_args_list)
            outfile = "%s/pre-ev.out"%self.parent.dir
            f = open(outfile,"w")
            p = subprocess.call(cmd_args_list, stdout=f, stdin=PIPE, stderr=STDOUT) 
            f.close()
            print "call ok"

            f = open(outfile,"r")
            data= f.read()
            f.close()
            self.progress = 100
            self.emit(SIGNAL("analysisProgress"))
            return

        # pour toutes les autres analyses le schema est le même
        cmd_args_list = [executablePath,"-p", "%s/"%self.parent.dir, "%s"%option, '%s'%params, "-i", '%s'%self.analysis.name, "-m", "-t", "%s"%nbMaxThread]
        print " ".join(cmd_args_list)
        outfile = "%s/%s.out"%(self.parent.dir,self.analysis.category)
        f = open(outfile,"w")
        p = subprocess.Popen(cmd_args_list, stdout=f, stdin=PIPE, stderr=STDOUT) 
        #f.close()
        print "popen ok"


        # la scrutation de la progression est identique pour toutes les analyses
        self.progress = 1
        self.tmpp = 1
        self.emit(SIGNAL("analysisProgress"))
        while True:
            self.updateProgress()
            # verification de l'arret du programme
            if p.poll() != None:
                f.close()
                g = open(outfile,"r")
                data= g.read()
                print "data:%s"%data
                #print "poll:%s"%p.poll()
                g.close()
                # on attend un peu pour etre sur que l'ecriture de la progression a été effectué
                time.sleep(2)
                self.updateProgress()
                if self.progress < 100:
                    self.problem = "Analysis program exited before the end of the analysis (%s%%)"%self.progress
                    self.emit(SIGNAL("analysisProblem"))
                    return
            time.sleep(2)

