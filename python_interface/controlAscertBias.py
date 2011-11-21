# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
from utils.visualizescenario import *
from utils.data import *
import output
from utils.cbgpUtils import log
import codecs

formControl,baseControl = uic.loadUiType("uis/controlAscertBias.ui")

class ControlAscertBias(formControl,baseControl):
    def __init__(self,parent=None):
        super(ControlAscertBias,self).__init__(parent)
        self.parent=parent

        self.createWidgets()

    def createWidgets(self):
        #self.ui = Ui_Frame()
        self.ui = self
        self.ui.setupUi(self)

        self.ui.verticalLayout_3.setAlignment(QtCore.Qt.AlignTop)
        self.ui.tableWidget.setColumnWidth(0,200)
        self.ui.tableWidget.setColumnWidth(1,200)

        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.clearButton,SIGNAL("clicked()"),self.clear)

    def fillSampleFromData(self,nind):
        if self.ui.tableWidget.rowCount() == 0:
            lnames = nind.keys()
            lnames.sort()
            for i in lnames:
                self.addRow(name="%s"%(i))

    def addRow(self,name="locus",val="0"):
        self.ui.tableWidget.insertRow(self.ui.tableWidget.rowCount())
        #self.ui.tableWidget.setCellWidget(self.ui.tableWidget.rowCount()-1,i,QPushButton("View"))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,0,QTableWidgetItem("%s"%(name)))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,1,QTableWidgetItem("%s"%val))
        self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,0).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,0).flags() & ~Qt.ItemIsEditable)
        #self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,1).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,1).flags() & ~Qt.ItemIsEditable)

    def loadAscertConf(self):
        if os.path.exists(self.parent.dir+"/%s"%self.parent.parent.ascertainment_conf_name):
            f = codecs.open(self.parent.dir+"/%s"%self.parent.parent.ascertainment_conf_name,'r',"utf-8")
            lines = f.readlines()
            f.close()
            if len(lines) >= 3:
                for i,n in enumerate(lines[0].strip().split(' ')):
                    self.ui.tableWidget.setItem(i,1,QTableWidgetItem("%s"%(n)))
                self.ui.freqMinEdit.setText(lines[1].strip())
                if lines[2].strip() == "N":
                    self.ui.nincRadio.setChecked(True)
                else:
                    self.ui.incRadio.setChecked(True)

    def validate(self,silent=False):
        # verif
        problem = ""
        for i in range(self.ui.tableWidget.rowCount()):
            try:
                popname = str(self.ui.tableWidget.item(i,0).text())
                val = int(self.ui.tableWidget.item(i,1).text())
                if val < 0 :
                    raise Exception("Number of individuals must be positive")
                if val > self.parent.data.nind[popname]:
                    raise Exception("Number of individuals must be inferior to %s"%self.parent.data.nind[popname])
            except Exception,e:
                problem += "Sample %s : %s\n"%(i+1,e)
        try:
            freq = float(self.ui.freqMinEdit.text())
            if freq < 0:
                raise Exception("Frequency must be positive")
        except Exception,e:
            problem += "Frequency : %s\n"%(e)
        if problem != "":
            if not silent:
                output.notify(self,"Value error",problem)
            self.parent.setAscertValid(False)
        else:
            self.parent.setAscertValid(True)
            #sortie
            self.exit()

    def clear(self):
        for i in range(self.ui.tableWidget.rowCount()):
            self.ui.tableWidget.setItem(i,1,QTableWidgetItem("0"))

        self.freqMinEdit.setText("0")
        self.nincRadio.setChecked(True)

    def exit(self):
        self.parent.ui.refTableStack.removeWidget(self)
        self.parent.ui.refTableStack.setCurrentIndex(0)

