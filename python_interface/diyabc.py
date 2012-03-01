#!/usr/bin/python
# -*- coding: utf-8 -*-

## @package python_interface.diyabc
# @author Julien Veyssier
#
# @brief Contient la classe principale et la fonction main

# variable qui sera changée par le générateur d'exécutable
VERSION='development version'
from datetime import datetime
VERSION_DATE='01/01/1970'

import shutil
import sys,traceback
import os
# les .app démarrent python avec '/' comme cwd
if "darwin" in sys.platform and ".app/" in sys.argv[0]:
    # pour aller dans le rep où est le .app
    #mycwd = "/".join(sys.argv[0].split(".app/")[0].split('/')[:-1])
    # pour aller a l'interieur du .app
    mycwd = sys.argv[0].split(".app/")[0] + ".app/Contents/Resources/"
    os.chdir(mycwd)
# pour connaitre les modules, on manipule le pythonpath
sys.path.append("/".join(os.getcwd().split('/')[:-1]))
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from PyQt4 import uic
from preferences import Preferences
from showLogFile import ShowLogFile
from utils.cbgpUtils import Documentator,getLastRevisionDate
import output
import subprocess
from utils.cbgpUtils import cmdThread,logRotate,TeeLogger,log,DirNFileDialog
from utils.trayIconHandler import TrayIconHandler
from threading import Thread
from utils.data import isSNPDatafile
import dataPath

if VERSION=='development version':
    VERSION_DATE=getLastRevisionDate("../")
formDiyabc,baseDiyabc = uic.loadUiType("uis/diyabc.ui")

## @class Diyabc
# @brief Classe principale, fenêtre principale
class Diyabc(formDiyabc,baseDiyabc):
    """ Classe principale qui est aussi la fenêtre principale de la GUI
    """
    def __init__(self,app,parent=None,projects=None,logfile=None):
        super(Diyabc,self).__init__(parent)
        self.app = app
        self.logfile = logfile
        self.project_list = []
        self.recentList = []
        self.recentMenuEntries = []
        self.wtEnabled = True
        
        self.main_conf_name = "conf.tmp"
        self.hist_conf_name = "conf.hist.tmp"
        self.gen_conf_name = "conf.gen.tmp"
        self.analysis_conf_name = "conf.analysis"
        self.table_header_conf_name = "conf.th.tmp"
        self.reftableheader_name = "header.txt"
        self.ascertainment_conf_name = "conf.ascert.tmp"

        self.scenario_pix_dir_name = "scenario_pictures"
        self.scenario_pix_basename = "scenario"
        self.PCA_dir_name = "pictures"

        self.showLogFile_win = ShowLogFile(self)
        self.showLogFile_win.setWindowTitle("Logfile viewer")

        self.createWidgets()

        confdir = os.path.expanduser("~/.diyabc/")
        if not os.path.exists(confdir):
            os.mkdir(confdir)

        configFile = confdir+"config.cfg"
        self.preferences_win = Preferences(self,configFile)
        self.preferences_win.hide()
        self.preferences_win.loadPreferences()
        self.ui.stackedWidget.addWidget(self.preferences_win)

        self.setWindowIcon(QIcon(dataPath.DATAPATH+"/icons/coccicon.png"))

        self.illegalProjectNameCharacters = ['-',"'",'"','.',"/"]
        self.illegalAnalysisNameCharacters = ["'",'"','.',"/"]

        # ouverture des projets donnés en paramètre au lancement
        if projects != None and len(projects) > 0:
            for projDirName in projects:
                self.openProject(projDirName)
        try:
            self.documentator = Documentator(dataPath.DOCPATH+"/index.html")
            self.updateDoc()
        except Exception as e:
            log(1,"Documentation error : %s"%e)
            self.documentator = None

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        # pour le dragNdrop des dossier projet
        self.setAcceptDrops(True)

        pic = QPixmap(dataPath.DATAPATH+"/accueil_pictures/blue_orange.png")
        self.ui.imgLabel.setPixmap(pic)
        pic.scaled(100,100)
        self.switchToWelcomeStack()

        # about window
        self.aboutWindow = uic.loadUi("uis/about.ui")
        self.aboutWindow.parent = self
        self.aboutWindow.setWindowTitle('About DIYABC')
        ui = self.aboutWindow
        ui.logoLabel.setPixmap(QPixmap(dataPath.DATAPATH+"/icons/coccicon.png"))
        txt = str(self.aboutWindow.infoLabel.text())
        txt = txt.replace('vvv',VERSION).replace('ddd',VERSION_DATE)
        self.aboutWindow.infoLabel.setText(txt)
        QObject.connect(ui.okButton,SIGNAL("clicked()"),self.aboutWindow.close)

        self.ui.tab_6.hide()
        self.ui.tabWidget.removeTab(0)

        # gestion du menu
        file_menu = self.ui.menubar.addMenu("&File")
        self.file_menu = file_menu
        self.setStyleSheet("QMenu {\
                color: black;\
                background-color: white;\
                margin: 2px; /* some spacing around the menu */\
                }\
                \
                QMenu::item {\
                padding: 2px 25px 2px 20px;\
                border: 1px solid transparent; /* reserve space for selection border */\
                }\
                \
                QMenu::item:selected {\
                border-color: darkblue;\
                /*background: rgba(100, 100, 100, 150);*/\
                background-color: #FFD800;\
                }\
                \
                QMenu::item:disabled {\
                /*border-color: darkblue;*/\
                /*background: rgba(100, 100, 100, 150);*/\
                background-color: #ECECEC;\
                color: gray;\
                }\
                \
                QMenu::icon:checked { /* appearance of a 'checked' icon */\
                background: gray;\
                border: 1px inset gray;\
                position: absolute;\
                top: 1px;\
                right: 1px;\
                bottom: 1px;\
                left: 1px;\
                }\
                \
                QMenu::separator {\
                height: 2px;\
                background: lightblue;\
                margin-left: 10px;\
                margin-right: 5px;\
                }\
                \
                QMenu::indicator {\
                width: 13px;\
                height: 13px;\
                }")
        self.newMenu = file_menu.addMenu(QIcon(dataPath.DATAPATH+"/icons/folder-new.png"),"&New project")
        self.newMenu.addAction(QIcon(dataPath.DATAPATH+"/icons/gene.png"),"&Microsatellites and/or sequences",self.newProject,QKeySequence(Qt.CTRL + Qt.Key_M))
        self.newMenu.addAction(QIcon(dataPath.DATAPATH+"/icons/dna.png"),"&SNP",self.newProject,QKeySequence(Qt.CTRL + Qt.Key_N))
        file_menu.addAction(QIcon(dataPath.DATAPATH+"/icons/fileopen.png"),"&Open project",self.openProject,QKeySequence(Qt.CTRL + Qt.Key_O))
        self.recent_menu = file_menu.addMenu(QIcon(dataPath.DATAPATH+"/icons/document-open-recent.png"),"Open recent projects")
        self.recent_menu.setDisabled(True)
        self.saveAllProjActionMenu = file_menu.addAction(QIcon(dataPath.DATAPATH+"/icons/document-save-all.png"),"&Save all projects",self.saveAllProjects,QKeySequence(Qt.CTRL + Qt.Key_A))
        self.saveAllProjActionMenu.setDisabled(True)

        file_menu.addAction(QIcon(dataPath.DATAPATH+"/icons/redhat-system_settings.png"),"&Settings",self.switchToPreferences,QKeySequence(Qt.CTRL + Qt.Key_P))
        self.simulate_menu = file_menu.addMenu(QIcon(dataPath.DATAPATH+"/icons/mask.jpeg"),"&Simulate data set(s)")
        self.simulate_menu.addAction(QIcon(dataPath.DATAPATH+"/icons/gene.png"),"&Microsatellites and/or sequences (Genepop format)",self.simulateDataSets)
        self.simulate_menu.addAction(QIcon(dataPath.DATAPATH+"/icons/dna.png"),"&SNP (ad-hoc format)",self.simulateDataSets)

        action = file_menu.addAction(QIcon(dataPath.DATAPATH+"/icons/window-close.png"),"&Quit",self.close,QKeySequence(Qt.CTRL + Qt.Key_Q))
        #mettre plusieurs raccourcis claviers pour le meme menu
        #action.setShortcuts([QKeySequence(Qt.CTRL + Qt.Key_Q),QKeySequence(Qt.Key_Escape)])
        action.setShortcuts([QKeySequence(Qt.CTRL + Qt.Key_Q)])
        #sepAc = QAction("Open recent projects",self)
        #sepAc.setSeparator(True)
        #file_menu.addAction(sepAc)
        #style_menu = self.ui.menubar.addMenu("Style")
        #action_group = QActionGroup(style_menu)
        #for stxt in self.styles:
        #    self.style_actions[stxt] = style_menu.addAction(stxt,self.changeStyle)
        #    self.style_actions[stxt].setActionGroup(action_group)
        #    self.style_actions[stxt].setCheckable(True)
        navigate_menu = self.ui.menubar.addMenu("&Navigate")
        self.navigate_menu = navigate_menu
        self.navigateProjectActions = []
        self.prevProjectActionMenu = navigate_menu.addAction(QIcon(dataPath.DATAPATH+"/icons/arrow-up.png"),"&Previous project",self.prevProject,QKeySequence(Qt.CTRL + Qt.Key_PageUp))
        self.nextProjectActionMenu = navigate_menu.addAction(QIcon(dataPath.DATAPATH+"/icons/arrow-down.png"),"&Next project",self.nextProject,QKeySequence(Qt.CTRL + Qt.Key_PageDown))
        navigate_menu.addSeparator()
        self.nextProjectActionMenu.setDisabled(True)
        self.prevProjectActionMenu.setDisabled(True)
        help_menu = self.ui.menubar.addMenu("&Help")
        self.help_menu = help_menu
        help_menu.addAction(QIcon(dataPath.DATAPATH+"/icons/dialog-question.png"),"&About DIYABC",self.aboutWindow.show)
        help_menu.addAction(QIcon(dataPath.DATAPATH+"/icons/gnome-mime-text.png"),"&Show logfile",self.showLogFile,QKeySequence(Qt.CTRL + Qt.Key_L))

        self.currentProjectMenu = None
        QObject.connect(self.ui.tabWidget,SIGNAL("currentChanged(int)"),self.updateCurrentProjectMenu)
	
        QObject.connect(self.ui.tabWidget,SIGNAL("tabCloseRequested(int)"),self.closeProject)

        QObject.connect(self.ui.openProjectButton,SIGNAL("clicked()"),self.openProject)
        QObject.connect(self.ui.newMSSProjectButton,SIGNAL("clicked()"),self.newProject)
        QObject.connect(self.ui.newSNPProjectButton,SIGNAL("clicked()"),self.newProject)
        self.ui.versionLabel.setText('%s'%VERSION)

        # TOOLBAR
        newButton = QPushButton(QIcon(dataPath.DATAPATH+"/icons/folder-new.png"),"New MSS",self)
        newButton.setObjectName("newButton")
        newButton.setToolTip("New Microsatellites/Sequences project")
        newButton.setMaximumSize(QSize(85, 22))
        #newButton.setMinimumSize(QSize(16, 18))
        newButton.setFlat(True)
        QObject.connect(newButton,SIGNAL("clicked()"),self.newProject)
        self.ui.toolBar.addWidget(newButton)
        #self.ui.toolBar.addSeparator()

        newSNPButton = QPushButton(QIcon(dataPath.DATAPATH+"/icons/folder-new.png"),"New SNP",self)
        newSNPButton.setObjectName("newSNPButton")
        newSNPButton.setToolTip("New SNP project")
        newSNPButton.setMaximumSize(QSize(85, 22))
        #newSNPButton.setMinimumSize(QSize(16, 18))
        newSNPButton.setFlat(True)
        QObject.connect(newSNPButton,SIGNAL("clicked()"),self.newProject)
        self.ui.toolBar.addWidget(newSNPButton)
        #self.ui.toolBar.addSeparator()

        openButton = QPushButton(QIcon(dataPath.DATAPATH+"/icons/fileopen.png"),"Open",self)
        openButton.setObjectName("openButton")
        openButton.setToolTip("Open project")
        openButton.setMaximumSize(QSize(70, 22))
        #openButton.setMinimumSize(QSize(16, 18))
        openButton.setFlat(True)
        QObject.connect(openButton,SIGNAL("clicked()"),self.openProject)
        self.ui.toolBar.addWidget(openButton)
        #self.ui.toolBar.addSeparator()

        saveButton = QPushButton(QIcon(dataPath.DATAPATH+"/icons/document-save.png"),"Save",self)
        self.saveButton = saveButton
        saveButton.setObjectName("saveButton")
        saveButton.setToolTip("Save current project")
        saveButton.setMaximumSize(QSize(66, 22))
        #saveButton.setMinimumSize(QSize(16, 18))
        saveButton.setFlat(True)
        QObject.connect(saveButton,SIGNAL("clicked()"),self.saveCurrentProject)
        self.ui.toolBar.addWidget(saveButton)
        saveButton.setDisabled(True)
        #self.ui.toolBar.addSeparator()

        saveAllButton = QPushButton(QIcon(dataPath.DATAPATH+"/icons/document-save-all.png"),"Save all",self)
        self.saveAllButton = saveAllButton
        saveAllButton.setObjectName("saveAllButton")
        saveAllButton.setToolTip("Save all projects")
        saveAllButton.setMaximumSize(QSize(85, 22))
        #saveButton.setMinimumSize(QSize(16, 18))
        saveAllButton.setFlat(True)
        QObject.connect(saveAllButton,SIGNAL("clicked()"),self.saveAllProjects)
        self.ui.toolBar.addWidget(saveAllButton)
        saveAllButton.setDisabled(True)

        spacer = QWidget()
        spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.ui.toolBar.addWidget(spacer)

        wtButton = QPushButton(QIcon(dataPath.DATAPATH+"/icons/whats.png"),"What's this ?",self)
        wtButton.setDisabled(False)
        wtButton.setToolTip("Click on this button and then on another object to get the documentation")
        self.wtButton = wtButton
        wtButton.setMaximumSize(QSize(136, 22))
        #wtButton.setMinimumSize(QSize(16, 18))
        wtButton.setFlat(True)
        QObject.connect(wtButton,SIGNAL("clicked()"),self.enterWhatsThisMode)
        self.ui.toolBar.addWidget(wtButton)
        #wtButton.hide()

        for but in [newButton,newSNPButton,openButton,saveButton,saveAllButton,wtButton]:
            but.setStyleSheet("QPushButton:hover { background-color: #FFD800;  border-style: outset; border-width: 1px; border-color: black;border-style: outset; border-radius: 5px; } QPushButton:pressed { background-color: #EE1C17; border-style: inset;} ")

        self.systrayHandler = TrayIconHandler(dataPath.DATAPATH+"/icons/coccicon.png",dataPath.DATAPATH+"/icons/coccicon.gif",self.file_menu,self)

    def enterWhatsThisMode(self):
        """ Change le style du curseur de souris et attend un clic
        sur un objet pour afficher son "whatsThis"
        """
        QWhatsThis.enterWhatsThisMode()

    def updateDoc(self,obj=None):
        """ Met à jour le "what's this" pour tous les
        objets fils de obj documentés dans Documentator
        """
        if obj == None:
            obj = self
        log(3,"Updating documentation of "+"%s"%obj.objectName())
        l = []
        for typ in [QLabel,QPushButton,QPlainTextEdit,QListWidget,QLineEdit,QRadioButton,QComboBox,QProgressBar,QCheckBox]:
            l.extend(obj.findChildren(typ))
        for e in l:
            objnamestr = "%s\n\n"%e.objectName()
            objname_debug = self.preferences_win.debugWhatsThisCheck.isChecked()
            try:
                doc_dico = self.documentator.getDocHashByTags("%s"%e.objectName())
                # on remplace les SRC pour que les images soient bien chargées
                for tag in doc_dico.keys():
                    doc_dico[tag] = doc_dico[tag].replace('SRC="','SRC="%s/'%dataPath.DOCPATH)
                docstr = ""
                # on n'encadre pas le default tag
                if doc_dico.has_key("default_tag"):
                    docstr += "<table border='1'><tr><td>" +doc_dico["default_tag"]+"</td></tr></table>\n"
                # on encadre les textes pour chaque tag
                for tag in doc_dico.keys():
                    if tag != "default_tag":
                        docstr += "<table border='1'><tr><th><font color='orange'>%s</font>"%tag
                        docstr += "</th></tr><tr><td>" + doc_dico[tag]+"</td></tr></table>\n"

                if objname_debug:
                    e.setWhatsThis(output.whatsthis_header + objnamestr + "<br/><br/>" + docstr + output.whatsthis_footer)
                else:
                    e.setWhatsThis(output.whatsthis_header + docstr + output.whatsthis_footer)
                log(4,"Adding documentation of "+"%s"%e.objectName())
            except Exception as ex:
                if objname_debug:
                    e.setWhatsThis(output.whatsthis_header + objnamestr + output.whatsthis_footer)
                pass

    def toggleWt(self,yesno):
        """ toggle le méchanisme d'aide intégrée
        """
        self.wtEnabled = yesno
        if yesno:
            self.wtButton.setDisabled(False)
        else:
            self.wtButton.setDisabled(True)

    def setToolBarPosition(self,pos):
        self.removeToolBar(self.ui.toolBar)
        self.addToolBar(pos,self.ui.toolBar)
        self.ui.toolBar.show()

    def showLog(self,text):
        """ fonction appelée par le logger pour afficher le log dans la fenetre de log
        ne pas faire de print dans cette fonction
        """
        self.showLogFile_win.logText.moveCursor(QTextCursor.End)
        self.showLogFile_win.logText.moveCursor(QTextCursor.StartOfLine)
        self.showLogFile_win.logText.appendPlainText(text)

    def updateCurrentProjectMenu(self,projIndex):
        self.updateNavigateMenu()
        # si -1 : plus d'onglet
        if projIndex != -1:
            curprojname = self.tabWidget.currentWidget().name
            if self.currentProjectMenu != None:
                self.currentProjectMenu.setText("&Project %s"%curprojname)
            else:
                self.currentProjectMenu = self.ui.menubar.insertMenu(self.navigate_menu.menuAction(),QMenu("Project %s"%curprojname,self))
                self.saveProjActionMenu =   self.currentProjectMenu.menu().addAction(QIcon(dataPath.DATAPATH+"/icons/document-save.png"),"&Save project",self.saveCurrentProject,QKeySequence(Qt.CTRL + Qt.Key_S))
                self.cloneProjActionMenu =  self.currentProjectMenu.menu().addAction(QIcon(dataPath.DATAPATH+"/icons/tab-duplicate.png"),"&Clone project",self.cloneCurrentProject,QKeySequence(Qt.CTRL + Qt.Key_K))
                self.RngProjActionMenu =  self.currentProjectMenu.menu().addAction(QIcon(dataPath.DATAPATH+"/icons/random.png"),"Re&generate RNG",self.rngCurrentProject,QKeySequence(Qt.CTRL + Qt.Key_R))
                self.deleteReftableActionMenu =  self.currentProjectMenu.menu().addAction(QIcon(dataPath.DATAPATH+"/icons/user-trash.png"),"D&elete reftable",self.deleteReftableCurrentProject)
                self.closeProjActionMenu =  self.currentProjectMenu.menu().addAction(QIcon(dataPath.DATAPATH+"/icons/project-close.png"),"C&lose project",self.closeCurrentProject,QKeySequence(Qt.CTRL + Qt.Key_W))
                self.deleteProjActionMenu = self.currentProjectMenu.menu().addAction(QIcon(dataPath.DATAPATH+"/icons/user-trash.png"),"&Delete project",self.deleteCurrentProject,QKeySequence(Qt.CTRL + Qt.Key_X))
        else:
            # on supprime le menu
            self.menubar.removeAction(self.currentProjectMenu)
            self.currentProjectMenu = None

    def updateNavigateMenu(self):
        for ac in self.navigateProjectActions:
            self.navigate_menu.removeAction(ac)
        self.navigateProjectActions = []

        for i in range(self.ui.tabWidget.count()):
            name = self.ui.tabWidget.widget(i).name
            self.navigateProjectActions.append( self.navigate_menu.addAction(QIcon(dataPath.DATAPATH+"/icons/fileopen.png"),"Go to %s (%s)"%(name,i+1),self.goToProject) )
            if i == self.ui.tabWidget.currentIndex():
                self.navigateProjectActions[-1].setDisabled(True)

    def goToProject(self):
        ac=self.sender()
        num = int(ac.text().split('(')[1].split(')')[0])
        self.ui.tabWidget.setCurrentIndex(num-1)

    def simulateDataSets(self):
        from projectSimulation import ProjectSimulationSnp,ProjectSimulationMsatSeq
        fileDial = QtGui.QFileDialog(self,"Select name and location of the new simulated data set(s)")
        fileDial.setAcceptMode(QFileDialog.AcceptSave)
        fileDial.setLabelText(QtGui.QFileDialog.Accept,"Ok")
        if "darwin" in sys.platform:
            title = "Genericname"
        else:
            title = "Data file\ngeneric name"
        fileDial.setLabelText(QtGui.QFileDialog.FileName,title)
        ok = (fileDial.exec_() == 1)
        result = fileDial.selectedFiles()
        if len(result) > 0:
            path = result[0]
        path = "%s"%path
        # on enleve l'eventuel '/' de fin et on extrait le nom
        if path != None and len(path) > 0 and len(path.split('/')) > 0 :
            if path[-1] == "/":
                path = path[:-1]
            name = path.split("/")[-1]
            directory = os.path.dirname(path)

        if ok:
            if "Genepop" in self.sender().text():
                newSimProj = ProjectSimulationMsatSeq(name,directory,self)
            else:
                newSimProj = ProjectSimulationSnp(name,directory,self)
            # un nouveau projet a au moins un scenario
            newSimProj.hist_model_win.addSc()
            newSimProj.hist_model_win.hideRemoveScButtons()
            self.project_list.append(newSimProj)
            # si c'est le premier projet, on permet la fermeture par le menu
            # ajout au tabwidget de l'ui principale
            # ajout de l'onglet
            self.ui.tabWidget.addTab(newSimProj,"[SIM]%s"%newSimProj.name)
            self.ui.tabWidget.setCurrentWidget(newSimProj)

            if len(self.project_list) == 1:
                self.saveAllProjActionMenu.setDisabled(False)
                self.saveButton.setDisabled(False)
            if len(self.project_list) == 2:
                self.nextProjectActionMenu.setDisabled(False)
                self.prevProjectActionMenu.setDisabled(False)
                self.saveAllButton.setDisabled(False)
            self.switchToMainStack()
            log(1,"Simulation project '%s' successfully created"%(newSimProj.name))
            self.updateDoc(newSimProj)

    def setRecent(self,rlist):
        self.recentList = rlist
        self.redrawRecent()

    def redrawRecent(self):
        """ Met à jour le menu des projets récents
        supprime les recents qui n'existent plus
        """
        # cleaning
        for ac in self.recentMenuEntries:
            self.recent_menu.removeAction(ac)
        self.recentMenuEntries = []
        self.entryToRecent = {}
        # copy
        future_recent_list = []
        for recent in self.recentList:
            future_recent_list.append(recent)
        # drawing
        nbMaxRecent = self.preferences_win.getMaxRecentNumber()
        nb_added = 0
        for i,rec in enumerate(self.recentList):
            if os.path.exists(str(rec)):
                self.recentMenuEntries.append( self.recent_menu.addAction(QIcon(dataPath.DATAPATH+"/icons/document-open-recent.png"),rec.split('/')[-1],self.openRecent) )
                self.entryToRecent[ self.recentMenuEntries[-1] ] = rec
                nb_added += 1
            else:
                # le project n'existe plus, on supprime celui ci des recent
                log(3,"Recent project %s doesn't exist anymore"%str(rec))
                future_recent_list.remove(rec)
            # on ajoute au maximum N recent
            if nb_added == nbMaxRecent:
                break
        self.recentList = future_recent_list
        if len(self.recentList) > 0:
            self.recent_menu.setDisabled(False)
        else:
            self.recent_menu.setDisabled(True)

    def getRecent(self):
        return self.recentList

    def addRecent(self,rec):
        """ Ajoute un projet recent. Garde les N derniers récents. 
        Si le projet était déjà dans la liste, le met au sommet
        """
        # on vire les eventuelles occurences du projet que l'on veut ajouter
        while self.recentList.count(rec) > 0:
            self.recentList.remove(rec)

        nbMaxRecent = self.preferences_win.getMaxRecentNumber()
        self.recentList.insert(0,rec)
        while len(self.recentList) > nbMaxRecent:
            self.recentList.pop()
        self.redrawRecent()

        self.preferences_win.saveRecent()
        self.preferences_win.writeConfigFile()

    def openRecent(self):
        """ Evennement d'ouverture d'un projet récent depuis le menu
        """
        ac = self.sender()
        path = self.entryToRecent[ac]
        self.openProject(path)
        #self.showTrayMessage("plop","recent project opened \n%s"%path)

    def showLogFile(self):
        """ Ouverture de la fenetre d'affichage des logs
        """
        self.showLogFile_win.show()
        
    def showStatus(self,msg,time=None):
        """ Affiche 'msg' dans la statusbar pendant 'time'
        """
        if time!=None:
            self.ui.statusBar.showMessage(msg,time)
        else:
            self.ui.statusBar.showMessage(msg)
        QCoreApplication.processEvents()

    def clearStatus(self):
        self.ui.statusBar.clearMessage()

    def switchToMainStack(self):
        self.ui.stackedWidget.setCurrentIndex(0)

    def switchToWelcomeStack(self):
        self.ui.stackedWidget.setCurrentIndex(1)

    def switchToPreferences(self):
        self.ui.stackedWidget.setCurrentIndex(2)

    def isSNPProjectDir(self,dir):
        """ Renvoie vrai si le dossier dont le chemin (relatif ou absolu)
        est dir est un projet de type SNP
        """
        if os.path.exists(dir) and os.path.exists("%s/%s"%(dir,self.main_conf_name)):
            f=open("%s/%s"%(dir,self.main_conf_name),'r')
            cont = f.readlines()
            f.close()
            if len(cont)>0:
                return isSNPDatafile("%s/%s"%(dir,cont[0].strip()))
        return False

    def openProject(self,dir=None):
        """ ouverture d'un projet existant
        """
        from projectMsatSeq import ProjectMsatSeq
        from projectSnp import ProjectSnp
        if dir == None:
            qq = DirNFileDialog(None,"Select project file or directory")
            qq.setFilter("DIYABC project files (*.diyabcproject);;all files (*)")
            ret = qq.exec_()
            if ret:
                print qq.selectedFiles()[0]
                name = qq.selectedFiles()[0]
            else:
                name = ""
            if name != "" and name != None:
                # c'est un dossier
                if os.path.isdir(name):
                    dir = name
                # c'est un fichier
                else:
                    dir = '/'.join(str(name).split('/')[:-1])
                    if os.path.exists(dir) and os.path.exists("%s/%s"%(dir,self.main_conf_name)):
                        # le fichier est bien dans un projet
                        log(3,"%s is in a project directory, opening this project"%name)
                    else:
                        log(3,"%s is not in a project directory, nothing to open"%name)
            else:
                return

        # on enlève le dernier '/' s'il y en a un
        if dir != "" and dir[-1] == "/":
            dir = dir[:-1]
        log(1,"attempting to open the project : %s"%dir)
        self.showStatus("Loading project %s"%dir.split('/')[-1])
        QApplication.setOverrideCursor( Qt.WaitCursor )

        if len(str(dir).split('/')[-1].split('_')) > 4:
            project_name = "_".join(str(dir).split('/')[-1].split('_')[:-3])
        else:
            project_name = str(dir).split('/')[-1].split('_')[0]
        # si le dossier existe et qu'il contient conf.hist.tmp
        if dir != "":
            if os.path.exists(dir) and os.path.exists("%s/%s"%(dir,self.main_conf_name)):
                proj_name_list = []
                for p in self.project_list:
                    proj_name_list.append(p.name)
                if not project_name in proj_name_list:
                    proj_ready_to_be_opened = True
                    # si le projet est verrouillé
                    if os.path.exists("%s/.DIYABC_lock"%dir):
                        if not output.debug:
                            reply = QMessageBox.question(self,"Project is locked","The %s project is currently locked.\
    \nThere are two possible reasons for that : \
    \n- The project is currently opened in another instance of DIYABC\
    \n- The last time you opened this project, DIYABC was closed unregularly\n\
    \nWould you like to open the project anyway and get the lock with this DIYABC ?\
    \nIf you say YES, think about closing potential other DIYABCs which manipulate this project"%project_name,
                                    QtGui.QMessageBox.Yes | QtGui.QMessageBox.No,QtGui.QMessageBox.No)
                            if reply == QtGui.QMessageBox.Yes:
                                os.remove("%s/.DIYABC_lock"%dir)
                            else:
                                proj_ready_to_be_opened = False
                        else:
                            # si on est en mode debug, on vire le verrou sans sommation
                            os.remove("%s/.DIYABC_lock"%dir)
                    if proj_ready_to_be_opened:
                        # on selectionne le type de projet
                        if self.isSNPProjectDir(dir):
                            proj_to_open = ProjectSnp(project_name,dir,self)
                        else:
                            proj_to_open = ProjectMsatSeq(project_name,dir,self)
                        try:
                            proj_to_open.loadFromDir()
                        except Exception as e:
                            tb = traceback.format_exc()
                            self.clearStatus()
                            QApplication.restoreOverrideCursor()
                            output.notify(self,"opening error","An error occured during the opening of the project\
 %s :\n\nThe cause may be a dirty user handmade modification of the project files or a bug.\
 If it persists with clean project files, please submit a bug to developpers\n\n%s"%(dir,tb))
                            return
                        self.project_list.append(proj_to_open)
                        self.ui.tabWidget.addTab(proj_to_open,proj_to_open.name)
                        self.ui.tabWidget.setCurrentWidget(proj_to_open)
                        # verification generale pour mettre a jour l'etat du modèle historique et mutationnel
                        proj_to_open.checkAll()
                        # si la reftable existe, on affiche le nombre d'enregistrement de celle ci et on 
                        # verrouille (freeze) les modèles historique et génétique
                        proj_to_open.putRefTableSize()
                        # si c'est le premier projet, on permet la fermeture/del/save par le menu
                        if len(self.project_list) == 1:
                            self.saveAllProjActionMenu.setDisabled(False)
                            self.saveButton.setDisabled(False)
                        if len(self.project_list) == 2:
                            self.saveAllButton.setDisabled(False)
                            self.nextProjectActionMenu.setDisabled(False)
                            self.prevProjectActionMenu.setDisabled(False)
                        # on quitte la page d'accueil si on y était
                        self.switchToMainStack()
                        # creation du lock
                        proj_to_open.lock()
                        # si les RNG n'existent pas, on les crée
                        if not os.path.exists("%s/RNG_state_0000.bin"%dir):
                            proj_to_open.initializeRNG()
                        log(1,"Project '%s' opened successfully"%dir)
                        self.addRecent(dir)
                        self.updateDoc(proj_to_open)
                else:
                    output.notify(self,"Name error","A project named \"%s\" is already loaded"%project_name)
            else:
               output.notify(self,"Load error","\"%s\" is not a project directory"%dir)
        self.clearStatus()
        QApplication.restoreOverrideCursor()

    def rngCurrentProject(self):
        current_project = self.ui.tabWidget.currentWidget()
        current_project.initializeRNG()

    def cloneCurrentProject(self,cloneName=None,cloneDir=None):
        """ duplique un projet vers un autre répertoire
        demande le nom du clone puis le répertoire dans lequel mettre le clone du projet
        """ 
        self.saveCurrentProject()
        current_project = self.ui.tabWidget.currentWidget()
        log(1,"attempting to clone the project : %s"%current_project.dir)
        if not current_project.canBeCloned():
            output.notify(self,"Impossible to clone","Impossible to clone that kind of project")
            return
        ok = True
        if cloneName == None or cloneDir == None:
            fileDial = QtGui.QFileDialog(self,"Select location of the clone project")
            fileDial.setAcceptMode(QFileDialog.AcceptSave)
            fileDial.setLabelText(QtGui.QFileDialog.Accept,"Clone project")
            fileDial.setLabelText(QtGui.QFileDialog.FileName,"Clone name")
            ok = (fileDial.exec_() == 1)
            if not ok:
                return
            result = fileDial.selectedFiles()
            if len(result) > 0:
                path = result[0]
            path = "%s"%path
            # on enleve l'eventuel '/' de fin et on extrait le nom du projet
            if path != None and len(path) > 0 and len(path.split('/')) > 0 :
                if path[-1] == "/":
                    path = path[:-1]
                cloneName = path.split("/")[-1]
                cloneDir = os.path.dirname(path)
            else:
                # on a eu un probleme inconnu, on a un path qui n'a pas de '/'
                return

        if ok:
            if self.checkProjectName(cloneName):
                # on vire le nom à la fin du path pour obtenir le dossier du clone
                if cloneDir != "" and os.path.exists(cloneDir):
                    if not self.isProjDir(cloneDir):
                        # name_YYYY_MM_DD-num le plus elevé
                        dd = datetime.now()
                        cd = 100
                        while cd > 0 and not os.path.exists(cloneDir+"/%s_%i_%i_%i-%i"%(cloneName,dd.year,dd.month,dd.day,cd)):
                            cd -= 1
                        if cd == 100:
                                output.notify(self,"Error","With this version, you cannot have more than 100 \
                                        project directories\nfor the same project name and in the same directory")
                        else:
                            clonedir = cloneDir+"/%s_%i_%i_%i-%i"%(cloneName,dd.year,dd.month,dd.day,(cd+1))
                            try:
                                # on crée le dossier de destination et on y copie les fichiers utiles
                                os.mkdir(clonedir)
                                for filepath in [self.main_conf_name,
                                        self.hist_conf_name,
                                        self.gen_conf_name,
                                        self.table_header_conf_name,
                                        self.reftableheader_name,
                                        self.ascertainment_conf_name,
                                        current_project.dataFileName]:
                                    if os.path.exists("%s/%s"%(current_project.dir,filepath)):
                                         shutil.copy("%s/%s"%(current_project.dir,filepath),"%s/%s"%(clonedir,filepath))

                                # si les noms sont différents, on le charge
                                if cloneName != current_project.name:
                                    self.openProject(clonedir)
                                else:
                                    output.notify(self,"Load error","The clone project was successfully created but can not be opened because\
                                                it has the same name than the origin project\nClose the origin project if you want to open the clone")
                                log(1,"Project %s was successfully cloned in %s"%(current_project.dir,clonedir))
                            except OSError as e:
                                output.notify(self,"Error",str(e))
                    else:
                        output.notify(self,"Incorrect directory","A project can not be in a project directory")

    def checkProjectName(self,name):
        """ vérifie si le nom de projet ne comporte pas de caractères illégaux et s'il n'est pas vide
        """
        if name == "":
            output.notify(self,"Name error","The project name cannot be empty.")
            return False
        for c in self.illegalProjectNameCharacters:
            if c in name:
                output.notify(self,"Name error","The following characters are not allowed in project name : . \" ' _ - /")
                return False
        return True

    def newProject(self,path=None):
        """ Création d'un projet
        """
        from projectMsatSeq import ProjectMsatSeq
        from projectSnp import ProjectSnp
        log(1,'Attempting to create a new project')
        ok = True
        if path == None:
            fileDial = QtGui.QFileDialog(self,"Select location of the new project")
            fileDial.setAcceptMode(QFileDialog.AcceptSave)
            fileDial.setLabelText(QtGui.QFileDialog.Accept,"Create project")
            fileDial.setLabelText(QtGui.QFileDialog.FileName,"Project name")
            ok = (fileDial.exec_() == 1)
            result = fileDial.selectedFiles()
            if len(result) > 0:
                path = result[0]
            path = "%s"%path
            # on enleve l'eventuel '/' de fin et on extrait le nom du projet
            if path != None and len(path) > 0 and len(path.split('/')) > 0 :
                if path[-1] == "/":
                    path = path[:-1]
                name = path.split("/")[-1]
        else:
            name = path.split("/")[-1]

        # verification de l'existence du dossier dans lequel le dossier du projet 
        # va être créé
        if not os.path.exists(os.path.dirname(path)):
            output.notify(self,"Creation impossible","Directory %s does not exist.\nImpossible to create the project in %s"%(os.path.dirname(path),path))
            return

        if ok:
            log(1,'The name of the new project will be %s\nIt will be saved in %s'%(name,path))
            if self.checkProjectName(name):
                proj_name_list = []
                for p in self.project_list:
                    proj_name_list.append(p.name)
                if not name in proj_name_list:
                    # dans tous les cas, on fait un projet msatseq
                    # le cas snp est géré dans dataFileSelectionAndCopy
                    # la creation du dossier est faite plus tard
                    if "SNP" in self.sender().text():
                        newProj = ProjectSnp(name,None,self)
                    else:
                        newProj = ProjectMsatSeq(name,None,self)
                    # un nouveau projet a au moins un scenario
                    newProj.hist_model_win.addSc()
                    self.project_list.append(newProj)
                    # si c'est le premier projet, on permet la fermeture par le menu
                    # ajout au tabwidget de l'ui principale
                    # ajout de l'onglet
                    self.ui.tabWidget.addTab(newProj,newProj.name)
                    self.ui.tabWidget.setCurrentWidget(newProj)

                    # creation du dir en attendant la selection du datafile
                    newProj.dirCreation(path)
                    newProj.initializeRNG()

                    if len(self.project_list) == 1:
                        self.saveAllProjActionMenu.setDisabled(False)
                        self.saveButton.setDisabled(False)
                    if len(self.project_list) == 2:
                        self.nextProjectActionMenu.setDisabled(False)
                        self.prevProjectActionMenu.setDisabled(False)
                        self.saveAllButton.setDisabled(False)
                    self.switchToMainStack()
                    self.updateDoc(newProj)
                    log(1,'Project %s successfully created in %s'%(newProj.name,newProj.dir))
                else:
                    output.notify(self,"Name error","A project named \"%s\" is already loaded."%name)

    def closeProject(self,index,save=None):
        """ ferme le projet qui est à l'index "index" du tabWidget
        le sauvegarde si save == True et le déverrouille
        """
        cdir = self.ui.tabWidget.currentWidget().dir
        if save == None:
            if not output.debug:
                qmb = QMessageBox()
                qmb.setText("Do you want to save the Project ?")
                qmb.setStandardButtons(QMessageBox.Save | QMessageBox.Discard | QMessageBox.Cancel)
                qmb.setDefaultButton(QMessageBox.Save)
                reply = qmb.exec_()
                if (reply == QMessageBox.Cancel):
                    return
                else:
                    save = (reply == QtGui.QMessageBox.Save)
            else:
                save = True
        
        # est ce que l'index est out of range?
        if self.ui.tabWidget.widget(index) != 0:
            # deverrouillage du projet
            self.ui.tabWidget.widget(index).unlock()
            # suppression du projet dans la liste locale
            self.project_list.remove(self.ui.tabWidget.widget(index))
            projToClose = self.ui.tabWidget.widget(index)
            if save:
                self.ui.tabWidget.widget(index).save()
        self.ui.tabWidget.removeTab(index)
        del(projToClose)
        # si c'est le dernier projet, on désactive la fermeture par le menu
        if len(self.project_list) == 0:
            self.saveAllProjActionMenu.setDisabled(True)
            self.saveButton.setDisabled(True)
            self.switchToWelcomeStack()
        if len(self.project_list) == 1:
            self.nextProjectActionMenu.setDisabled(True)
            self.prevProjectActionMenu.setDisabled(True)
            self.saveAllButton.setDisabled(True)
        log(1,"Project '%s' closed"%(cdir))

    def closeCurrentProject(self,save=None):
        """ ferme le projet courant, celui de l'onglet séléctionné
        """
        self.closeProject(self.ui.tabWidget.currentIndex(),save)

    def deleteReftableCurrentProject(self):
        self.ui.tabWidget.currentWidget().deleteReftable()

    def saveCurrentProject(self):
        """ sauve le projet courant, cad ecrit les fichiers temporaires de conf
        """
        self.ui.tabWidget.currentWidget().save()
        log(1,"Project '%s' saved"%(self.ui.tabWidget.currentWidget().name))

    def saveAllProjects(self):
        """ sauve tous les projets
        """
        for proj in self.project_list:
            proj.save()

    def deleteCurrentProject(self):
        """ efface le projet courant
        """
        if self.ui.tabWidget.currentWidget().canBeDeleted():
            if not output.debug:
                reply = QtGui.QMessageBox.question(self, 'Deletion confirmation',
                    "Are you sure you want to delete Project %s ?"%(self.ui.tabWidget.currentWidget().name), QtGui.QMessageBox.Yes | 
                    QtGui.QMessageBox.No, QtGui.QMessageBox.Yes)
                ok = (reply == QtGui.QMessageBox.Yes)
            else:
                ok = True
            if ok:
                ddir = self.ui.tabWidget.currentWidget().dir
                self.deleteProject(self.ui.tabWidget.currentIndex())
                log(1,"Project '%s' deleted"%(ddir))
        else:
            output.notify(self,"Impossible to delete","Impossible to delete that kind of project")

    def deleteProject(self,index):
        """ efface le projet dont l'index est donné en paramètre
        """
        projdir = str(self.ui.tabWidget.widget(index).dir)
        shutil.rmtree(projdir)
        # on ferme le projet sans sauver
        self.closeProject(index,False)

    def nextProject(self):
        if self.ui.tabWidget.count() > 0:
            self.ui.tabWidget.setCurrentIndex((self.ui.tabWidget.currentIndex() + 1) % self.ui.tabWidget.count())

    def prevProject(self):
        if self.ui.tabWidget.count() > 0:
            self.ui.tabWidget.setCurrentIndex((self.ui.tabWidget.currentIndex() - 1) % self.ui.tabWidget.count())

    def isProjDir(self,dir):
        """ retourne vrai si le répertoire donné en paramètre est un 
        repertoire de projet ou s'il est dans un répertoire de projet
        """
        if os.path.exists("%s/%s"%(dir,self.main_conf_name))\
                or os.path.exists("%s/%s"%(os.path.abspath(os.path.join(dir, '..')),self.main_conf_name)):
            return True
        else:
            return False

    def closeEvent(self, event):
        """ Redéfinition de l'évennement de fermeture de la
        fenetre principale pour arrêter l'activité des projets
        et sauvegarder certains paramètres
        """
        QApplication.setOverrideCursor( Qt.WaitCursor )
        for proj in self.project_list:
            # si le projet est bien créé (et pas en cours de création)
            if proj.dir != None:
                proj.stopActivities()
        self.preferences_win.saveRecent()
        self.preferences_win.saveToolBarPosition(self.toolBarArea(self.ui.toolBar))
        self.preferences_win.writeConfigFile()
        # windows ne nettoie pas son tray, aidons le
        self.systrayHandler.hide()
        QApplication.restoreOverrideCursor()
        event.accept()

    def event(self,event):
        """ méthode d'interception des event
        les event arrivent jusque là s'ils n'ont pas été interceptés plus
        tot par d'autres objets fils
        """
        if event.type() == QEvent.MouseButtonRelease and event.button() == 2:
            elemList = self.findChildren(QLabel)
            elemList += self.findChildren(QPushButton)
            for c in elemList:
                if c.underMouse():
                    self.file_menu.popup(QCursor.pos())

                    on = str(c.objectName())
                    log(1,"Asking documentation of %s"%on)
                    try:
                        output.notify(self,"Documentation",self.documentator.getDocString(on))
                    except Exception as e:
                        output.notify(self,"Documentation error","%s"%e)
            log(3,"Button '%s' pressed"%event.button())
        # Quand on reprend le focus sur la fenêtre, on stoppe le
        # clignottement éventuel de l'icone du systray
        elif event.type() == QEvent.WindowActivate:
            self.systrayHandler.leaveSystrayBlink()
        return QWidget.event(self,event)

    def dropEvent(self,event):
        """ Ouverture du projet par glisser deposer
        """
        for url in event.mimeData().urls():
            path = url.toString()
            log(1,"Folder %s was dragNdroped to be opened"%path)
            # WINDOWS n'a pas de /
            if "win" in sys.platform and len(path) > 0:
                self.openProject(path.replace("file:///",""))
            else:
                self.openProject(path.replace("file://",""))
        event.acceptProposedAction()

    def dragEnterEvent(self,event):
        event.acceptProposedAction()

class ImportProjectThread(Thread):
    """ Classe qui effectue l'import de la classe project en parallèle au chargement
    du programme. L'interface démarre beaucoup plus vite et charge la classe projet 
    en même temps qu'elle apparait
    """
    def __init__(self):
        super(ImportProjectThread,self).__init__()
        self.start()
    def run(self):
        #log(4,"Pre-loading of Project class STARTING")
        from projectMsatSeq import ProjectMsatSeq
        from projectSimulation import ProjectSimulationSnp,ProjectSimulationMsatSeq
        from projectSnp import ProjectSnp
        #log(4,"Pre-loading of Project class FINISHED")

def main():
    output.debug = False
    if not os.path.exists(os.path.expanduser("~/.diyabc/")):
        # c'est sans doute la première fois qu'on lance diyabc
        # sous linux, on appelle gconf pour voir les icones dans les menus et boutons
        if "linux" in sys.platform:
            cmd_args_list = ["gconftool-2", "--type", "boolean", "--set", "/desktop/gnome/interface/buttons_have_icons", "true"]
            p = subprocess.Popen(cmd_args_list) 
            cmd_args_list = ["gconftool-2", "--type", "boolean", "--set", "/desktop/gnome/interface/menus_have_icons", "true"]
            p = subprocess.Popen(cmd_args_list) 

    nargv = sys.argv
    # getting cmd option
    cmd=False
    if "-cmd" in nargv:
        nargv.remove("-cmd")
        cmd=True

    projects_to_open = []
    for arg in nargv[1:]:
        if not arg.startswith('-'):
            projects_to_open.append(arg)

    # determine le nom du fichier de log
    if not os.path.exists(os.path.expanduser("~/.diyabc/")):
        os.mkdir(os.path.expanduser("~/.diyabc/"))
    if not os.path.exists(os.path.expanduser("~/.diyabc/logs/")):
        os.mkdir(os.path.expanduser("~/.diyabc/logs/"))
    dd = datetime.now()
    logfile = os.path.expanduser("~/.diyabc/logs/%02d_%02d_%s-%02dh_%02dm-%s.log"%(dd.day,dd.month,dd.year,dd.hour,dd.minute,os.getpid()))

    # instanciation des objets principaux
    app = QApplication(nargv)
    myapp = Diyabc(app,projects=projects_to_open,logfile=logfile)
    myapp.setWindowTitle("DIYABC %s  (%s)"%(VERSION,VERSION_DATE))
    myapp.show()
    # les .app sous macos nécessitent cela pour que l'appli s'affiche en FG
    if "darwin" in sys.platform and ".app/" in sys.argv[0]:
        myapp.raise_()

    # pour les logs dans un fichier et sur le terminal
    # chaque TeeLogger remplace une sortie et écrit dans 
    # le fichier qu'on lui donne
    myOut = TeeLogger(logfile,True,myapp.showLog)
    myErr = TeeLogger(logfile,False,myapp.showLog)
    sys.stdout = myOut
    sys.stderr = myErr
    log(1,"\033[5;36m DIYABC launched \033[00m")

    # effacement des logs de plus de N jours si le dossier de logs dépasse X Mo
    try:
        logRotate(os.path.expanduser("~/.diyabc/logs/"),10,50)
    except Exception as e:
        output.notify(None,"Log rotate failure","%s"%e)

    # création du prompt
    if cmd:
        cmdT = cmdThread(myapp)
    # parallel import of project and simulationProject classes
    yop = ImportProjectThread()

    # lancement de la boucle Qt
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()

