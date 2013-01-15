# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from setMutationModelSequences import SetMutationModelSequences

class SetMutationModelSequencesAnalysis(SetMutationModelSequences):
    """ Spécialisation de SetMutationModelSequences pour la définition d'une analyse
    """
    def __init__(self,parent=None,box_group=None):
        super(SetMutationModelSequencesAnalysis,self).__init__(parent,box_group)

        self.ui.clearButton.hide()

    def exit(self):
        ## reactivation des onglets
        #self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
        #self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        #self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))
        self.parent.parent.ui.analysisStack.removeWidget(self)
        #self.parent.parent.ui.analysisStack.setCurrentIndex(0)
        self.parent.parent.ui.analysisStack.setCurrentWidget(self.parent)

    def clear(self):
        pass

    def validate(self):
        """ vérifie la validité des informations entrées dans le mutation model
        retourne au setGen tab et set la validité du mutation model du groupe
        """
        if self.allValid():
            self.exit()
            self.parent.setMutationSeqValid_dico[self.box_group] = True

    def hideFixedParameters(self):
        for prefix in ["mmr","mc1","mc2"]:
            exec("mini =    float(str(self.ui.%sMinEdit.text()))"%prefix)
            exec("maxi =    float(str(self.ui.%sMaxEdit.text()))"%prefix)
            if (maxi - mini) < 0.00000001:
                exec("self.ui.%sFrame.setDisabled(True)"%prefix)

