#!/usr/bin/python
# -*- coding: utf-8 -*-

# variable qui sera changée par le générateur d'exécutable
VERSION='version 2.0.0'

import time
#import dataImages
import shutil
import codecs
import sys
import os
if sys.platform.startswith('win'):
    sys.path.append("Python26/Lib/site-packages")
# pour connaitre les modules, on manipule le pythonpath
sys.path.append("/".join(os.getcwd().split('/')[:-1]))
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from PyQt4 import uic
#from uis.diyabc_ui import Ui_MainWindow
from project import *
from preferences import Preferences
from documentator import Documentator
import output

formDiyabc,baseDiyabc = uic.loadUiType("uis/diyabc.ui")

class Diyabc(formDiyabc,baseDiyabc):
    """ Classe principale qui est aussi la fenêtre principale de la GUI
    """
    def __init__(self,app,parent=None,projects=None):
        super(Diyabc,self).__init__(parent)
        self.app = app
        self.project_list = []
        
        self.main_conf_name = "conf.tmp"
        self.hist_conf_name = "conf.hist.tmp"
        self.gen_conf_name = "conf.gen.tmp"
        self.analysis_conf_name = "conf.analysis"
        self.table_header_conf_name = "conf.th.tmp"
        self.reftableheader_name = "header.txt"

        self.scenario_pix_dir_name = "scenario_pictures"
        self.scenario_pix_basename = "scenario"
        self.PCA_dir_name = "PCA_pictures"

        self.preferences_win = Preferences(self)
        self.preferences_win.loadPreferences()
        #self.defaultMMMValues = self.preferences.getDicoMMM()

        self.createWidgets()
        self.setWindowIcon(QIcon("docs/accueil_pictures/coccicon.png"))

        self.illegalProjectNameCharacters = ['_','-',"'",'"','.','/']

        self.documentator = Documentator("docs/documentation.xml",self)

        # ouverture des projets donnés en paramètre au lancement
        if len(projects) > 0:
            self.switchToMainStack()
        for projDirName in projects:
            self.openProject(projDirName)

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        #pal = QPalette()
        #pal.setBrush(self.backgroundRole(), QBrush(QImage("docs/accueil_pictures/correct.png")))
        #self.setPalette(pal)

        #im = QImage(dataImages.dada,200,200)
        #pic = QPixmap()
        #pic.convertFromImage(im)
        pic = QPixmap("docs/accueil_pictures/black_red.png")
        self.ui.imgLabel.setPixmap(pic)
        self.switchToWelcomeStack()

        #proj1 = Project(self.ui,"ploproj")
        self.ui.tab_6.hide()
        self.ui.tabWidget.removeTab(0)

        # gestion du menu
        file_menu = self.ui.menubar.addMenu("&File")
        self.file_menu = file_menu
        file_menu.addAction("&New Project",self.newProject,QKeySequence(Qt.CTRL + Qt.Key_N))
        file_menu.addAction("&Open",self.openProject,QKeySequence(Qt.CTRL + Qt.Key_O))
        self.saveProjActionMenu = file_menu.addAction("&Save current project",self.saveCurrentProject,QKeySequence(Qt.CTRL + Qt.Key_S))
        self.deleteProjActionMenu = file_menu.addAction("&Delete current project",self.deleteCurrentProject,QKeySequence(Qt.CTRL + Qt.Key_D))
        self.cloneProjActionMenu = file_menu.addAction("&Clone current project",self.cloneCurrentProject,QKeySequence(Qt.CTRL + Qt.Key_K))
        self.closeProjActionMenu = file_menu.addAction("C&lose current project",self.closeCurrentProject,QKeySequence(Qt.CTRL + Qt.Key_W))
        self.closeProjActionMenu.setDisabled(True)
        self.saveProjActionMenu.setDisabled(True)
        self.deleteProjActionMenu.setDisabled(True)
        self.cloneProjActionMenu.setDisabled(True)
        file_menu.addAction("&Preferences",self.setPreferences,QKeySequence(Qt.CTRL + Qt.Key_P))
        action = file_menu.addAction("&Quit",self.close,QKeySequence(Qt.CTRL + Qt.Key_Q))
        #mettre plusieurs raccourcis claviers pour le meme menu
        action.setShortcuts([QKeySequence(Qt.CTRL + Qt.Key_Q),QKeySequence(Qt.Key_Escape)])
        #style_menu = self.ui.menubar.addMenu("Style")
        #action_group = QActionGroup(style_menu)
        #for stxt in self.styles:
        #    self.style_actions[stxt] = style_menu.addAction(stxt,self.changeStyle)
        #    self.style_actions[stxt].setActionGroup(action_group)
        #    self.style_actions[stxt].setCheckable(True)
        navigate_menu = self.ui.menubar.addMenu("&Navigate")
        navigate_menu.addAction("&Next Project",self.nextProject,QKeySequence(Qt.CTRL + Qt.Key_PageDown))
        navigate_menu.addAction("&Previous Project",self.prevProject,QKeySequence(Qt.CTRL + Qt.Key_PageUp))
        help_menu = self.ui.menubar.addMenu("&Help")
        help_menu.addAction("&About DIYABC",self.switchToWelcomeStack)
	
        QObject.connect(self.ui.tabWidget,SIGNAL("tabCloseRequested(int)"),self.closeProject)

        QObject.connect(self.ui.openProjectButton,SIGNAL("clicked()"),self.openProject)
        QObject.connect(self.ui.newProjectButton,SIGNAL("clicked()"),self.newProject)
        QObject.connect(self.ui.skipWelcomeButton,SIGNAL("clicked()"),self.switchToMainStack)
        self.ui.versionLabel.setText('%s'%VERSION)
        #self.ui.frame.setDisabled(True)
        #self.ui.frame.setAutoFillBackground(True)
        #self.ui.frame.setBackgroundColor(Qt.Blue)

        #self.setCursor(QCursor(QPixmap("/home/julien/vcs/git/diyabc.git/python_interface/docs/accueil_pictures/coccicon.png").scaled(32,32)))

    def switchToMainStack(self):
        self.ui.menubar.show()
        self.ui.stackedWidget.setCurrentIndex(0)

    def switchToWelcomeStack(self):
        self.ui.menubar.hide()
        self.ui.stackedWidget.setCurrentIndex(1)

    def setPreferences(self):
        self.preferences_win.show()


    #def changeStyle(self):
    #    for stxt in self.styles:
    #        if self.sender() == self.style_actions[stxt]:
    #            self.app.setStyle(stxt)

    def openProject(self,dir=None):
        """ ouverture d'un projet existant
        """
        if dir == None:
            qfd = QFileDialog()
            dir = str(qfd.getExistingDirectory())
        # on enlève le dernier '/' s'il y en a un
        if dir != "" and dir[-1] == "/":
            dir = dir[:-1]
        proj_name = str(dir).split('/')[-1].split('_')[0]
        # si le dossier existe et qu'il contient conf.hist.tmp
        if dir != "":
            if os.path.exists(dir) and os.path.exists("%s/%s"%(dir,self.main_conf_name)):
                project_name = dir.split('/')[-1].split('_')[0]
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
    \nIf you say YES, think about closing potential other DIYABCs which manipulate this project"%proj_name,
                                    QtGui.QMessageBox.Yes | QtGui.QMessageBox.No,QtGui.QMessageBox.No)
                            if reply == QtGui.QMessageBox.Yes:
                                os.remove("%s/.DIYABC_lock"%dir)
                            else:
                                proj_ready_to_be_opened = False
                        else:
                            # si on est en mode debug, on vire le verrou sans sommation
                            os.remove("%s/.DIYABC_lock"%dir)
                    if proj_ready_to_be_opened:
                        proj_to_open = Project(project_name,dir,self)
                        proj_to_open.loadFromDir()
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
                            self.closeProjActionMenu.setDisabled(False)
                            self.saveProjActionMenu.setDisabled(False)
                            self.deleteProjActionMenu.setDisabled(False)
                            self.cloneProjActionMenu.setDisabled(False)
                        # on quitte la page d'accueil si on y était
                        self.switchToMainStack()
                        # creation du lock
                        proj_to_open.lock()
                else:
                    output.notify(self,"Name error","A project named \"%s\" is already loaded"%proj_name)
            else:
               output.notify(self,"Load error","\"%s\" is not a project directory"%dir)

    def cloneCurrentProject(self,cloneBaseName=None,cloneDir=None):
        """ duplique un projet vers un autre répertoire
        demande le nom du clone puis le répertoire dans lequel mettre le clone du projet
        """ 
        self.saveCurrentProject()
        current_project = self.ui.tabWidget.currentWidget()
        ok = True
        if cloneBaseName == None:
            cloneBaseName, ok = QtGui.QInputDialog.getText(self, 'Clone project', 'Enter the name of the clone project:')
        if ok:
            if self.checkProjectName(cloneBaseName):
                if cloneDir == None:
                    qfd = QFileDialog()
                    cloneDir = str(qfd.getExistingDirectory(self,"Where to put the clone"))
                if cloneDir != "" and os.path.exists(cloneDir):
                    if not self.isProjDir(cloneDir):
                        # name_YYYY_MM_DD-num le plus elevé
                        dd = datetime.now()
                        #num = 36
                        cd = 100
                        while cd > 0 and not os.path.exists(cloneDir+"/%s_%i_%i_%i-%i"%(cloneBaseName,dd.year,dd.month,dd.day,cd)):
                            cd -= 1
                        if cd == 100:
                                output.notify(self,"Error","With this version, you cannot have more than 100 \
                                        project directories\nfor the same project name and in the same directory")
                        else:
                            clonedir = cloneDir+"/%s_%i_%i_%i-%i"%(cloneBaseName,dd.year,dd.month,dd.day,(cd+1))
                            #self.ui.dirEdit.setText(newdir)
                            try:
                                #print current_project.dir, " to ", clonedir
                                #shutil.copytree(current_project.dir,clonedir)
                                #os.remove("%s/reftable.bin"%clonedir)

                                # on crée le dossier de destination et on y copie les fichiers utiles
                                os.mkdir(clonedir)
                                for filepath in [self.main_conf_name,
                                        self.hist_conf_name,
                                        self.gen_conf_name,
                                        self.table_header_conf_name,
                                        self.reftableheader_name,
                                        current_project.dataFileName,
                                        self.analysis_conf_name]:
                                    if os.path.exists("%s/%s"%(current_project.dir,filepath)):
                                         shutil.copy("%s/%s"%(current_project.dir,filepath),"%s/%s"%(clonedir,filepath))

                                #shutil.copy("%s/%s"%(current_project.dir,self.main_conf_name),"%s/%s"%(clonedir,self.main_conf_name))
                                #shutil.copy("%s/%s"%(current_project.dir,self.hist_conf_name),"%s/%s"%(clonedir,self.hist_conf_name))
                                #shutil.copy("%s/%s"%(current_project.dir,self.gen_conf_name),"%s/%s"%(clonedir,self.gen_conf_name))
                                #shutil.copy("%s/%s"%(current_project.dir,self.table_header_conf_name),"%s/%s"%(clonedir,self.table_header_conf_name))
                                #shutil.copy("%s/%s"%(current_project.dir,self.reftableheader_name),"%s/%s"%(clonedir,self.reftableheader_name))
                                #shutil.copy("%s/%s"%(current_project.dir,current_project.dataFileName),"%s/%s"%(clonedir,current_project.dataFileName))
                                #shutil.copy("%s/%s"%(current_project.dir,self.analysis_conf_name),"%s/%s"%(clonedir,self.analysis_conf_name))

                                # si les noms sont différents, on le charge
                                if cloneBaseName != current_project.name:
                                    self.openProject(clonedir)
                                else:
                                    output.notify(self,"Load error","The cloned has been cloned but can not be opened because\
                                                it has the same name than the origin project\nClose the origin project if you want to open the clone")
                            except OSError,e:
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


    def newProject(self,name=None):
        """ Création d'un projet
        """
        ok = True
        #if name == None:
        #    name, ok = QtGui.QInputDialog.getText(self, 'New project', 'Enter the name of the new project:')
        if name == None:
            fileDial = QtGui.QFileDialog(self,"Select location of the new project")
            fileDial.setLabelText(QtGui.QFileDialog.Accept,"Create project")
            fileDial.setLabelText(QtGui.QFileDialog.FileName,"Project name")
            ok = (fileDial.exec_() == 1)
            result = fileDial.selectedFiles()
            if len(result) > 0:
                path = result[0]
            #path = fileDial.getSaveFileName(self,"Project location")
            path = "%s"%path
            print ok,";",path
            # on enleve l'eventuel '/' de fin et on extrait le nom du projet
            if path != None and len(path) > 0 and len(path.split('/')) > 0 :
                if path[-1] == "/":
                    path = path[:-1]
                name = path.split("/")[-1]

        if ok:
            if self.checkProjectName(name):
                proj_name_list = []
                for p in self.project_list:
                    proj_name_list.append(p.name)
                if not name in proj_name_list:
                    newProj = Project(name,None,self)
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

                    if len(self.project_list) == 1:
                        self.closeProjActionMenu.setDisabled(False)
                        self.saveProjActionMenu.setDisabled(False)
                        self.deleteProjActionMenu.setDisabled(False)
                        self.cloneProjActionMenu.setDisabled(False)
                    self.switchToMainStack()
                else:
                    output.notify(self,"Name error","A project named \"%s\" is already loaded."%name)

    def closeProject(self,index,save=None):
        """ ferme le projet qui est à l'index "index" du tabWidget
        le sauvegarde si save == True et le déverrouille
        """
        if save == None:
            if not output.debug:
                reply = QtGui.QMessageBox.question(self, 'Message',
                    "Do you want to save the Project ?", QtGui.QMessageBox.Yes | 
                    QtGui.QMessageBox.No, QtGui.QMessageBox.Yes)
                save = (reply == QtGui.QMessageBox.Yes)
            else:
                save = True
        
        # est ce que l'index est out of range?
        if self.ui.tabWidget.widget(index) != 0:
            # deverrouillage du projet
            self.ui.tabWidget.widget(index).unlock()
            # suppression du projet dans la liste locale
            self.project_list.remove(self.ui.tabWidget.widget(index))
            if save:
                self.ui.tabWidget.widget(index).save()
        self.ui.tabWidget.removeTab(index)
        # si c'est le dernier projet, on désactive la fermeture par le menu
        if len(self.project_list) == 0:
            self.closeProjActionMenu.setDisabled(True)
            self.saveProjActionMenu.setDisabled(True)
            self.deleteProjActionMenu.setDisabled(True)
            self.cloneProjActionMenu.setDisabled(True)
            self.switchToWelcomeStack()

    def closeCurrentProject(self,save=None):
        """ ferme le projet courant, celui de l'onglet séléctionné
        """
        self.closeProject(self.ui.tabWidget.currentIndex(),save)
    def saveCurrentProject(self):
        """ sauve le projet courant, cad ecrit les fichiers temporaires de conf
        """
        self.ui.tabWidget.currentWidget().save()
    def deleteCurrentProject(self):
        """ efface le projet courant
        """
        self.deleteProject(self.ui.tabWidget.currentIndex())
    def deleteProject(self,index):
        """ efface le projet dont l'index est donné en paramètre
        """
        projdir = self.ui.tabWidget.widget(index).dir
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
        for proj in self.project_list:
            # si le projet est bien créé (et pas en cours de création)
            if proj.dir != None:
                proj.stopRefTableGen()
                proj.unlock()
        event.accept()
    #    reply = QtGui.QMessageBox.question(self, 'Message',
    #        "Are you sure to quit?", QtGui.QMessageBox.Yes | 
    #        QtGui.QMessageBox.No, QtGui.QMessageBox.Yes)

    #    if reply == QtGui.QMessageBox.Yes:
    #        event.accept()
    #    else:
    #        event.ignore()

    #def mousePressEvent(self,event):
    #    print self.sender()
    #    print "plop"
    def event(self,event):
        if event.type() == QEvent.MouseButtonRelease and event.button() == 2:
            elemList = self.findChildren(QLabel)
            elemList += self.findChildren(QPushButton)
            for c in elemList:
                if c.underMouse():
                    self.file_menu.popup(QCursor.pos())

                    on = str(c.objectName())
                    if self.documentator.getDocString(on) != None:
                        output.notify(self,"Documentation",self.documentator.getDocString(on))
                    else:
                        output.notify(self,"Documentation","No documentation found")
            print event.button()
            print "ow yeah"
        return QWidget.event(self,event)

    def dropEvent(self,event):
        """ Ouverture du projet par glisser deposer
        """
        for url in event.mimeData().urls():
            path = url.toString()
            # WINDOWS n'a pas de /
            if "win" in sys.platform and len(path) > 0:
                print "winwin"
                self.openProject(path.replace("file:///",""))
            else:
                self.openProject(path.replace("file://",""))
        event.acceptProposedAction()

    def dragEnterEvent(self,event):
        event.acceptProposedAction()

if __name__ == "__main__":
    nargv = sys.argv
    projects_to_open = nargv[1:]
    #nargv.extend(["-title","DIYABC v%s"%VERSION])
    app = QApplication(nargv)
    myapp = Diyabc(app,projects=projects_to_open)
    myapp.setWindowTitle("DIYABC %s"%VERSION)
    myapp.show()
    myapp.setAcceptDrops(True)
    #QTest.mouseClick(myapp.ui.skipWelcomeButton,Qt.LeftButton)
    sys.exit(app.exec_())

