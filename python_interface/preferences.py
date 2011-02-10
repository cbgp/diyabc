# -*- coding: utf-8 -*-

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from preferences_ui import Ui_MainWindow

class Preferences(QMainWindow):
    """ Classe principale qui est aussi la fenêtre principale de la GUI
    """
    def __init__(self,parent=None):
        super(Preferences,self).__init__(parent)
        self.createWidgets()

        self.ui.tabWidget.setTabText(0,"Connexion")
        self.ui.tabWidget.setTabText(1,"def mut mod M")
        self.ui.tabWidget.setTabText(2,"def mut mod S")


    def createWidgets(self):
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        QObject.connect(self.ui.applyButton,SIGNAL("clicked()"),self.close)
        QObject.connect(self.ui.cancelButton,SIGNAL("clicked()"),self.close)
