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

class SetMutationModelSequencesRefTable(SetMutationModelSequences):
    def __init__(self,parent=None,box_group=None):
        super(SetMutationModelSequencesRefTable,self).__init__(parent,box_group)

    def exit(self):
        ## reactivation des onglets
        #self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
        #self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        #self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))
        self.parent.parent.ui.refTableStack.removeWidget(self)
        self.parent.parent.ui.refTableStack.setCurrentWidget(self.parent)

    def clear(self):
        self.parent.clearMutationModelSeq(self.box_group)

    def validate(self):
        """ vérifie la validité des informations entrées dans le mutation model
        retourne au setGen tab et set la validité du mutation model du groupe
        """
        if self.allValid():
            self.exit()
            self.parent.setMutationSeqValid_dico[self.box_group] = True
            self.parent.writeGeneticConfFromGui()

    def getParamTableHeader(self):
        result = u""
        gnumber = self.parent.groupList.index(self.box_group)+1
        if float(str(self.ui.mmrMinEdit.text())) < float(str(self.ui.mmrMaxEdit.text())):
            pname = u"µseq_%s"%(gnumber)
            result += pname
            for i in range(14-len(pname)):
                result += " "
        if not self.ui.jukesRadio.isChecked():
            if float(str(self.ui.mc1MinEdit.text())) < float(str(self.ui.mc1MaxEdit.text())):
                pname = u"k1seq_%s"%(gnumber)
                result += pname
                for i in range(14-len(pname)):
                    result += " "
            if self.ui.tamuraRadio.isChecked():
                if float(str(self.ui.mc2MinEdit.text())) < float(str(self.ui.mc2MaxEdit.text())):
                    pname = u"k2seq_%s"%(gnumber)
                    result += pname
                    for i in range(14-len(pname)):
                        result += " "
        return result



