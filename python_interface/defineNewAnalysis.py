# -*- coding: utf-8 -*-

import os
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from defineNewAnalysis_ui import Ui_Frame

class DefineNewAnalysis(QFrame):
    """ Classe pour créer une fenêtre à l'intérieur de laquelle on dessine les scénarios (valides car déjà vérifiés)
    on passe au constructeur une liste de dictionnaires contenant les informations sur chaque scénario
    """
    def __init__(self,parent=None):
        super(DefineNewAnalysis,self).__init__(parent)
        self.parent=parent
        self.createWidgets()
        self.ui.verticalLayout_2.setAlignment(Qt.AlignHCenter)
        self.ui.verticalLayout_2.setAlignment(Qt.AlignTop)



    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)

    def validate(self):
        pass

    def exit(self):
        # reactivation des onglets
        self.parent.setTabEnabled(1,True)
        self.parent.setTabEnabled(0,True)
        self.parent.removeTab(self.parent.indexOf(self))
        self.parent.setCurrentIndex(1)

