# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from setMutationModelSequences import SetMutationModelSequences
import output

class SetMutationModelSequencesRefTable(SetMutationModelSequences):
    """ spécialisation de SetMutationModelSequences pour les genetic data 
    dans le cadre de la génération de la table de référence
    """
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
            # on met le bouton en police normale pour signaler qu'il est valide
            set_mut_button = self.box_group.findChild(QPushButton,"setMutationButton")
            fontt = set_mut_button.font()
            fontt.setBold(False)
            set_mut_button.setStyleSheet("border-color: #000000")
            set_mut_button.setFont(fontt)

    def getParamTableHeader(self):
        """ retourne une chaine contenant les paramètres (valeurs qui varient)
        pour l'écriture du conf.th
        """
        result = u""
        gnumber = self.parent.groupList.index(self.box_group)+1
        if float(str(self.ui.mmrMinEdit.text())) < float(str(self.ui.mmrMaxEdit.text())):
            pname = u"µseq_%s"%(gnumber)
            result += output.centerHeader(pname,14)
        if not self.ui.jukesRadio.isChecked():
            if float(str(self.ui.mc1MinEdit.text())) < float(str(self.ui.mc1MaxEdit.text())):
                pname = u"k1seq_%s"%(gnumber)
                result += output.centerHeader(pname,14)
            if self.ui.tamuraRadio.isChecked():
                if float(str(self.ui.mc2MinEdit.text())) < float(str(self.ui.mc2MaxEdit.text())):
                    pname = u"k2seq_%s"%(gnumber)
                    result += output.centerHeader(pname,14)
        return result



