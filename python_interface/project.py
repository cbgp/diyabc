# -*- coding: utf-8 -*-

import time
import os
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from project import *
from project_ui import *
from setHistFrame import SetHistoricalModel
from setGenData import SetGeneticData
from data import Data
from datetime import datetime 
import os.path

class Project(QTabWidget):
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

        # utile seulement si on derive de QTabWidget
        super(Project,self).__init__(parent)
        # on peut aussi instancier un tabwidget
        #self.tab = QTabWidget()

        # instanciation du widget project_ui
        self.ui = Ui_TabWidget()
        self.ui.setupUi(self)

        # desactivation des boutons
        self.ui.setHistoricalButton.setDisabled(True)
        self.ui.setGeneticButton.setDisabled(True)
        self.ui.runButton.setDisabled(True)
        self.ui.browseDirButton.setDisabled(True)
        self.ui.dataFileEdit.setReadOnly(True)
        self.ui.dirEdit.setReadOnly(True)
        self.ui.groupBox.hide()
        #self.ui.groupBox.setVisible(False)

        # creation des onglets "set ..."
        self.hist_model_win = SetHistoricalModel(self)
        self.hist_model_win.hide()
        #self.addTab(self.hist_model_win,"Set historical model")
        #self.setTabEnabled(2,False)

        self.gen_data_win = SetGeneticData(self)
        self.gen_data_win.hide()
        #self.addTab(self.gen_data_win,"Set genetic data")
        #self.setTabEnabled(3,False)


        # manual alignment set
        #self.ui.verticalLayout.setAlignment(self.ui.horizontalFrame,Qt.AlignTop)
        #self.ui.verticalLayout.setAlignment(self.ui.groupBox,Qt.AlignTop)
        self.ui.verticalLayout_2.setAlignment(self.ui.newAnButton,Qt.AlignCenter)
        self.ui.verticalLayout_3.setAlignment(self.ui.progressBar,Qt.AlignCenter)
        self.ui.projNameLabelValue.setText(name)
        self.ui.tableWidget.setColumnWidth(1,150)
        self.ui.tableWidget.setColumnWidth(2,300)
        self.ui.tableWidget.setColumnWidth(3,70)

        QObject.connect(self.ui.newAnButton,SIGNAL("clicked()"),self.addRow)
        QObject.connect(self.ui.tableWidget,SIGNAL("cellClicked(int,int)"),self.clcl)
        QObject.connect(self.ui.setHistoricalButton,SIGNAL("clicked()"),self.setHistorical)
        QObject.connect(self.ui.setGeneticButton,SIGNAL("clicked()"),self.setGenetic)
        QObject.connect(self.ui.browseDataFileButton,SIGNAL("clicked()"),self.dataFileSelection)
        QObject.connect(self.ui.browseDirButton,SIGNAL("clicked()"),self.dirSelection)

        # inserer image
        self.ui.setHistoricalButton.setIcon(QIcon("docs/redcross.png"))
        self.ui.setGeneticButton.setIcon(QIcon("docs/redcross.png"))

        self.setTabIcon(0,QIcon("docs/redcross.png"))
        self.setTabIcon(1,QIcon("/usr/share/pixmaps/baobab.xpm"))


        #for i in range(self.ui.tableWidget.columnCount()):
        #    for j in range(self.ui.tableWidget.rowCount()):
        #        it = QTableWidgetItem("%i,%i"%(j,i))
        #        self.ui.tableWidget.setItem(j,i,it)

        self.connect(self.ui.startButton, SIGNAL("clicked()"),self,SLOT("on_btnStart_clicked()"))
        self.connect(self.ui.cancelButton, SIGNAL("clicked()"),self.cancelTh)

    @pyqtSignature("")
    def on_btnStart_clicked(self):
        """Start the treatment in the thread"""
        self.th = MyThread(self)
        self.th.connect(self.th,SIGNAL("increment"),
                              self.incProgress)
        self.ui.progressBar.connect (self, SIGNAL("canceled()"),self.th,SLOT("cancel()"))
        self.th.start()
 
    def incProgress(self):
        """Increment the progress dialog"""
        self.ui.progressBar.setValue((self.ui.progressBar.value()+1)%100)

    def cancelTh(self):
        print 'plop'
        self.emit(SIGNAL("canceled()"))


    def dataFileSelection(self):
        """ dialog pour selectionner le fichier à lire
        il est lu et vérifié. S'il est invalide, on garde la sélection précédente
        """
        qfd = QFileDialog()
        qfd.setDirectory(self.ui.dirEdit.text())
        name = qfd.getOpenFileName()
        self.loadDataFile(name)

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
            QMessageBox.information(self,"Data file error","%s%s"%(e,keep))


    def dirSelection(self):
        """ selection du repertoire pour un nouveau projet et copie du fichier de données
        """
        qfd = QFileDialog()
        #qfd.setDirectory("~/")
        name = qfd.getExistingDirectory()
        if name != "":
            # name_YYYY_MM_DD-num le plus elevé
            dd = datetime.now()
            #num = 36
            cd = 100
            while cd > 0 and not os.path.exists(name+"/%s_%i_%i_%i-%i"%(self.name,dd.year,dd.month,dd.day,cd)):
                cd -= 1
            if cd == 100:
                QMessageBox.information(self,"Error","With this version, you cannot have more than 100 \
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
                except OSError,e:
                    QMessageBox.information(self,"Error",str(e))


    def changeIcon(self):
        l=["ok.png","redcross.png"]
        self.hist_state = (self.hist_state+1)% len(l)
        self.ui.setHistoricalButton.setIcon(QIcon("docs/"+l[self.hist_state%len(l)]))
        self.ui.setGeneticButton.setIcon(QIcon("docs/"+l[self.hist_state%len(l)]))
        self.ui.setSummaryButton.setIcon(QIcon("docs/"+l[self.hist_state%len(l)]))
        self.setTabIcon(0,QIcon("docs/"+l[self.hist_state%len(l)]))

        self.ui.progressBar.setValue((self.ui.progressBar.value()+10)%self.ui.progressBar.maximum())

    def clcl(self,i,j):
        print str(i)+","+str(j)
        #self.ui.tableWidget.item(i,j).setFlags(Qt.ItemIsEditable)
        self.ui.tableWidget.hideRow(i)
    def addRow(self):
        print self.ui.tableWidget.rowCount()
        self.ui.tableWidget.insertRow(self.ui.tableWidget.rowCount())
        print self.ui.tableWidget.rowCount()
        for i in range(self.ui.tableWidget.columnCount()):
            if i == 5:
                self.ui.tableWidget.setCellWidget(self.ui.tableWidget.rowCount()-1,i,QPushButton("View"))
            else:
                self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,i,QTableWidgetItem("new"+str(i)))
                print str(self.ui.tableWidget.rowCount()-1)+","+str(i)
            #self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,i).setText("new")
        #self.ui.tableWidget.insertRow(2)

    def setHistorical(self):
        """ passe sur l'onglet correspondant
        """
        self.addTab(self.hist_model_win,"Set historical model")

        self.setTabEnabled(0,False)
        self.setTabEnabled(1,False)
        # nécéssaire seulement sous windows
        # on dirait que sous linux, le simple setCurrentWidget rend l'onglet enabled
        self.setTabEnabled(self.count()-1,True)
        self.setCurrentWidget(self.hist_model_win)
        self.setHistValid(False)

    def setGenetic(self):
        """ passe sur l'onglet correspondant
        """
        self.setTabEnabled(0,False)
        self.setTabEnabled(1,False)
        self.addTab(self.gen_data_win,"Set genetic data")
        self.setCurrentWidget(self.gen_data_win)
        self.setGenValid(False)

        if self.gen_data_win.nbLocusGui == 0:
            self.gen_data_win.fillLocusTableFromData()

    def setNbScenarios(self,nb):
        self.ui.nbScLabel.setText(nb)
    def setNbParams(self,nb):
        self.ui.nbParamLabel.setText(nb)
    def clearHistoricalModel(self):
        self.removeTab(self.indexOf(self.hist_model_win))
        self.hist_model_win = SetHistoricalModel(self)
        self.insertTab(2,self.hist_model_win,"Set historical model")
        #self.setTabEnabled(self.indexOf(self.hist_model_win),False)
        self.setCurrentWidget(self.hist_model_win)


    def loadFromDir(self):
        """ charge les infos à partir du répertoire self.dir
        """
        # GUI
        self.ui.dirEdit.setText(self.dir)
        self.ui.browseDataFileButton.setDisabled(True)
        self.ui.groupBox.show()
        self.ui.setHistoricalButton.setDisabled(False)
        self.ui.setGeneticButton.setDisabled(False)

        # lecture du meta project
        if self.loadMyConf():
            # lecture de conf.hist.tmp
            self.hist_model_win.loadHistoricalConf()
            self.gen_data_win.loadGeneticConf()
        else:
            QMessageBox.information(self,"Load error","Impossible to read the project configuration")

    def loadMyConf(self):
        """ lit le fichier conf.tmp pour charger le fichier de données
        """
        if os.path.exists(self.ui.dirEdit.text()+"/conf.tmp"):
            f = open("%s/conf.tmp"%(self.dir),"r")
            lines = f.readlines()
            self.dataFileName = lines[0].strip()
            self.ui.dataFileEdit.setText(lines[0].strip())
            # lecture du dataFile pour les infos de Gui Projet
            self.loadDataFile("%s/%s"%(self.dir,lines[0].strip()))
            return True
        return False



    def save(self):

        # save meta project
        if os.path.exists(self.ui.dirEdit.text()+"/conf.tmp"):
            os.remove("%s/conf.tmp" % self.ui.dirEdit.text())

        f = codecs.open(self.ui.dirEdit.text()+"/conf.tmp",'w',"utf-8")
        f.write("%s\n"%self.dataFileName)
        # recup du nombre de params (depuis historical model et les mutational qui varient)
        nb_param = self.hist_model_win.getNbParam()
        nb_param += self.gen_data_win.getNbParam()
        nb_sum_stats = self.gen_data_win.getNbSumStats()
        f.write("%s parameters and %s summary statisticsi\n\n"%(nb_param,nb_sum_stats))
        f.close()

        # save hist conf
        self.hist_model_win.writeHistoricalConfFromGui()
        # save gen conf
        self.gen_data_win.writeGeneticConfFromGui()

    def setHistValid(self,valid):
        self.hist_state_valid = valid
        self.verifyRefTableValid()
        if valid:
            self.ui.setHistoricalButton.setIcon(QIcon("docs/ok.png"))
        else:
            self.ui.setHistoricalButton.setIcon(QIcon("docs/redcross.png"))

    def setGenValid(self,valid):
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
        else:
            self.setTabIcon(0,QIcon("docs/redcross.png"))


class MyThread(QThread):
    """ thread de traitement qui met à jour la progressBar
    """
    def __init__(self,parent):
        super(MyThread,self).__init__(parent)
        self.cancel = False

    def run (self):
        for i in range(1000):
            if self.cancel: break
            time.sleep(0.01)
            self.emit(SIGNAL("increment"))
            #print "%d "%(i),

    @pyqtSignature("")
    def cancel(self):
        """SLOT to cancel treatment"""
        self.cancel = True

