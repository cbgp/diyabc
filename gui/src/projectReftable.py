# -*- coding: utf-8 -*-

## @package python_interface.projectReftable
# @author Julien Veyssier
#
# @brief Projets pour créer une table de référence


import pickle,mimetypes
from socket import *
import time
import os,copy
import shutil
import codecs
import subprocess
import tarfile,stat, sys
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui,QtCore
from analysis.defineNewAnalysis import DefineNewAnalysis
from analysis.drawEstimationAnalysisResult import DrawEstimationAnalysisResult
from analysis.drawComparisonAnalysisResult import DrawComparisonAnalysisResult
from analysis.drawPCAAnalysisResult import DrawPCAAnalysisResult
from analysis.viewAnalysisParameters import ViewAnalysisParameters
from viewTextFile import ViewTextFile
from utils.executableLauncherThread import LauncherThread
from datetime import datetime
import os.path
import output
from utils.cbgpUtils import log, addLine,getFsEncoding
import utils.cbgpUtils as cbgpUtils
import utils.diyabcUtils as diyabcUtils
from project import Project
import variables
from variables import DATAPATH

## @class ProjectReftable
# @brief Projets pour créer une table de référence
class ProjectReftable(Project):
    """ classe qui représente un projet
    par defaut, un projet est considéré comme nouveau, cad que l'affichage est celui d'un projet vierge
    pour un projet chargé, on modifie l'affichage en conséquence dans loadFromDir
    """



    __DICO_CATEGORY_DIR_NAME = {
                "estimate" : "estimation",
                "compare" : "comparison",
                "confidence" : "confidence",
                "confidence_posterior_global" : "confidence",
                "confidence_prior_global" : "confidence",
                "confidence_prior_specific" : "confidence",
                "modelChecking" : "modelChecking",
                "pre-ev" : "pca",
                "bias" : "bias"
                }

    def __init__(self,name,dir=None,parent=None):
        self.dataFileSource = ""
        self.dataFileName = ""
        self.analysisList = []
        self.analysisQueue = []
        self.dicoFrameAnalysis = {}

        self.thAnalysis = None

        super(ProjectReftable,self).__init__(name,dir,parent)
        self.fsCoding = getFsEncoding(logLevel=False)
        # creation du fichier .diyabcproject au cas où il n'existe pas
        if self.dir != None and not os.path.exists((u"%s/%s.diyabcproject"%(self.dir,self.name)).encode(self.fsCoding)):
            f = open((u"%s/%s.diyabcproject"%(self.dir,self.name)).encode(self.fsCoding),'w')
            f.write("created in %s"%self.dir)
            f.close()

        # manual alignment set
        self.ui.verticalLayout_2.setAlignment(self.ui.newAnButton,Qt.AlignCenter)
        self.ui.verticalLayout_3.setAlignment(self.ui.progressBar,Qt.AlignCenter)

        QObject.connect(self.ui.newAnButton,SIGNAL("clicked()"),self.defineNewAnalysis)
        QObject.connect(self.ui.browseDataFileButton,SIGNAL("clicked()"),self.dataFileSelectionAndCopy)

        self.connect(self.ui.runReftableButton, SIGNAL("clicked()"),self,SLOT("launchReftableGeneration()"))
        self.connect(self.ui.stopReftableButton, SIGNAL("clicked()"),self.stopRefTableGen)

    def canBeDeleted(self):
        return True

    def canBeCloned(self):
        return True

    def stopActivities(self):
        self.stopRefTableGen()
        self.stopAnalysis()
        self.unlock()

    def importRefTableFile(self,filename=None):
        """ Import reference table in given or selected file and merge it
        with the current one if there was one
        """

        if filename == None:
            filename = QFileDialog.getOpenFileName(self,"Select reference table file","%s"%os.path.dirname((str(self.dir)).encode(self.fsCoding)),"Binary file (*.bin);;all files (*)")
            if filename == None or str(filename) == "":
                return
        elif not os.path.exists((u"%s"%filename).encode(self.fsCoding)):
            return

        if mimetypes.guess_type(str(filename))[0] != "application/octet-stream":
           output.notify(self,"Bad reftable file","The file you want to import is not a binary file and probably not a reference table file.")
           return
        shutil.copy(filename,(u"%s/reftable_1.bin"%(self.dir)).encode(self.fsCoding))
        if os.path.exists((u"%s/reftable.bin"%self.dir).encode(self.fsCoding)):
            shutil.move((u"%s/reftable.bin"%self.dir).encode(self.fsCoding),(u"%s/reftable_2.bin"%self.dir).encode(self.fsCoding))
        exPath = self.parent.preferences_win.getExecutablePath()
        if exPath != "":
            cmd_args_list = [exPath,"-p", "%s/"%self.dir, "-q"]
            outfile = u"%s/import.out"%self.dir
            if os.path.exists(outfile.encode(self.fsCoding)):
                os.remove(outfile.encode(self.fsCoding))
            fg = open(outfile.encode(self.fsCoding),"w")
            p = subprocess.Popen(cmd_args_list, stdout=fg, stdin=subprocess.PIPE, stderr=subprocess.STDOUT)
            while p.poll() == None:
                time.sleep(1)
            fg.close()
            if p.poll() == 0:
                log(3,"Reftable import and merge successfull")
                self.parent.reloadCurrentProject()
            else:
                log(3,"Reftable import and merge FAILED with exit code %s"%p.poll())


    def deleteReftable(self):
        """ Delete the reftable binary file in the project
        directory if it exists.
        """

        # si aucune manipulation sur la reftable :
        if self.th == None:
            # si il existe une reftable
            if os.path.exists((u"%s/reftable.bin"%self.dir).encode(self.fsCoding)):
                reply = QMessageBox.question(self,"Warning","If you delete the reference table, all analysis will also be deleted.\n\n Are you sure you want to delete the reference table of project %s"%self.name,QtGui.QMessageBox.Yes | QtGui.QMessageBox.No,QtGui.QMessageBox.No)
                if reply == QtGui.QMessageBox.Yes:
                    # effacer le fichier
                    os.remove((u"%s/reftable.bin"%self.dir).encode(self.fsCoding))
                    # effacer les analyses
                    for aframe in self.dicoFrameAnalysis.keys():
                        self.removeAnalysis(aframe)
                    # afficher la taille (0) de la reftable
                    self.putRefTableSize()
                    self.save()
            else:
                output.notify(self,"Deletion error","Reference table file does not exist")
        else:
            output.notify(self,"Deletion error","Impossible to delete the reference table during its generation")

    def stopRefTableGen(self):
        """ tue le thread de génération et crée le fichier .stop pour
        arrêter la génération de la reftable
        """

        if os.path.exists((u"%s"%self.dir).encode(self.fsCoding)):
            if self.th != None:
                self.th.terminate()
            f = open((u"%s/.stop"%self.dir).encode(self.fsCoding),"w")
            f.write(" ")
            f.close()
            time.sleep(1)
            need_to_start_analysis = (self.th != None and len(self.analysisQueue)>0)
            if self.th != None:
                self.th.killProcess()
                self.th = None
            self.stopUiGenReftable()
            if os.path.exists((u"%s/reftable.log"%(self.dir)).encode(self.fsCoding)):
                os.remove((u"%s/reftable.log"%(self.dir)).encode(self.fsCoding))
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
            the_frame.findChild(QPushButton,"analysisEdButton").setDisabled(False)
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
        anDir = analysis.name
        anDir += "_"
        anDir += self.__DICO_CATEGORY_DIR_NAME[analysis.category]

        typestr = self.__DICO_CATEGORY_DIR_NAME[analysis.category]
        if typestr == 'estimation':
            self.drawAnalysisFrame = DrawEstimationAnalysisResult(analysis,anDir,self)
        elif typestr == 'comparison':
            self.drawAnalysisFrame = DrawComparisonAnalysisResult(analysis,anDir,self)
        elif typestr == "pca" or typestr == "modelChecking":
            if os.path.exists((u"%s/analysis/%s/ACP.txt"%(self.dir,anDir)).encode(self.fsCoding)) or os.path.exists((u"%s/analysis/%s/mcACP.txt"%(self.dir,anDir)).encode(self.fsCoding)):
                self.drawAnalysisFrame = DrawPCAAnalysisResult(analysis,anDir,self)
            else:
                if typestr == "pca":
                    f = open((u"%s/analysis/%s/locate.txt"%(self.dir,anDir)).encode(self.fsCoding),'r')
                elif typestr == "modelChecking":
                    f = open((u"%s/analysis/%s/mclocate.txt"%(self.dir,anDir)).encode(self.fsCoding),'r')
                data = f.read()
                f.close()
                log(3,"Viewing analysis results, PCA or modelChecking")
                self.drawAnalysisFrame = ViewTextFile(data,self.returnToAnalysisList,self)
                self.drawAnalysisFrame.choiceFrame.hide()

        elif typestr == "confidence":
            f = open((u"%s/analysis/%s/confidence.txt"%(self.dir,anDir)).encode(self.fsCoding),'r')
            data = f.read()
            f.close()
            log(3,"Viewing analysis results, confidence in scenario choice")
            self.drawAnalysisFrame = ViewTextFile(data,self.returnToAnalysisList,self)
            self.drawAnalysisFrame.choiceFrame.hide()
        elif typestr == "bias":
            f = open((u"%s/analysis/%s/bias_original.txt"%(self.dir,anDir)).encode(self.fsCoding),'r')
            data = f.read()
            f.close()
            self.drawAnalysisFrame = ViewTextFile(data,self.returnToAnalysisList,self)
            self.drawAnalysisFrame.analysis = analysis
            self.drawAnalysisFrame.anDir = anDir
            ui = self.drawAnalysisFrame
            log(3,"Viewing analysis results, bias")
            if not os.path.exists((u"%s/analysis/%s/bias_composite.txt"%(self.dir,self.drawAnalysisFrame.anDir)).encode(self.fsCoding)):
                ui.cRadio.hide()
            if not os.path.exists((u"%s/analysis/%s/bias_scaled.txt"%(self.dir,self.drawAnalysisFrame.anDir)).encode(self.fsCoding)):
                ui.sRadio.hide()

            QObject.connect(ui.oRadio,SIGNAL("clicked()"),self.biasResultFileChanged)
            QObject.connect(ui.cRadio,SIGNAL("clicked()"),self.biasResultFileChanged)
            QObject.connect(ui.sRadio,SIGNAL("clicked()"),self.biasResultFileChanged)
        self.ui.analysisStack.addWidget(self.drawAnalysisFrame)
        self.ui.analysisStack.setCurrentWidget(self.drawAnalysisFrame)

        if (typestr == "pca" or typestr == "modelChecking")\
        and (os.path.exists((u"%s/analysis/%s/ACP.txt"%(self.dir,anDir)).encode(self.fsCoding))\
        or os.path.exists((u"%s/analysis/%s/mcACP.txt"%(self.dir,anDir)).encode(self.fsCoding))):
            try:
                self.drawAnalysisFrame.loadACP()
            except Exception as e:
                output.notify(self,"ACP loading error","Error during ACP file parsing\n\n%s"%e)
                QApplication.restoreOverrideCursor()

    def biasResultFileChanged(self):
        but = self.sender()
        if but == self.drawAnalysisFrame.oRadio:
            filename = "original"
        elif but == self.drawAnalysisFrame.cRadio:
            filename = "composite"
        elif but == self.drawAnalysisFrame.sRadio:
            filename = "scaled"
        f = open((u"%s/analysis/%s/bias_%s.txt"%(self.dir,self.drawAnalysisFrame.anDir,filename)).encode(self.fsCoding),'r')
        data = f.read()
        f.close()
        self.drawAnalysisFrame.dataPlain.clear()
        self.drawAnalysisFrame.dataPlain.setPlainText(data)

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
        self.ui.analysisStack.addWidget(def_analysis)
        self.ui.analysisStack.setCurrentWidget(def_analysis)

    def genMasterScript(self):
        """ génération du script à exécuter sur le noeud maitre
                numSimulatedDataSet=
                numSimulatedDataSetByJob=
                coresPerJob=
                maxConcurrentJobs=
                seed=
                diyabcPath=
                projectName=""
                dataFileName=""
                referenceTableHeaderName=""
        """
        numSimulatedDataSet = int(self.ui.nbSetsReqEdit.text()) - int(self.ui.nbSetsDoneEdit.text())
        numSimulatedDataSetByJob = int(self.parent.preferences_win.ui.numSimulatedDataSetByJobEdit.text())
        coresPerJob = int(self.parent.preferences_win.ui.coresPerJobEdit.text())
        maxConcurrentJobs = int(self.parent.preferences_win.ui.maxConcurrentJobsEdit.text())
        seed = int(self.parent.preferences_win.ui.seedEdit.text())
        diyabcPath=""
        if self.parent.preferences_win.ui.clusterBinLocationCombo.currentText() == "local" :
            diyabcPath = os.path.join('./', os.path.basename((str(self.parent.preferences_win.ui.diyabcPathLocalPathEdit.text())).encode(self.fsCoding)))
        elif self.parent.preferences_win.ui.clusterBinLocationCombo.currentText() == "cluster" :
            diyabcPath = str(self.parent.preferences_win.ui.diyabcPathClusterEdit.text())
        projectName = str(self.ui.projNameLabelValue.text())
        referenceTableHeaderName = self.parent.reftableheader_name
        dataFileName = self.dataFileName

        #str(self.ui.parent.preferences_win.ui.scriptMasterFirstTextEdit.toPlainText())
        masterScriptFirstPart= \
               self.parent.preferences_win.DEFAULT_FIRST_PART_SCRIPT_CLUSTER\
               .format(numSimulatedDataSet,
               numSimulatedDataSetByJob,
               coresPerJob,
               maxConcurrentJobs,
               seed,
               diyabcPath,
               projectName,
               dataFileName,
               referenceTableHeaderName)
        self.ui.parent.preferences_win.ui.scriptMasterFirstTextEdit.setText(masterScriptFirstPart)
        res = str(self.ui.parent.preferences_win.ui.scriptMasterFirstTextEdit.toPlainText()) \
                + str(self.ui.parent.preferences_win.ui.scriptMasterLastTextEdit.toPlainText())

        return res

    def genNodeScript(self):
        """ génération du script a exécuter sur chaque noeud
        """
        f = open((DATAPATH+"/txt/node_cluster.sh").encode(self.fsCoding),"rU")
        script= f.read()
        f.close()
        return script

    def generateComputationTar(self,tarname=None):
        """ génère une archive tar contenant l'exécutable, les scripts node et master,
        le datafile et le reftableheader.
        """

        if tarname == None:
            output.notify(self,"Warning","You are about to generate an archive in order to compute on a cluster,\n This archive will contain the datafile, the header file, the diyabc binary (optional) and the scripts to launch computations on an cluster main node")
            tarname = u"%s" % (QFileDialog.getSaveFileName(self,"Saving cluster archive","%s_reftableCluster"%str(self.ui.projNameLabelValue.text()),"TAR archive (*.tar)"))
            if tarname == u"":
                return ""
            if not tarname.endswith(u".tar"):
                tarname += u".tar"
        if tarname != "":
            dest = u"%s/cluster_generation_tmp/"%self.dir
            if os.path.exists(dest.encode(self.fsCoding)):
                shutil.rmtree(dest.encode(self.fsCoding))
            os.mkdir(dest.encode(self.fsCoding))

            # generation du master script
            script = self.genMasterScript()
            scmffile = u"%s/scmf"%dest
            if os.path.exists(scmffile.encode(self.fsCoding)):
                os.remove(scmffile.encode(self.fsCoding))
            scmf = open(scmffile.encode(self.fsCoding),'w')
            scmf.write(script)
            scmf.close()
            os.chmod(scmffile.encode(self.fsCoding),stat.S_IRUSR|stat.S_IWUSR|stat.S_IXUSR|stat.S_IRGRP|stat.S_IWGRP|stat.S_IXGRP|stat.S_IROTH|stat.S_IWOTH|stat.S_IXOTH)

            # generation du node script
            script = self.genNodeScript()
            scnffile = u"%s/scnf"%dest
            if os.path.exists(scnffile.encode(self.fsCoding)):
                os.remove(scnffile.encode(self.fsCoding))
            scnf = open(scnffile.encode(self.fsCoding),'w')
            scnf.write(script)
            scnf.close()
            os.chmod(scnffile.encode(self.fsCoding),stat.S_IRUSR|stat.S_IWUSR|stat.S_IXUSR|stat.S_IRGRP|stat.S_IWGRP|stat.S_IXGRP|stat.S_IROTH|stat.S_IWOTH|stat.S_IXOTH)

            # ajout des fichiers dans l'archive
            tarRepName = self.dir.split('/')[-1]
            if os.path.exists(tarname.encode(self.fsCoding)):
                os.remove(tarname.encode(self.fsCoding))
            tar = tarfile.open(tarname.encode(self.fsCoding),"w")
            tar.add(scmffile.encode(self.fsCoding),u'%s/launch.sh'%tarRepName)
            tar.add(scnffile.encode(self.fsCoding),u'%s/node.sh'%tarRepName)
            tar.add((u"%s/%s"%(self.dir,self.parent.reftableheader_name)).encode(self.fsCoding),u"%s/%s"%(tarRepName,self.parent.reftableheader_name))
            tar.add(self.dataFileSource.encode(self.fsCoding),u"%s/%s"%(str(tarRepName),str(self.dataFileName)))
            if self.parent.preferences_win.ui.clusterBinLocationCombo.currentText() == "local" :
                diyabcLocalPath = str(self.parent.preferences_win.ui.diyabcPathLocalPathEdit.text())
                if not os.path.exists(diyabcLocalPath.encode(self.fsCoding)) :
                    output.notify(self,"Value Error : cluster bin path","cluster binary not found. \n%s does not exists\Please verify cluster binary path options in preferences."%self.ui.diyabcPathLocalPathEdit.text())
                tar.add(diyabcLocalPath.encode(self.fsCoding),u"%s/%s"%(tarRepName, os.path.basename(diyabcLocalPath)))
            tar.close()
            # nettoyage des fichiers temporaires de script
            if os.path.exists(dest.encode(self.fsCoding)):
                shutil.rmtree(dest.encode(self.fsCoding))

        return tarname

    @pyqtSignature("")
    def launchReftableGeneration(self):
        """ Lance un thread de generation de la reftable
        """
        particleLoopSize = str(self.parent.preferences_win.particleLoopSizeEdit.text())
        try:
            nb_to_gen = int(self.ui.nbSetsReqEdit.text())
        except Exception as e:
            output.notify(self,"value error","Check the value of required number of data sets. Must be a positive integer.\n\n%s"%e)
            return
        if nb_to_gen <= int(self.ui.nbSetsDoneEdit.text()):
            output.notify(self,"value error","Please ask for more records than it already exists")
            return
            # lire le fichier maf et s'assurer que la maf de data file correspond
        modLoop = (nb_to_gen-int(self.ui.nbSetsDoneEdit.text()))%int(particleLoopSize)
        if modLoop != 0 :
            warning = "Your loop size being %s you will obtain a total of %s simulated datasets." \
                                      % (particleLoopSize,(nb_to_gen-modLoop+int(particleLoopSize)))
            qmb = QMessageBox()
            qmb.setText(warning)
            qmb.setStandardButtons(QMessageBox.Cancel | QMessageBox.Ok )
            qmb.setDefaultButton(QMessageBox.Cancel)
            reply = qmb.exec_()
            if (reply == QMessageBox.Cancel):
                return
        log(1,"Starting generation of the reference table")

        self.save()
        self.writeRefTableHeader()
        if self.th == None and self.thAnalysis == None:
            # checkRam return false results. We deseable it
            #cbgpUtils.checkRam()
            if not os.path.exists((u"%s/reftable.bin"%self.dir).encode(self.fsCoding)) and os.path.exists((u"%s/reftable.log"%self.dir).encode(self.fsCoding)):
                os.remove((u"%s/reftable.log"%self.dir).encode(self.fsCoding))


            # on demarre le thread local ou cluster
            if self.parent.preferences_win.ui.useClusterCheck.isChecked():
                #tname = self.generateComputationTar("%s/aaaa.tar"%tempfile.mkdtemp())
                tname = self.generateComputationTar()
                if tname != "":
                    log(3,"Tar file created in %s"%tname)
                    output.notify(self,"Notify","The archive has been generated in %s"%tname)
                else :
                    log(1,"CANCEL : Tar file not created")
                #self.th = RefTableGenThreadCluster(self,tname,nb_to_gen)
            else:
                exPath = self.parent.preferences_win.getExecutablePath()
                if exPath == "":
                    self.stopUiGenReftable()
                    return
                if not self.goodRNGSize():
                    self.initializeRNG()
                self.startUiGenReftable()
                self.nbReqBeforeComput = int(str(self.ui.nbSetsDoneEdit.text()))
                progressfile = "%s/reftable.log"%self.dir
                outfile = "%s/general.out"%self.dir
                signames = {
                    "progress" : "reftableProgress",
                    "log" : "reftableLog",
                    "termSuccess" : "reftableTermSuccess",
                    "termProblem" : "reftableProblem",
                    "newOutput" : "reftableNewOutput"
                }

                nbMaxThread = self.parent.preferences_win.getMaxThreadNumber()
                cmd_args_list = [exPath,"-p", "%s/"%self.dir, "-r", "%s"%nb_to_gen , "-g", "%s"%particleLoopSize ,"-m", "-t", "%s"%nbMaxThread]

                cmd_args_list_quoted = list(cmd_args_list)
                for i in range(len(cmd_args_list_quoted)):
                    if ";" in cmd_args_list_quoted[i] or " " in cmd_args_list_quoted[i] or ":" in cmd_args_list_quoted[i]:
                        cmd_args_list_quoted[i] = '"'+cmd_args_list_quoted[i]+'"'

                self.th = LauncherThread("%s reference table generation"%self.name,cmd_args_list,outfile_path=outfile,progressfile_path=progressfile,signalnames=signames)
                self.th.nb_to_gen = nb_to_gen
                self.th.connect(self.th,SIGNAL("reftableProgress(QString)"),self.reftableProgress)
                self.th.connect(self.th,SIGNAL("reftableTermSuccess()"),self.reftableTermSuccess)
                self.th.connect(self.th,SIGNAL("reftableProblem(QString)"),self.reftableProblem)
                self.th.connect(self.th,SIGNAL("reftableLog(int,QString)"),self.reftableLog)
                self.th.start()
                addLine("%s/command.txt"%self.dir,"Command launched : %s\n\n"%" ".join(cmd_args_list_quoted))
        else:
            output.notify(self,"Impossible to launch","An analysis is processing,\
                    \nimpossible to launch reftable generation")

    def reftableProblem(self,msg):
        output.notify(self,"reftable problem","Something happened during the reftable generation :\n %s"%(msg))
        self.parent.systrayHandler.showTrayMessage("DIYABC : reftable","Reference table generation of project\n{0} has failed".format(self.name))
        self.stopRefTableGen()
        self.nextAnalysisInQueue()

    def reftableLog(self,lvl,msg):
        if self.th != None:
            log(lvl,msg)

    def stopUiGenReftable(self):
        self.ui.runReftableButton.setText("Run computations")
        self.ui.runReftableButton.setDisabled(False)
        self.ui.progressBar.setValue(0)
        self.ui.progressBar.hide()
        self.ui.stopReftableButton.hide()
        self.ui.fromToFrame.hide()

    def startUiGenReftable(self):
        self.ui.runReftableButton.setText("Running ...")
        self.ui.runReftableButton.setDisabled(True)
        self.ui.progressBar.show()
        self.ui.stopReftableButton.show()
        self.ui.fromToFrame.show()
        self.ui.nbReqFromLabel.setText("from %s"%self.ui.nbSetsDoneEdit.text())
        self.ui.nbReqToLabel.setText("to %s"%self.ui.nbSetsReqEdit.text())

    def reftableProgress(self,progressFileContent):
        """Incremente la barre de progression de la génération de la reftable
        """
        log(3,"[project %s] Reference table generation progress file content has changed : %s"%(self.name,str(progressFileContent).replace('\n',' ')))
        lines = str(progressFileContent).split('\n')
        try :
            if len(lines) > 2:
                if lines[0].strip() != "OK" and lines[0].strip() != "END":
                    self.reftableProblem("Executable says there is a problem in the progress file (%s)"%lines[0].strip())
                    return
                done = int(lines[1])
                time_remaining = lines[2]
            else:
                #self.reftableProblem("Progress file parsing error")
                log(3,"Progress file parsing error, number of lines < 3")
                return
        except Exception :
            log(3,"Progress file parsing error, unknown error")
            time_remaining = "unknown"
            done = int(str(self.ui.nbSetsDoneEdit.text()))
            pass

        nb_to_do = self.th.nb_to_gen
        self.ui.runReftableButton.setText("Running ... %s remaining"%(time_remaining.replace('\n',' ')))
        pc = (float(done - self.nbReqBeforeComput)/float(nb_to_do - self.nbReqBeforeComput))*100
        if pc > 0 and pc < 1:
            pc = 1
        self.ui.progressBar.setValue(int(pc))
        oldDone = int(self.ui.nbSetsDoneEdit.text())
        self.ui.nbSetsDoneEdit.setText("%s"%done)
        # si c'est le premier inc : on freeze et on permet la création d'analyse
        if oldDone == 0 and int(done) > 0:
            self.freezeHistModel()
            self.freezeGenData()
            self.ui.newAnButton.setDisabled(False)

    def reftableTermSuccess(self):
        # si on a fini, on met à jour l'affichage de la taille de la reftable
        # et on verrouille eventuellement histmodel et gendata
        log(2,"reference table generation of project %s finished with a success code"%self.name)
        self.parent.systrayHandler.showTrayMessage("DIYABC : reftable","Reference table generation of project\n{0} has finished".format(self.name))
        self.putRefTableSize()
        self.stopUiGenReftable()
        if int(str(self.ui.nbSetsDoneEdit.text())) < int(str(self.ui.nbSetsReqEdit.text())) :
            log(1,"reference table generation of project %s finished with a success code but the number of data sets asked are not done."%self.name)
            output.notify(self,"Generation Error","Reference table generation of project %s finished with a success code. \
            \nBut the total required number of data sets are not done !\n\nThis situation can occur when the operating system maximum file size is reached for reftable.bin file."%self.name)
        self.th = None
        self.nextAnalysisInQueue()

    def writeRefTableHeader(self):
        """ écriture du header.txt à partir des conf
        """
        if os.path.exists((u"%s/%s"%(self.dir,self.parent.main_conf_name)).encode(self.fsCoding)) and \
           os.path.exists((u"%s/%s"%(self.dir,self.parent.hist_conf_name)).encode(self.fsCoding)) and \
           os.path.exists((u"%s/%s"%(self.dir,self.parent.gen_conf_name)).encode(self.fsCoding)) and \
           os.path.exists((u"%s/%s"%(self.dir,self.parent.table_header_conf_name)).encode(self.fsCoding)):
            if os.path.exists((u"%s/%s"%(self.dir,self.parent.reftableheader_name)).encode(self.fsCoding)):
                os.remove((u"%s/%s" %(self.dir,self.parent.reftableheader_name)).encode(self.fsCoding))
            log(2,"Writing reference table header from the conf files")

            fdest = codecs.open((u"%s/%s"%(self.dir,self.parent.reftableheader_name)).encode(self.fsCoding),"w","utf-8")
            for name in [self.parent.main_conf_name,self.parent.hist_conf_name,self.parent.gen_conf_name,self.parent.table_header_conf_name]:
                fo = codecs.open((u"%s/%s"%(self.dir,name)).encode(self.fsCoding),"rU","utf-8")
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
            # selection du dernier chemin pour open
            default_path = os.path.expanduser(u"~/".encode(self.fsCoding))
            if self.parent.preferences_win.getLastFolder("open") != "":
                default_path = self.parent.preferences_win.getLastFolder("open")
            name = QFileDialog.getOpenFileName(self,"Select datafile","%s"%default_path,self.getDataFileFilter())
        if self.loadDataFile(name):
            # si on a reussi a charger le data file, on vire le bouton browse
            self.ui.browseDataFileButton.hide()
            # et on copie ce datafile dans le dossier projet
            if not os.path.exists((u"%s/%s"%(self.dir,self.dataFileSource.split('/')[-1])).encode(self.fsCoding)):
                shutil.copy(str(self.dataFileSource).encode(self.fsCoding),(u"%s/%s"%(self.dir,self.dataFileSource.split('/')[-1])).encode(self.fsCoding))
            self.dataFileName = self.dataFileSource.split('/')[-1]
            self.ui.dataFileEdit.setText("%s/%s"%(self.dir,self.dataFileSource.split('/')[-1]))
            self.ui.groupBox_6.show()
            self.ui.groupBox_7.show()
            self.ui.groupBox_8.show()
            # quand on a choisi le datafile, on active le menu projet en mettant à jour
            self.parent.updateCurrentProjectMenu(self.parent.tabWidget.currentIndex())
            self.parent.addRecent(self.dir)
            return True
        return False

    def dirCreation(self,path):
        """ selection du repertoire pour un nouveau projet et copie du fichier de données
        """
        log(2,"Creation and files copy of folder '%s'"%path)
        if path != "":
            if not self.parent.isProjDir(path.encode(self.fsCoding)):
                # name_YYYY_MM_DD-num le plus elevé
                dd = datetime.now()
                cd = 100
                while cd > 0 and not os.path.exists((path+u"_%i_%i_%i-%i"%(dd.year,dd.month,dd.day,cd)).encode(self.fsCoding)):
                    cd -= 1
                if cd == 100:
                    output.notify(self,"Error","With this version, you cannot have more than 100 \
                                project directories\nfor the same project name and in the same directory")
                else:
                    newdir = str(path+"_%i_%i_%i-%i"%(dd.year,dd.month,dd.day,(cd+1)))
                    self.ui.dirEdit.setText(newdir)
                    try:
                        os.mkdir(newdir.encode(self.fsCoding))
                        self.ui.setHistoricalButton.setDisabled(False)
                        self.ui.setGeneticButton.setDisabled(False)
                        self.dir = newdir
                        # verrouillage du projet
                        self.lock()
                        # creation du fichier .diyabcproject
                        if not os.path.exists((u"%s/%s.diyabcproject"%(self.dir,self.name)).encode(self.fsCoding)):
                            f = open((u"%s/%s.diyabcproject"%(self.dir,self.name)).encode(self.fsCoding),'w')
                            f.write("created in %s"%self.dir)
                            f.close()
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
            self.addAnalysisGui(analysis,analysis.name,"evaluate scenarios and priors",analysis.computationParameters,analysis.status)
        elif type_analysis == "estimate":
            self.addAnalysisGui(analysis,analysis.name,"estimate parameters","params",analysis.status)
        elif type_analysis == "bias":
            self.addAnalysisGui(analysis,analysis.name,"bias and precision",str(analysis.chosenSc),analysis.status)
        elif type_analysis == "compare":
            self.addAnalysisGui(analysis,analysis.name,"compare scenarios","%s | %s"%(analysis.candidateScList,analysis.chosenSc),analysis.status)
        elif "confidence" in type_analysis :
            self.addAnalysisGui(analysis,analysis.name,"confidence in scenario choice","%s | %s"%(analysis.candidateScList,analysis.chosenSc),analysis.status)
        elif type_analysis == "modelChecking":
            self.addAnalysisGui(analysis,analysis.name,"model checking","%s | %s"%(analysis.candidateScList,analysis.chosenSc),analysis.status)

    def addAnalysisGui(self,analysis,name,atype,params,status):
        """ crée les objets graphiques pour une analyse et les ajoute
        """

        frame_9 = QtGui.QFrame(self.ui.scrollAreaWidgetContents)
        frame_9.setFrameShape(QtGui.QFrame.StyledPanel)
        frame_9.setFrameShadow(QtGui.QFrame.Raised)
        frame_9.setObjectName("frame_analysis_%s"%name)
        frame_9.setMinimumSize(QtCore.QSize(0, 32))
        frame_9.setMaximumSize(QtCore.QSize(9999, 32))
        horizontalLayout_4 = QtGui.QHBoxLayout(frame_9)
        horizontalLayout_4.setObjectName("horizontalLayout_4")
        analysisEdButton = QtGui.QPushButton("Edit",frame_9)
        analysisEdButton.setMinimumSize(QtCore.QSize(35, 0))
        analysisEdButton.setMaximumSize(QtCore.QSize(35, 16777215))
        analysisEdButton.setObjectName("analysisEdButton")
        analysisEdButton.setFont(QFont("",8,-1,False))
        horizontalLayout_4.addWidget(analysisEdButton)
        analysisCpButton = QtGui.QPushButton("Copy",frame_9)
        analysisCpButton.setMinimumSize(QtCore.QSize(35, 0))
        analysisCpButton.setMaximumSize(QtCore.QSize(35, 16777215))
        analysisCpButton.setObjectName("analysisCpButton")
        analysisCpButton.setFont(QFont("",8,-1,False))
        horizontalLayout_4.addWidget(analysisCpButton)
        analysisRmButton = QtGui.QPushButton("Del",frame_9)
        analysisRmButton.setMinimumSize(QtCore.QSize(35, 0))
        analysisRmButton.setMaximumSize(QtCore.QSize(35, 16777215))
        analysisRmButton.setObjectName("analysisRmButton")
        analysisRmButton.setFont(QFont("",8,-1,False))
        horizontalLayout_4.addWidget(analysisRmButton)
        analysisNameLabel = QtGui.QLabel(name,frame_9)
        analysisNameLabel.setAlignment(Qt.AlignCenter)
        analysisNameLabel.setMinimumSize(QtCore.QSize(140, 0))
        analysisNameLabel.setMaximumSize(QtCore.QSize(140, 16777215))
        analysisNameLabel.setObjectName("analysisNameLabel")
        analysisNameLabel.setFrameShape(QtGui.QFrame.StyledPanel)
        horizontalLayout_4.addWidget(analysisNameLabel)
        analysisTypeLabel = QtGui.QLabel(atype,frame_9)
        analysisTypeLabel.setAlignment(Qt.AlignCenter)
        analysisTypeLabel.setObjectName("analysisTypeLabel")
        analysisTypeLabel.setFrameShape(QtGui.QFrame.StyledPanel)
        analysisTypeLabel.setMaximumSize(QtCore.QSize(200, 16777215))
        analysisTypeLabel.setMinimumSize(QtCore.QSize(200, 0))
        horizontalLayout_4.addWidget(analysisTypeLabel)
        analysisParamsButton = QtGui.QPushButton("View values",frame_9)
        analysisParamsButton.setObjectName("analysisParamsLabel")
        analysisParamsButton.setMaximumSize(QtCore.QSize(100, 16777215))
        analysisParamsButton.setMinimumSize(QtCore.QSize(100, 0))
        analysisParamsButton.setMaximumSize(QtCore.QSize(70, 16777215))
        horizontalLayout_4.addWidget(analysisParamsButton)
        analysisStatusBar = QtGui.QProgressBar(frame_9)
        analysisStatusBar.setMinimumSize(QtCore.QSize(110, 0))
        analysisStatusBar.setMaximumSize(QtCore.QSize(110, 16777215))
        analysisStatusBar.setAlignment(Qt.AlignCenter)
        analysisStatusBar.setObjectName("analysisStatusBar")
        horizontalLayout_4.addWidget(analysisStatusBar)
        if status == "finished":
            buttonLabel = "View results"
            analysisStatusBar.setValue(100)
            analysisEdButton.setDisabled(True)
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

        analysisStopButton = QtGui.QPushButton(QIcon(variables.ICONPATH+"/stop.png"),"Stop",frame_9)
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
        QObject.connect(analysisCpButton,SIGNAL("clicked()"),self.copyAnalysis)
        QObject.connect(analysisEdButton,SIGNAL("clicked()"),self.editAnalysis)
        QObject.connect(analysisButton,SIGNAL("clicked()"),self.tryLaunchViewAnalysis)
        QObject.connect(analysisParamsButton,SIGNAL("clicked()"),self.viewAnalysisParameters)
        QObject.connect(analysisStopButton,SIGNAL("clicked()"),self.stopAnalysis)

        self.parent.updateDoc(frame_9)

    def changeAnalysisName(self,analysis,new_name):
        analysis.name = new_name
        for fr in self.dicoFrameAnalysis.keys():
            if self.dicoFrameAnalysis[fr] == analysis:
                fr.findChild(QLabel,"analysisNameLabel").setText(new_name)

    def editAnalysis(self):
        """ démarre l'édition d'une analyse en passant par un define auquel
        on donne une analyse deja existante, il se débrouille
        """
        frame = self.sender().parent()
        analysis = self.dicoFrameAnalysis[frame]
        log(1,"Entering in analysis %s edition"%analysis.name)
        def_analysis = DefineNewAnalysis(self,self.hist_model_win.getNbScenario(),analysis)

        self.ui.analysisStack.addWidget(def_analysis)
        self.ui.analysisStack.setCurrentWidget(def_analysis)
        def_analysis.analysisNameEdit.setText(analysis.name)

    def copyAnalysis(self):
        frame = self.sender().parent()
        analysis = self.dicoFrameAnalysis[frame]
        dup = copy.deepcopy(analysis)
        maxnum = 0
        for an in self.analysisList:
            if an.name.startswith(analysis.name.split('-')[0]):
                numstr = str(an.name.split('-')[-1])
                if numstr.isdigit():
                    if int(numstr) > maxnum:
                        maxnum=int(numstr)
        dup.name = dup.name.split('-')[0]+'-'+str(maxnum+1)
        # changement de l'état de l'analyse
        dup.status = "new"
        log(1,"Copying analysis '%s' to '%s'"%(analysis.name,dup.name))
        self.addAnalysis(dup)

    def removeAnalysis(self,aframe=None):
        if aframe == None:
            frame = self.sender().parent()
        else:
            frame = aframe
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

        anDir = "%s/analysis/" % (self.dir)
        anDir += analysis.name
        anDir += "_"
        anDir += self.__DICO_CATEGORY_DIR_NAME[analysis.category]
        del self.dicoFrameAnalysis[frame]
        self.ui.verticalLayout_9.removeWidget(frame)
        if os.path.exists(anDir.encode(self.fsCoding)) :
            reply = QMessageBox.question(self,"Warning","The analysis has been removed from diyabc interface.\n\n Do you want to remove it also from your hard drive ?\n\n Delete directory : %s ?"%anDir,QtGui.QMessageBox.Yes | QtGui.QMessageBox.No,QtGui.QMessageBox.No)
            if reply == QtGui.QMessageBox.Yes :
                try :
                    shutil.rmtree(anDir.encode(self.fsCoding))
                    log(1, "Remove analys directory %s" % anDir)
                except Exception as e :
                    QMessageBox.information(self,"Error", "Could not erase all or a part of the analysis directory : \n\n %s\n\n\ñ%s" % (anDir, e))
                    log(1, "Could not erase all or a part of the analysis directory : %s. Caught this exception :\n %s" % (anDir,e))


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
        if not os.path.exists((u"%s/analysis/"%self.dir).encode(self.fsCoding)):
            os.mkdir((u"%s/analysis/"%self.dir).encode(self.fsCoding))

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
            # on ne fait rien si on a pas d'exécutable
            executablePath = self.parent.preferences_win.getExecutablePath()
            if executablePath == "":
                return
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
            the_frame.findChild(QPushButton,"analysisEdButton").setDisabled(True)
            the_frame.findChild(QPushButton,"analysisButton").setText("Running")
            the_frame.findChild(QPushButton,"analysisButton").setStyleSheet("background-color: #6DD963")
            the_frame.findChild(QPushButton,"analysisStopButton").show()
            analysis.status = "running"

            log(1,"Launching analysis '%s'"%analysis.name)
            self.save()
            log(3,"Cleaning '%s_progress.txt' file"%analysis.name)
            if os.path.exists((u"%s/%s_progress.txt"%(self.dir,analysis.name)).encode(self.fsCoding)):
                os.remove((u"%s/%s_progress.txt"%(self.dir,analysis.name)).encode(self.fsCoding))
            self.launchAnalysisThread(analysis)
            # on la vire de la queue
            self.analysisQueue.remove(analysis)
            # on met à jour les queue numbers
            for frame in self.dicoFrameAnalysis.keys():
                anatmp = self.dicoFrameAnalysis[frame]
                if anatmp in self.analysisQueue:
                    index = self.analysisQueue.index(anatmp)
                    frame.findChild(QPushButton,"analysisButton").setText("Queued (%s)"%index)
                    frame.findChild(QPushButton,"analysisButton").setStyleSheet("background-color: #F4992B")

    def getAnalysisCmdArgsList(self, analysis, quoted=True):
        cmd_args_list = []
        signames = {
                "progress" : "analysisProgress",
                "log" : "analysisLog",
                "termSuccess" : "analysisTermSuccess",
                "termProblem" : "analysisProblem",
                "newOutput" : "newOutput"
                }
        executablePath = self.parent.preferences_win.getExecutablePath()
        nbMaxThread = self.parent.preferences_win.getMaxThreadNumber()
        particleLoopSize = str(self.parent.preferences_win.particleLoopSizeEdit.text())
        params = analysis.computationParameters
        if analysis.category == "estimate":
            option = "-e"
        elif analysis.category == "compare":
            option = "-c"
        elif analysis.category == "bias":
            option = "-b"
            particleLoopSize = int(params.split('d:')[1].split(';')[0])
        elif "confidence" in analysis.category:
            option = "-f"
            suboption = "t"
            if analysis.category == "confidence_posterior_global" :
                suboption = "c"
            elif analysis.category == "confidence_prior_global" :
                suboption = "b"
            particleLoopSize = int(params.split('%s:'%suboption)[1].split(';')[0])
        elif analysis.category == "modelChecking":
            option = "-j"
        elif analysis.category == "pre-ev":
            option = "-d"
            #progressfile = None
        cmd_args_list = [executablePath,"-p", "%s/"%self.dir, "%s"%option,
                '%s'%params.replace(u'\xb5','u'), "-i", '%s'%analysis.name,
                "-g" ,"%s"%particleLoopSize , "-m", "-t", "%s"%nbMaxThread]

        if not quoted :
            return cmd_args_list

        cmd_args_list_quoted = list(cmd_args_list)
        for i in range(len(cmd_args_list_quoted)):
            if ";" in cmd_args_list_quoted[i] or " " in cmd_args_list_quoted[i] or ":" in cmd_args_list_quoted[i]:
                cmd_args_list_quoted[i] = '"'+cmd_args_list_quoted[i]+'"'
        return cmd_args_list_quoted

    def launchAnalysisThread(self,analysis):
        outfile = "%s/%s.out"%(self.dir,analysis.category)
        progressfile = "%s/%s_progress.txt"%(self.dir,analysis.name)
        signames = {
                "progress" : "analysisProgress",
                "log" : "analysisLog",
                "termSuccess" : "analysisTermSuccess",
                "termProblem" : "analysisProblem",
                "newOutput" : "newOutput"
                }
        cmd_args_list = self.getAnalysisCmdArgsList(analysis, quoted=False)
        cmd_args_list_quoted = self.getAnalysisCmdArgsList(analysis, quoted=True)

        addLine("%s/command.txt"%self.dir,"Command launched for analysis '%s' : %s\n\n"%(analysis.name," ".join(cmd_args_list_quoted)))

        self.thAnalysis = LauncherThread(analysis.name,cmd_args_list,outfile_path=outfile,progressfile_path=progressfile,signalnames=signames)
        self.thAnalysis.analysis = analysis
        self.thAnalysis.connect(self.thAnalysis,SIGNAL("analysisProgress(QString)"),self.analysisProgress)
        self.thAnalysis.connect(self.thAnalysis,SIGNAL("analysisTermSuccess()"),self.analysisTermSuccess)
        self.thAnalysis.connect(self.thAnalysis,SIGNAL("analysisProblem(QString)"),self.analysisProblem)
        self.thAnalysis.connect(self.thAnalysis,SIGNAL("analysisLog(int,QString)"),self.analysisLog)
        self.thAnalysis.start()

    def analysisProblem(self,msg):
        msg_to_show = msg
        msg_to_show = unicode(msg_to_show)
        msg_to_show = msg_to_show.encode('iso-8859-1')
        output.notify(self,"analysis problem","Something happened during the analysis %s : %s"%(self.thAnalysis.analysis.name,msg_to_show))

        # nettoyage du progress.txt
        aid = self.thAnalysis.analysis.name
        self.thAnalysis.analysis.status = "new"
        if os.path.exists((u"%s/%s_progress.txt"%(self.dir,aid)).encode(self.fsCoding)):
            os.remove((u"%s/%s_progress.txt"%(self.dir,aid)).encode(self.fsCoding))

        frame = None
        for fr in self.dicoFrameAnalysis.keys():
            if self.dicoFrameAnalysis[fr] == self.thAnalysis.analysis:
                frame = fr
                break
        frame.findChild(QPushButton,"analysisEdButton").setDisabled(False)
        frame.findChild(QPushButton,"analysisButton").setText("Re-launch")
        frame.findChild(QPushButton,"analysisButton").setStyleSheet("background-color: #EFB1B3")
        frame.findChild(QPushButton,"analysisStopButton").hide()
        frame.findChild(QProgressBar,"analysisStatusBar").setValue(0)
        self.thAnalysis.killProcess()
        self.thAnalysis = None

        self.nextAnalysisInQueue()

    def analysisLog(self,lvl,msg):
        """ log pour le thread analysis
        """
        if self.thAnalysis != None:
            log(lvl,msg)

    def analysisTermSuccess(self):
        frame = None
        for fr in self.dicoFrameAnalysis.keys():
            if self.dicoFrameAnalysis[fr] == self.thAnalysis.analysis:
                frame = fr
                break
        frame.findChild(QProgressBar,"analysisStatusBar").setValue(100)

        log(3,"Terminating analysis '%s' because the program ended successfully"%self.thAnalysis.analysis.name)
        frame.findChild(QPushButton,"analysisButton").setText("View results")
        frame.findChild(QPushButton,"analysisButton").setStyleSheet("background-color: #79D8FF")
        frame.findChild(QPushButton,"analysisStopButton").hide()
        self.terminateAnalysis()

    def analysisProgress(self,progstr):
        """ met à jour l'indicateur de progression de l'analyse en cours
        """
        proglines = progstr.split('\n')
        if len(proglines) > 0:
            fl = str(proglines[0])
            fl = fl.strip()
            t1 = float(fl.split()[0])
            t2 = float(fl.split()[-1].strip())
            if t2 != 0:
                prog = (t1*100/t2)
            else:
                return
            frame = None
            for fr in self.dicoFrameAnalysis.keys():
                if self.dicoFrameAnalysis[fr] == self.thAnalysis.analysis:
                    frame = fr
                    break
            if prog < 1 and prog > 0:
                prog = 1
            frame.findChild(QProgressBar,"analysisStatusBar").setValue(int(prog))
            log(3,"Progress line : '%s', new state : %s"%(fl,prog))

            # TODO peut etre à enlever car on a le signal termsuccess
            #if prog >= 100.0:
            #    log(3,"Terminating analysis because progression indicated 100 %% (%s)"%prog)
            #    frame.findChild(QPushButton,"analysisButton").setText("View results")
            #    frame.findChild(QPushButton,"analysisButton").setStyleSheet("background-color: #79D8FF")
            #    frame.findChild(QPushButton,"analysisStopButton").hide()
            #    self.terminateAnalysis()     def analysisProgress(self,progstr):
        """ met à jour l'indicateur de progression de l'analyse en cours
        """

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
        # griser le bouton d'édition
        for fr in self.dicoFrameAnalysis.keys():
            if self.dicoFrameAnalysis[fr] == the_analysis:
                fr.findChild(QPushButton,"analysisEdButton").setDisabled(True)
                break
        # nettoyage du progress.txt
        if os.path.exists((u"%s/%s_progress.txt"%(self.dir,aid)).encode(self.fsCoding)):
            os.remove((u"%s/%s_progress.txt"%(self.dir,aid)).encode(self.fsCoding))

        if atype == "estimate":
            if os.path.exists((u"%s/%s_mmmq_composite.txt"%(self.dir,aid)).encode(self.fsCoding))\
            or os.path.exists((u"%s/%s_mmmq_scaled.txt"%(self.dir,aid)).encode(self.fsCoding))\
            or os.path.exists((u"%s/%s_mmmq_original.txt"%(self.dir,aid)).encode(self.fsCoding)):
                # deplacement des fichiers de résultat
                aDirName = "%s_estimation"%aid
                if not os.path.exists((u"%s/analysis/%s"%(self.dir,aDirName)).encode(self.fsCoding)):
                    os.mkdir((u"%s/analysis/%s"%(self.dir,aDirName)).encode(self.fsCoding))
                if os.path.exists((u"%s/%s_paramstatdens_original.txt"%(self.dir,aid)).encode(self.fsCoding))\
                and os.path.exists((u"%s/%s_phistar_original.txt"%(self.dir,aid)).encode(self.fsCoding))\
                and os.path.exists((u"%s/%s_mmmq_original.txt"%(self.dir,aid)).encode(self.fsCoding)):
                    shutil.move((u"%s/%s_paramstatdens_original.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/paramstatdens_original.txt"%(self.dir,aDirName)).encode(self.fsCoding))
                    shutil.move((u"%s/%s_mmmq_original.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/mmmq_original.txt"%(self.dir,aDirName)).encode(self.fsCoding))
                    shutil.move((u"%s/%s_phistar_original.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/phistar_original.txt"%(self.dir,aDirName)).encode(self.fsCoding))
                if os.path.exists((u"%s/%s_paramstatdens_composite.txt"%(self.dir,aid)).encode(self.fsCoding))\
                and os.path.exists((u"%s/%s_phistar_composite.txt"%(self.dir,aid)).encode(self.fsCoding))\
                and os.path.exists((u"%s/%s_mmmq_composite.txt"%(self.dir,aid)).encode(self.fsCoding)):
                    shutil.move((u"%s/%s_paramstatdens_composite.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/paramstatdens_composite.txt"%(self.dir,aDirName)).encode(self.fsCoding))
                    shutil.move((u"%s/%s_mmmq_composite.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/mmmq_composite.txt"%(self.dir,aDirName)).encode(self.fsCoding))
                    shutil.move((u"%s/%s_phistar_composite.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/phistar_composite.txt"%(self.dir,aDirName)).encode(self.fsCoding))
                if os.path.exists((u"%s/%s_paramstatdens_scaled.txt"%(self.dir,aid)).encode(self.fsCoding))\
                and os.path.exists((u"%s/%s_phistar_scaled.txt"%(self.dir,aid)).encode(self.fsCoding))\
                and os.path.exists((u"%s/%s_mmmq_scaled.txt"%(self.dir,aid)).encode(self.fsCoding)):
                    shutil.move((u"%s/%s_paramstatdens_scaled.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/paramstatdens_scaled.txt"%(self.dir,aDirName)).encode(self.fsCoding))
                    shutil.move((u"%s/%s_mmmq_scaled.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/mmmq_scaled.txt"%(self.dir,aDirName)).encode(self.fsCoding))
                    shutil.move((u"%s/%s_phistar_scaled.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/phistar_scaled.txt"%(self.dir,aDirName)).encode(self.fsCoding))
            else:
                self.thAnalysis.problem = "No output files produced\n(%s\n%s\n%s)"%("%s/%s_mmmq_composite.txt"%(self.dir,aid),\
                        "%s/%s_mmmq_scaled.txt"%(self.dir,aid),"%s/%s_mmmq_original.txt"%(self.dir,aid))
                if os.path.exists((u"%s/estimate.out"%(self.dir)).encode(self.fsCoding)):
                    f = open((u"%s/estimate.out"%(self.dir)).encode(self.fsCoding),"rU")
                    lastLine = f.readlines()[-1]
                    f.close()
                    self.thAnalysis.problem += "\n%s"%lastLine
                self.thAnalysis.emit(SIGNAL("analysisProblem(QString)"),self.thAnalysis.problem)
                return
        elif atype == "compare":
            if (self.thAnalysis.analysis.logreg!=True and os.path.exists((u"%s/%s_compdirect.txt"%(self.dir,aid)).encode(self.fsCoding))) \
                or\
                (self.thAnalysis.analysis.logreg==True and os.path.exists((u"%s/%s_compdirect.txt"%(self.dir,aid)).encode(self.fsCoding)) and  os.path.exists((u"%s/%s_complogreg.txt"%(self.dir,aid)).encode(self.fsCoding))) :
                dd = datetime.now()
                date = "%s/%s/%s"%(dd.day,dd.month,dd.year)
                # deplacement des fichiers de résultat
                aDirName = "%s_comparison"%aid
                if not os.path.exists((u"%s/analysis/%s"%(self.dir,aDirName)).encode(self.fsCoding)):
                    os.mkdir((u"%s/analysis/%s"%(self.dir,aDirName)).encode(self.fsCoding))
                shutil.move((u"%s/%s_compdirect.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/compdirect.txt"%(self.dir,aDirName)).encode(self.fsCoding))
                g = open((u"%s/analysis/%s/compdirect.txt"%(self.dir,aDirName)).encode(self.fsCoding),'rU')
                datadir = g.readlines()
                g.close()
                if self.thAnalysis.analysis.logreg==True :
                    shutil.move((u"%s/%s_complogreg.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/complogreg.txt"%(self.dir,aDirName)).encode(self.fsCoding))
                    f = open((u"%s/analysis/%s/complogreg.txt"%(self.dir,aDirName)).encode(self.fsCoding),'r')
                    datareg = f.read()
                    f.close()
                fdaTextToDisplay = ""
                if self.thAnalysis.analysis.fda == '1' :
                    fdaTextToDisplay = "Summary statistics have been replaced by LDA component\n"
                textToDisplay = "\
                COMPARISON OF SCENARIOS\n\
                (%s)\n\n\
    Project directory : %s\n\
    Candidate scenarios : %s\n\
    Number of simulated data sets : %s\n\
    %s\
    \n\n Direct approach\n\n"%(date,self.dir,the_analysis.candidateScList,self.ui.nbSetsDoneEdit.text(),fdaTextToDisplay)
                textDirect = datadir[0].replace("   n   ","closest")
                i = 10
                while i < len(datadir):
                    textDirect += datadir[i]
                    i+=10
                textToDisplay += textDirect
                if  self.thAnalysis.analysis.logreg==True :
                    textToDisplay += "\n\n Logistic approach\n\n"
                    textToDisplay += datareg
                dest = open((u"%s/analysis/%s/compdirlog.txt"%(self.dir,aDirName)).encode(self.fsCoding),'w')
                dest.write(textToDisplay)
                dest.close()
            else:
                self.thAnalysis.problem = "Missing output file(s)\n"
                if os.path.exists((u"%s/%s_compdirect.txt"%(self.dir,aid)).encode(self.fsCoding)) :
                    self.thAnalysis.problem += "%s/%s_compdirect.txt found : OK\n"%(self.dir,aid)
                else :
                    self.thAnalysis.problem += "%s/%s_compdirect.txt NOT FOUND : ERROR\n"%(self.dir,aid)
                if self.thAnalysis.analysis.logreg==True :
                    if os.path.exists((u"%s/%s_complogreg.txt"%(self.dir,aid)).encode(self.fsCoding)) :
                        self.thAnalysis.problem += "%s/%s_complogreg.txt found : OK\n"%(self.dir,aid)
                    else :
                        self.thAnalysis.problem += "%s/%s_complogreg.txt not found : ERROR\n"%(self.dir,aid)
                if os.path.exists((u"%s/compare.out"%(self.dir)).encode(self.fsCoding)):
                    f = open((u"%s/compare.out"%(self.dir)).encode(self.fsCoding),"rU")
                    lastLine = f.readlines()[-1]
                    f.close()
                    self.thAnalysis.problem += "\nThis is the last line of the output file %s/compare.out :\n%s"%(self.dir,lastLine)
                self.thAnalysis.emit(SIGNAL("analysisProblem(QString)"),self.thAnalysis.problem)
                return
        elif atype == "bias":
            if os.path.exists((u"%s/%s_bias_original.txt"%(self.dir,aid)).encode(self.fsCoding))\
            or os.path.exists((u"%s/%s_bias_scaled.txt"%(self.dir,aid)).encode(self.fsCoding))\
            or os.path.exists((u"%s/%s_bias_composite.txt"%(self.dir,aid)).encode(self.fsCoding)):
                log(3,(u"File %s/%s_bias_original.txt exists"%(self.dir,aid)).encode(self.fsCoding))
                # deplacement des fichiers de résultat
                aDirName = "%s_bias"%aid
                if not os.path.exists((u"%s/analysis/%s"%(self.dir,aDirName)).encode(self.fsCoding)):
                    os.mkdir((u"%s/analysis/%s"%(self.dir,aDirName)).encode(self.fsCoding))
                if os.path.exists((u"%s/%s_bias_original.txt"%(self.dir,aid)).encode(self.fsCoding)):
                    shutil.move((u"%s/%s_bias_original.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/bias_original.txt"%(self.dir,aDirName)).encode(self.fsCoding))
                if os.path.exists((u"%s/%s_bias_composite.txt"%(self.dir,aid)).encode(self.fsCoding)):
                    shutil.move((u"%s/%s_bias_composite.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/bias_composite.txt"%(self.dir,aDirName)).encode(self.fsCoding))
                if os.path.exists((u"%s/%s_bias_scaled.txt"%(self.dir,aid)).encode(self.fsCoding)):
                    shutil.move((u"%s/%s_bias_scaled.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/bias_scaled.txt"%(self.dir,aDirName)).encode(self.fsCoding))
                log(3,"Copy of '%s/%s_bias.txt' to '%s/analysis/%s/bias_original.txt' done"%(self.dir,aid,self.dir,aDirName))
            else:
                self.thAnalysis.problem = "No output files produced\n"
                if os.path.exists((u"%s/bias.out"%(self.dir)).encode(self.fsCoding)):
                    f = open((u"%s/bias.out"%(self.dir)).encode(self.fsCoding),"rU")
                    lastLine = f.readlines()[-1]
                    f.close()
                    self.thAnalysis.problem += "\n%s"%lastLine
                self.thAnalysis.emit(SIGNAL("analysisProblem(QString)"),self.thAnalysis.problem)
                log(3,"File %s/%s_bias.txt doesn't exist. Results cannot be moved"%(self.dir,aid))
                return
        elif "confidence" in atype :
            if os.path.exists((u"%s/%s_confidence.txt"%(self.dir,aid)).encode(self.fsCoding)):
                # deplacement des fichiers de résultat
                aDirName = "%s_confidence"%aid
                if not os.path.exists((u"%s/analysis/%s"%(self.dir,aDirName)).encode(self.fsCoding)):
                    os.mkdir((u"%s/analysis/%s"%(self.dir,aDirName)).encode(self.fsCoding))
                shutil.move((u"%s/%s_confidence.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/confidence.txt"%(self.dir,aDirName)).encode(self.fsCoding))
            else:
                self.thAnalysis.problem = "No output files produced\n"
                if os.path.exists((u"%s/confidence.out"%(self.dir)).encode(self.fsCoding)):
                    f = open((u"%s/confidence.out"%(self.dir)).encode(self.fsCoding),"rU")
                    lastLine = f.readlines()[-1]
                    f.close()
                    self.thAnalysis.problem += "\n%s"%lastLine
                self.thAnalysis.emit(SIGNAL("analysisProblem(QString)"),self.thAnalysis.problem)
                return
        elif atype == "modelChecking":
            if os.path.exists((u"%s/%s_mcACP.txt"%(self.dir,aid)).encode(self.fsCoding)) or os.path.exists((u"%s/%s_mclocate.txt"%(self.dir,aid)).encode(self.fsCoding)):
                # deplacement des fichiers de résultat
                aDirName = "%s_modelChecking"%aid
                if not os.path.exists((u"%s/analysis/%s"%(self.dir,aDirName)).encode(self.fsCoding)):
                    os.mkdir((u"%s/analysis/%s"%(self.dir,aDirName)).encode(self.fsCoding))
                if os.path.exists((u"%s/%s_mclocate.txt"%(self.dir,aid)).encode(self.fsCoding)):
                    shutil.move((u"%s/%s_mclocate.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/mclocate.txt"%(self.dir,aDirName)).encode(self.fsCoding))
                if os.path.exists((u"%s/%s_mcACP.txt"%(self.dir,aid)).encode(self.fsCoding)):
                    shutil.move((u"%s/%s_mcACP.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/mcACP.txt"%(self.dir,aDirName)).encode(self.fsCoding))
            else:
                self.thAnalysis.problem = "No output files produced\n"
                if os.path.exists((u"%s/modelChecking.out"%(self.dir)).encode(self.fsCoding)):
                    f = open((u"%s/modelChecking.out"%(self.dir)).encode(self.fsCoding),"rU")
                    lastLine = f.readlines()[-1]
                    f.close()
                    self.thAnalysis.problem += "\n%s"%lastLine
                self.thAnalysis.emit(SIGNAL("analysisProblem(QString)"),self.thAnalysis.problem)
                return
        elif atype == "pre-ev":
            if os.path.exists((u"%s/%s_locate.txt"%(self.dir,aid)).encode(self.fsCoding)) or os.path.exists((u"%s/%s_ACP.txt"%(self.dir,aid)).encode(self.fsCoding)):
                # deplacement des fichiers de résultat
                aDirName = "%s_pca"%aid
                if not os.path.exists((u"%s/analysis/%s"%(self.dir,aDirName)).encode(self.fsCoding)):
                    os.mkdir((u"%s/analysis/%s"%(self.dir,aDirName)).encode(self.fsCoding))
                if os.path.exists((u"%s/%s_locate.txt"%(self.dir,aid)).encode(self.fsCoding)):
                    shutil.move((u"%s/%s_locate.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/locate.txt"%(self.dir,aDirName)).encode(self.fsCoding))
                if os.path.exists((u"%s/%s_ACP.txt"%(self.dir,aid)).encode(self.fsCoding)):
                    shutil.move((u"%s/%s_ACP.txt"%(self.dir,aid)).encode(self.fsCoding),(u"%s/analysis/%s/ACP.txt"%(self.dir,aDirName)).encode(self.fsCoding))
            else:
                self.thAnalysis.problem = "No output files produced\n"
                if os.path.exists((u"%s/pre-ev.out"%(self.dir)).encode(self.fsCoding)):
                    f = open((u"%s/pre-ev.out"%(self.dir)).encode(self.fsCoding),"rU")
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

    def putRefTableSize(self):
        """ met à jour l'affichage de la taille de la reftable
            verrouille les modèles hist et gen si une reftable existe
            actualise le warning sur le nombre min de data sets a simuler
        """
        size = self.readRefTableSize()
        if (size != None) and (size != 0):
            self.ui.nbSetsDoneEdit.setText("%s"%size)
            self.freezeHistModel()
            self.freezeGenData()
            self.ui.newAnButton.setDisabled(False)
            minSizeForAnalysis = len(self.hist_model_win.scList) * self.MIN_NUM_DATA_SETS_BY_SCENARIO_FOR_ANALYSIS
            if size < minSizeForAnalysis :
                self.ui.warningNumTextBrowser.show()
            else :
                self.ui.warningNumTextBrowser.hide()

        else:
            # si on n'a pas de reftable, on empêche la définition d'analyse
            self.ui.nbSetsDoneEdit.setText("0")
            self.freezeHistModel(False)
            self.freezeGenData(False)
            self.ui.newAnButton.setDisabled(True)
            self.ui.warningNumTextBrowser.show()


    def freezeHistModel(self,yesno=True):
        """ empêche la modification du modèle historique tout en laissant
        la possibilité de le consulter
        """
        un=""
        if not yesno: un="un"
        log(2,"%sfreezing Historical Model"%un)
        self.hist_model_win.ui.clearButton.setDisabled(yesno)
        self.hist_model_win.ui.exitButton.setDisabled(yesno)
        for e in self.hist_model_win.findChildren(QLineEdit):
            e.setReadOnly(yesno)
        for e in self.hist_model_win.findChildren(QRadioButton):
            e.setDisabled(yesno)
        for e in self.hist_model_win.findChildren(QPushButton,"rmScButton"):
            e.setDisabled(yesno)
        for e in self.hist_model_win.findChildren(QPushButton,"rmCondButton"):
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
            return diyabcUtils.readRefTableSize(reftablefile)
        except Exception as e:
            return 0

    def readNbRecordsOfScenario(self,numSc):
        """ lit la table de référence binaire pour en extraire la taille et la retourner
        """
        reftablefile = "%s/reftable.bin"%self.dir
        return diyabcUtils.readNbRecordsOfScenario(reftablefile,numSc)

    def saveAnalysis(self):
        """ sauvegarde la liste des analyses dans le dossier du projet
        """
        log(2,"Saving analysis into %s"%self.parent.analysis_conf_name)
        l_to_save = []
        for a in self.analysisList:
            l_to_save.append(a)
        f=open((u"%s/%s"%(self.dir,self.parent.analysis_conf_name)).encode(self.fsCoding),"wb")
        pickle.dump(l_to_save,f)
        f.close()

    def loadAnalysis(self):
        """ charge les analyses sauvegardées
        """
        log(2,"Loading analysis from %s"%self.parent.analysis_conf_name)
        if os.path.exists((u"%s/%s"%(self.dir,self.parent.analysis_conf_name)).encode(self.fsCoding)):
            f=open((u"%s/%s"%(self.dir,self.parent.analysis_conf_name)).encode(self.fsCoding),"rb")
            l = pickle.load(f)
            f.close()
            for a in l:
                if a.status == "running" or a.status == "queued":
                    a.status = "new"
                attr = dir(a)
                if "histParamsFixed" not in attr:
                    a.histParamsFixed = None
                if "histParamsDrawn" not in attr:
                    a.histParamsDrawn = None
                if "mutationModelFixed" not in attr:
                    a.mutationModelFixed = ""
                if "mutationModelDrawn" not in attr:
                    a.mutationModelDrawn = ""
                if "sumStatsTHDico" not in attr:
                    a.sumStatsTHDico = {}
                if "sumStatsConfDico" not in attr:
                    a.sumStatsConfDico = {}
                if "logreg" not in attr:
                    a.logreg = None
                self.addAnalysis(a)

    def lock(self):
        """ crée le fichier de verrouillage pour empêcher l'ouverture
        du projet par une autre instance de DIYABC
        """
        log(2,"Locking the project '%s'"%self.dir)
        f = open((u"%s/.DIYABC_lock"%self.dir).encode(self.fsCoding),"w")
        f.write("%s"%os.getpid())
        f.close()

    def unlock(self):
        """ supprime le verrouillage sur le projet ssi le verrouillage
        a été effectué par notre instance de DIYABC
        """
        log(2,"Unlocking the project '%s'"%self.dir)
        if os.path.exists((u"%s/.DIYABC_lock"%self.dir).encode(self.fsCoding)):
            f = open((u"%s/.DIYABC_lock"%self.dir).encode(self.fsCoding))
            pid = f.read()
            f.close()
            # on ne deverrouille que si c'est nous qui avons verrouillé
            if pid == str(os.getpid()):
                os.remove((u"%s/.DIYABC_lock"%self.dir).encode(self.fsCoding))


