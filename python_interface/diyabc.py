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
        pass

    def newProject(self):
        """ Création d'un projet
        """
        text, ok = QtGui.QInputDialog.getText(self, 'New project', 'Enter the name of the new project:')
        if ok:
            self.project_list.append(Project(self.ui,text,self))
            # si c'est le premier projet, on permet la fermeture par le menu
            if len(self.project_list) == 1:
                self.closeProjActionMenu.setDisabled(False)

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

    def closeEvent(self, event):
        reply = QtGui.QMessageBox.question(self, 'Message',
            "Are you sure to quit?", QtGui.QMessageBox.Yes | 
            QtGui.QMessageBox.No, QtGui.QMessageBox.Yes)

        if reply == QtGui.QMessageBox.Yes:
            event.accept()
        else:
            event.ignore()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    myapp = Diyabc()
    myapp.show()
    sys.exit(app.exec_())


