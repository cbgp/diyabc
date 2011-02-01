# -*- coding: utf-8 -*-

import time
import shutil
import codecs
import sys
import os
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from diyabc_ui import Ui_MainWindow
from project import *

class Diyabc(QMainWindow):
    """ Classe principale qui est aussi la fenêtre principale de la GUI
    """
    def __init__(self,app,parent=None):
        super(Diyabc,self).__init__(parent)
        self.app = app
        self.project_list = []
        self.hist_conf_name = "cont.hist.tmp"
        self.scenario_pix_dir_name = "scenario_pictures"
        self.scenario_pix_basename = "scenario"

        self.styles = []
        for i in QStyleFactory.keys():
            self.styles.append(str(i))
        self.style_actions = {}

        self.createWidgets()

        self.default_style = self.style_actions["Cleanlooks"]
        self.default_style.activate(QAction.Trigger)

    def createWidgets(self):
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        #proj1 = Project(self.ui,"ploproj")
        self.ui.tab_6.hide()
        self.ui.tabWidget.removeTab(0)

        # gestion du menu
        file_menu = self.ui.menubar.addMenu("File")
        file_menu.addAction("New Project",self.newProject,QKeySequence(Qt.CTRL + Qt.Key_N))
        file_menu.addAction("Open",self.file_open,QKeySequence(Qt.CTRL + Qt.Key_O))
        self.saveProjActionMenu = file_menu.addAction("Save current project",self.saveCurrentProject,QKeySequence(Qt.CTRL + Qt.Key_S))
        self.deleteProjActionMenu = file_menu.addAction("Delete current project",self.deleteCurrentProject,QKeySequence(Qt.CTRL + Qt.Key_D))
        self.cloneProjActionMenu = file_menu.addAction("Clone current project",self.cloneCurrentProject,QKeySequence(Qt.CTRL + Qt.Key_C))
        self.closeProjActionMenu = file_menu.addAction("Close current project",self.closeCurrentProject,QKeySequence(Qt.CTRL + Qt.Key_W))
        self.closeProjActionMenu.setDisabled(True)
        self.saveProjActionMenu.setDisabled(True)
        self.deleteProjActionMenu.setDisabled(True)
        self.cloneProjActionMenu.setDisabled(True)
        action = file_menu.addAction("Quit",self.close,QKeySequence(Qt.CTRL + Qt.Key_Q))
        #mettre plusieurs raccourcis claviers pour le meme menu
        action.setShortcuts([QKeySequence(Qt.CTRL + Qt.Key_Q),QKeySequence(Qt.Key_Escape)])
        style_menu = self.ui.menubar.addMenu("Style")
        action_group = QActionGroup(style_menu)
        for stxt in self.styles:
            self.style_actions[stxt] = style_menu.addAction(stxt,self.changeStyle)
            self.style_actions[stxt].setActionGroup(action_group)
            self.style_actions[stxt].setCheckable(True)
        navigate_menu = self.ui.menubar.addMenu("Navigate")
        navigate_menu.addAction("Next Project",self.nextProject,QKeySequence(Qt.CTRL + Qt.Key_PageDown))
        navigate_menu.addAction("Previous Project",self.prevProject,QKeySequence(Qt.CTRL + Qt.Key_PageUp))
	
        QObject.connect(self.ui.tabWidget,SIGNAL("tabCloseRequested(int)"),self.closeProject)

    def changeStyle(self):
        for stxt in self.styles:
            if self.sender() == self.style_actions[stxt]:
                self.app.setStyle(stxt)

    def file_open(self,dir=None):
        """ ouverture d'un projet existant
        """
        if dir == None:
            qfd = QFileDialog()
            dir = str(qfd.getExistingDirectory())
        proj_name = str(dir).split('/')[-1].split('_')[0]
        # si le dossier existe et qu'il contient conf.hist.tmp
        if dir != "":
            if os.path.exists(dir) and os.path.exists("%s/conf.tmp"%dir):
                project_name = dir.split('/')[-1].split('_')[0]
                proj_name_list = []
                for p in self.project_list:
                    proj_name_list.append(p.name)
                if not project_name in proj_name_list:
                    proj_ready_to_be_opened = True
                    # si le projet est verrouillé
                    if os.path.exists("%s/.DIYABC_lock"%dir):
                        reply = QMessageBox.question(self,"Project is locked","The %s project is currently locked by another instance of DIYABC\
                                would you like to open it anyway and get the lock for this DIYABC ?"%proj_name,
                                QtGui.QMessageBox.Yes | QtGui.QMessageBox.No,QtGui.QMessageBox.No)
                        if reply == QtGui.QMessageBox.Yes:
                            os.remove("%s/.DIYABC_lock"%dir)
                        else:
                            proj_ready_to_be_opened = False
                    if proj_ready_to_be_opened:
                        proj_to_open = Project(project_name,dir,self)
                        proj_to_open.loadFromDir()
                        self.project_list.append(proj_to_open)
                        self.ui.tabWidget.addTab(proj_to_open,proj_to_open.name)
                        self.ui.tabWidget.setCurrentWidget(proj_to_open)
                        # si c'est le premier projet, on permet la fermeture/del/save par le menu
                        if len(self.project_list) == 1:
                            self.closeProjActionMenu.setDisabled(False)
                            self.saveProjActionMenu.setDisabled(False)
                            self.deleteProjActionMenu.setDisabled(False)
                            self.cloneProjActionMenu.setDisabled(False)
                        # creation du lock
                        proj_to_open.lock()
                else:
                    QMessageBox.information(self,"Name error","A project named \"%s\" is already loaded"%proj_name)
            else:
                QMessageBox.information(self,"Load error","This is not a project directory")

    def cloneCurrentProject(self):
        """ duplique un projet vers un autre répertoire
        demande le nom du clone puis le répertoire dans lequel mettre le clone du projet
        """ 
        self.saveCurrentProject()
        current_project = self.ui.tabWidget.currentWidget()
        proj_base_name, ok = QtGui.QInputDialog.getText(self, 'Clone project', 'Enter the name of the clone project:')
        if ok:
            if proj_base_name != "":
                if ('_' not in proj_base_name) and ('-' not in proj_base_name) and ("'" not in proj_base_name) and ('"' not in proj_base_name) and ('.' not in proj_base_name):
                    qfd = QFileDialog()
                    dirname = str(qfd.getExistingDirectory(self,"Where to put the clone"))
                    if dirname != "":
                        if not self.isProjDir(dirname):
                            # name_YYYY_MM_DD-num le plus elevé
                            dd = datetime.now()
                            #num = 36
                            cd = 100
                            while cd > 0 and not os.path.exists(dirname+"/%s_%i_%i_%i-%i"%(proj_base_name,dd.year,dd.month,dd.day,cd)):
                                cd -= 1
                            if cd == 100:
                                QMessageBox.information(self,"Error","With this version, you cannot have more than 100 \
                                        project directories\nfor the same project name and in the same directory")
                            else:
                                clonedir = dirname+"/%s_%i_%i_%i-%i"%(proj_base_name,dd.year,dd.month,dd.day,(cd+1))
                                #self.ui.dirEdit.setText(newdir)
                                try:
                                    print current_project.dir, " to ", clonedir
                                    shutil.copytree(current_project.dir,clonedir)
                                    # si les noms sont différents, on le charge
                                    if proj_base_name != current_project.name:
                                        self.file_open(clonedir)
                                    else:
                                        QMessageBox.information(self,"Load error","The cloned has been cloned but can not be opened because\
                                                it has the same name than the origin project\nClose the origin project if you want to open the clone")
                                except OSError,e:
                                    QMessageBox.information(self,"Error",str(e))
                        else:
                            QMessageBox.information(self,"Incorrect directory","A project can not be in a project directory")
                else:
                    QMessageBox.information(self,"Name error","The following characters are not allowed in project name : . \" ' _ -")
            else:
                QMessageBox.information(self,"Name error","The project name cannot be empty.")




    def newProject(self):
        """ Création d'un projet
        """
        # TODO verifier caracteres speciaux dans le nom
        text, ok = QtGui.QInputDialog.getText(self, 'New project', 'Enter the name of the new project:')
        if ok:
            if text != "":
                if ('_' not in text) and ('-' not in text) and ("'" not in text) and ('"' not in text) and ('.' not in text):
                    proj_name_list = []
                    for p in self.project_list:
                        proj_name_list.append(p.name)
                    if not text in proj_name_list:
                        newProj = Project(text,None,self)
                        self.project_list.append(newProj)
                        # si c'est le premier projet, on permet la fermeture par le menu
                        # ajout au tabwidget de l'ui principale
                        # ajout de l'onglet
                        self.ui.tabWidget.addTab(newProj,newProj.name)
                        self.ui.tabWidget.setCurrentWidget(newProj)

                        if len(self.project_list) == 1:
                            self.closeProjActionMenu.setDisabled(False)
                            self.saveProjActionMenu.setDisabled(False)
                            self.deleteProjActionMenu.setDisabled(False)
                            self.cloneProjActionMenu.setDisabled(False)
                    else:
                        QMessageBox.information(self,"Name error","A project named \"%s\" is already loaded."%text)
                else:
                    QMessageBox.information(self,"Name error","The following characters are not allowed in project name : . \" ' _ -")
            else:
                QMessageBox.information(self,"Name error","The project name cannot be empty.")

    def closeProject(self,index,save=None):
        """ ferme le projet qui est à l'index "index" du tabWidget
        le sauvegarde si save == True et le déverrouille
        """
        if save == None:
            reply = QtGui.QMessageBox.question(self, 'Message',
                "Do you want to save the Project ?", QtGui.QMessageBox.Yes | 
                QtGui.QMessageBox.No, QtGui.QMessageBox.Yes)
            save = (reply == QtGui.QMessageBox.Yes)
        
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

    def closeCurrentProject(self):
        """ ferme le projet courant, celui de l'onglet séléctionné
        """
        self.closeProject(self.ui.tabWidget.currentIndex())
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
        if os.path.exists("%s/conf.tmp"%dir) or os.path.exists("%s/conf.tmp"%os.path.abspath(os.path.join(dir, '..'))):
            return True
        else:
            return False


    def closeEvent(self, event):
        for proj in self.project_list:
            proj.unlock()
        event.accept()
    #    reply = QtGui.QMessageBox.question(self, 'Message',
    #        "Are you sure to quit?", QtGui.QMessageBox.Yes | 
    #        QtGui.QMessageBox.No, QtGui.QMessageBox.Yes)

    #    if reply == QtGui.QMessageBox.Yes:
    #        event.accept()
    #    else:
    #        event.ignore()

if __name__ == "__main__":
    app = QApplication(sys.argv)
#    if sys.platform.startswith('darwin'):
#        app.setStyle("macintosh")
#    else:
#        app.setStyle("cleanlooks")
    myapp = Diyabc(app)
    myapp.show()
    sys.exit(app.exec_())


