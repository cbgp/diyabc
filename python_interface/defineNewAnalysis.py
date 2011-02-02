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

        #QObject.connect(self.ui.closeButton,SIGNAL("clicked()"),self.exit)
        #QObject.connect(self.ui.saveButton,SIGNAL("clicked()"),self.save)

    def exit(self):
        # reactivation des onglets
        self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
        self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))

