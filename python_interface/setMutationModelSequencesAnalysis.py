# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from setMutationModelSequences_ui import Ui_Frame
from setMutationModelSequences import SetMutationModelSequences

class SetMutationModelSequencesAnalysis(SetMutationModelSequences):
    def __init__(self,parent=None,box_group=None):
        super(SetMutationModelSequencesAnalysis,self).__init__(parent,box_group)

        self.ui.clearButton.hide()

    def exit(self):
        # reactivation des onglets
        self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
        self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))

    def clear(self):
        pass

    def validate(self):
        """ vérifie la validité des informations entrées dans le mutation model
        retourne au setGen tab et set la validité du mutation model du groupe
        """
        if self.allValid():
            self.exit()
            self.parent.setMutationSeqValid_dico[self.box_group] = True

