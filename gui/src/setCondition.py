# -*- coding: utf-8 -*-

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.set_condition_ui import Ui_MainWindow

formSetCondition,baseSetCondition= uic.loadUiType("uis/setCondition.ui")

class SetCondition(formSetCondition,baseSetCondition):
    """ Fenetre de definition d'une condition
    """
    def __init__(self,param,target_list,parent=None):
        super(SetCondition,self).__init__(parent)
        self.parent=parent
        self.param = param
        self.target_list = target_list
        self.createWidgets()

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)
        self.ui.paramLabel.setText(self.param)
        for t in self.target_list:
            self.ui.targetComboBox.addItem(t)
        QObject.connect(self.ui.cancelButton,SIGNAL("clicked()"),self.close)
        QObject.connect(self.ui.validateButton,SIGNAL("clicked()"),self.valid)

    def valid(self):
        """ clic sur le bouton de validation qui entraine l'affichage de la condition dans setHistoricalModel
        """
        if self.ui.infRadio.isChecked():
            op = '<'
        elif self.ui.infERadio.isChecked():
            op = '<='
        elif self.ui.supERadio.isChecked():
            op = '>='
        else:
            op = '>'
        selected_target = self.ui.targetComboBox.currentText()
        my_condition = "%s%s%s"%(self.param,op,selected_target)
        inverseur = {"<":">=",">":"<=",">=":"<","<=":">"}
        mirroir = {"<=":">=",">=":"<=",">":"<","<":">"}
        incoherent = {"<=":"<",">=":">",">":">=","<":"<="}
        pleft = my_condition.split(op)[0]
        pright = my_condition.split(op)[1]
        # pour a > b : b < a ; cad la meme ecrite a l'envers
        my_condition_mirroir = "%s%s%s"%(pright,mirroir[op],pleft)
        # pour a > b : opposée vaut a <= b
        my_condition_opposee = "%s%s%s"%(pleft,inverseur[op],pright)
        # pour a > b : opp_mirroir vaut b >= a
        my_condition_mirroir_opposee = "%s%s%s"%(pright,mirroir[inverseur[op]],pleft)
        # pour a > b : a < b
        my_condition_incompatible = "%s%s%s"%(pleft,mirroir[op],pright)
        # pour a > b : b > a
        my_condition_incompatible_mirroir = "%s%s%s"%(pright,op,pleft)
        # pour a > b : a >= b
        my_condition_incoherent = "%s%s%s"%(pleft,incoherent[op],pright)
        # pour a > b : b <= a
        my_condition_incoherent_mirroir = "%s%s%s"%(pright,mirroir[incoherent[op]],pleft)
        for cond_box in self.parent.condList:
            #print "chaque condbox"
            econdition = str(cond_box.findChild(QLabel,"condLabel").text())
            if my_condition == econdition or econdition == my_condition_mirroir:
                QMessageBox.information(self,"Error","This condition (%s) already exists (%s)"%(my_condition,econdition))
                return 0
            elif econdition == my_condition_opposee or econdition == my_condition_mirroir_opposee:
                QMessageBox.information(self,"Error","The opposite of your new condition (%s) already exists (%s)"%(my_condition,econdition))
                return 0
            elif econdition == my_condition_incompatible or econdition == my_condition_incompatible_mirroir:
                QMessageBox.information(self,"Error","Your new condition (%s) is incompatible with an existing one (%s)"%(my_condition,econdition))
                return 0
            elif econdition == my_condition_incoherent or econdition == my_condition_incoherent_mirroir:
                QMessageBox.information(self,"Error","Your new condition (%s) is incoherent with an existing one (%s)"%(my_condition,econdition))
                return 0

        self.parent.addCondition("%s%s%s"%(self.param,op,selected_target))
        self.close()
