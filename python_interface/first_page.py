# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from first_page_ui import Ui_MainWindow
from project import *

class MainWindowTest(QMainWindow):
    def __init__(self,parent=None):
        super(MainWindowTest,self).__init__(parent)
        self.createWidgets()
        self.project_list = []

    def createWidgets(self):
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        #proj1 = Project(self.ui,"ploproj")
        self.ui.tab_6.hide()

        # manual alignment set
        #self.ui.verticalLayout_2.setAlignment(self.ui.newAnButton,Qt.AlignCenter)

        file_menu = self.ui.menubar.addMenu("File")
        file_menu.addAction("Open",self.file_open,QKeySequence(Qt.CTRL + Qt.Key_O))
        file_menu.addAction("New Project",self.newProject,QKeySequence(Qt.CTRL + Qt.Key_N))
        action = file_menu.addAction("Quit",self.close,QKeySequence(Qt.CTRL + Qt.Key_Q))
	
	#mettre plusieurs raccourcis claviers pour le meme menu
	#action.setShortcuts([QKeySequence(Qt.CTRL + Qt.Key_Q),QKeySequence(Qt.Key_Escape)])



    def file_open(self):
        pass

    def newProject(self):
        text, ok = QtGui.QInputDialog.getText(self, 'New project', 'Enter the name of the new project:')
        if ok:
            self.project_list.append(Project(self.ui,text))



        #QObject.connect(self.ui.lcdNumber,SIGNAL("sliderMoved(int)"),self.maj_prog)
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
    myapp = MainWindowTest()
    myapp.show()
    sys.exit(app.exec_())


