# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from setMutationModelMsat import SetMutationModelMsat

class SetMutationModelMsatRefTable(SetMutationModelMsat):
    """ spécialisation de SetMutationModelMsat pour les genetic data 
    dans le cadre de la génération de la table de référence
    """
    def __init__(self,parent=None,box_group=None):
        super(SetMutationModelMsatRefTable,self).__init__(parent,box_group)

    def exit(self):
        ## reactivation des onglets
        #self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
        #self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        #self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))
        self.parent.parent.ui.refTableStack.removeWidget(self)
        self.parent.parent.ui.refTableStack.setCurrentWidget(self.parent)

    def clear(self):
        """ clic sur clear, remet les valeurs par défaut
        """
        self.parent.clearMutationModel(self.box_group)

    def validate(self):
        """ vérifie la validité des informations entrées dans le mutation model
        retourne au setGen tab et set la validité du mutation model du groupe
        """
        if self.allValid():
            self.exit()
            self.parent.setMutationValid_dico[self.box_group] = True
            self.parent.writeGeneticConfFromGui()


    def getParamTableHeader(self):
        """ retourne une chaine contenant les paramètres (valeurs qui varient)
        pour l'écriture du conf.th
        """
        result = u""
        gnumber = self.parent.groupList.index(self.box_group)+1
        if float(str(self.ui.mmrMinEdit.text())) < float(str(self.ui.mmrMaxEdit.text())):
            pname = u"µmic_%s"%(gnumber)
            result += pname
            for i in range(14-len(pname)):
                result += " "
        if float(str(self.ui.mcpMinEdit.text())) < float(str(self.ui.mcpMaxEdit.text())):
            pname = u"pmic_%s"%(gnumber)
            result += pname
            for i in range(14-len(pname)):
                result += " "
        if float(str(self.ui.msrMinEdit.text())) < float(str(self.ui.msrMaxEdit.text())):
            pname = u"snimic_%s"%(gnumber)
            result += pname
            for i in range(14-len(pname)):
                result += " "
        #print "result %s : %s"%(gnumber,result)
        return result



