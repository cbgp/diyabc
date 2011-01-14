# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from set_condition_ui import Ui_MainWindow

class setCondition(QMainWindow):
    """ Fenetre de definition d'une condition
    """
    def __init__(self,param,target_list,parent=None):
        super(setCondition,self).__init__(parent)
        self.parent=parent
        self.param = param
        self.target_list = target_list
        self.createWidgets()

    def createWidgets(self):
        self.ui = Ui_MainWindow()
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
        self.parent.addCondition("%s %s %s"%(self.param,op,selected_target))
        self.close()