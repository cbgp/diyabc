# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from diyabc_ui import Ui_MainWindow
from project import *

class Diyabc(QMainWindow):
    """ Classe principale qui est aussi la fenêtre principale de la GUI
    """
    def __init__(self,parent=None):
        super(Diyabc,self).__init__(parent)
        self.createWidgets()
        self.project_list = []

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
        file_menu.addAction("Save",self.file_open,QKeySequence(Qt.CTRL + Qt.Key_S))
        self.closeProjActionMenu = file_menu.addAction("Close current project",self.closeCurrentProject,QKeySequence(Qt.CTRL + Qt.Key_W))
        self.closeProjActionMenu.setDisabled(True)
        action = file_menu.addAction("Quit",self.close,QKeySequence(Qt.CTRL + Qt.Key_Q))
	
	#mettre plusieurs raccourcis claviers pour le meme menu
	#action.setShortcuts([QKeySequence(Qt.CTRL + Qt.Key_Q),QKeySequence(Qt.Key_Escape)])
        QObject.connect(self.ui.tabWidget,SIGNAL("tabCloseRequested(int)"),self.closeProject)

    def file_open(self):
        """ ouverture d'un projet existant
        """
        qfd = QFileDialog()
        dir = qfd.getExistingDirectory()
        # si le dossier existe et qu'il contient conf.hist.tmp
        if dir != "":
            if os.path.exists(dir) and os.path.exists("%s/conf.hist.tmp"%dir):
                project_name = dir.split('/')[-1].split('_')[0]
                proj_name_list = []
                for p in self.project_list:
                    proj_name_list.append(p.name)
                if not project_name in proj_name_list:
                    proj_to_open = Project(project_name,dir,self)
                    proj_to_open.loadFromDir()
                    self.project_list.append(proj_to_open)
                    self.ui.tabWidget.addTab(proj_to_open,proj_to_open.name)
                    self.ui.tabWidget.setCurrentWidget(proj_to_open)
                    # si c'est le premier projet, on permet la fermeture par le menu
                    if len(self.project_list) == 1:
                        self.closeProjActionMenu.setDisabled(False)
                else:
                    QMessageBox.information(self,"Name error","A project named %s is already loaded"%text)
            else:
                QMessageBox.information(self,"Load error","This is not a project directory")





    def newProject(self):
        """ Création d'un projet
        """
        # TODO verifier caracteres speciaux dans le nom
        text, ok = QtGui.QInputDialog.getText(self, 'New project', 'Enter the name of the new project:')
        if ok:
            if text != "":
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
                else:
                    QMessageBox.information(self,"Name error","A project named %s is already loaded."%text)
            else:
                QMessageBox.information(self,"Name error","The project name cannot be empty.")

    def closeProject(self,index):
        """ ferme le projet qui est à l'index "index" du tabWidget
        """
        reply = QtGui.QMessageBox.question(self, 'Message',
            "Do you want to save the Project ?", QtGui.QMessageBox.Yes | 
            QtGui.QMessageBox.No, QtGui.QMessageBox.Yes)
        
        if self.ui.tabWidget.widget(index) != 0:
            self.project_list.remove(self.ui.tabWidget.widget(index))
        self.ui.tabWidget.removeTab(index)
        # si c'est le dernier projet, on désactive la fermeture par le menu
        if len(self.project_list) == 0:
            self.closeProjActionMenu.setDisabled(True)

    def closeCurrentProject(self):
        """ ferme le projet courant, celui de l'onglet séléctionné
        """
        self.closeProject(self.ui.tabWidget.currentIndex())

    #def closeEvent(self, event):
    #    reply = QtGui.QMessageBox.question(self, 'Message',
    #        "Are you sure to quit?", QtGui.QMessageBox.Yes | 
    #        QtGui.QMessageBox.No, QtGui.QMessageBox.Yes)

    #    if reply == QtGui.QMessageBox.Yes:
    #        event.accept()
    #    else:
    #        event.ignore()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    app.setStyle("cleanlooks")
    myapp = Diyabc()
    myapp.show()
    sys.exit(app.exec_())


