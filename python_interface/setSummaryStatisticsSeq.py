# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from setSummaryStatisticsSeq_ui import Ui_Frame

class SetSummaryStatisticsSeq(QFrame):
    def __init__(self,parent=None):
        super(SetSummaryStatisticsSeq,self).__init__(parent)
        self.parent=parent
        self.oneSampleList = []
        self.twoSampleList = []
        self.admixSampleList = []

        self.createWidgets()

        # remplissage des sample
        for i in range(self.parent.parent.data.nsample):
            self.addOneSample(i+1)

        if self.parent.parent.data.nsample >= 2:
            for i in range(self.parent.parent.data.nsample):
                j = i+1
                while j < self.parent.parent.data.nsample:
                    self.addTwoSample(i+1,j+1)
                    j+=1
            if self.parent.parent.data.nsample >= 3:
                pass
            else:
                self.ui.admixScroll.hide()
        else:
            self.ui.twoScroll.hide()
            self.ui.admixScroll.hide()

        for i in range(self.parent.parent.data.nsample):
            self.ui.adm1Combo.addItem(str(i+1))
            self.ui.adm2Combo.addItem(str(i+1))
            self.ui.adm3Combo.addItem(str(i+1))

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)

        ## all buttons
        #QObject.connect(self.ui.allMnoaButton,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.allMgdButton,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.allMsvButton,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.allMgwButton,SIGNAL("clicked()"),self.allPressed)
        ## none buttons
        #QObject.connect(self.ui.noneMnoaButton,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.noneMgdButton,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.noneMsvButton,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.noneMgwButton,SIGNAL("clicked()"),self.allPressed)

        ## all buttons 2
        #QObject.connect(self.ui.allMnoa2Button,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.allMgd2Button,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.allMsv2Button,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.allFst2Button,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.allCi2Button,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.allSad2Button,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.allDd2Button,SIGNAL("clicked()"),self.allPressed)
        ##none buttons 2
        #QObject.connect(self.ui.noneMnoa2Button,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.noneMgd2Button,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.noneMsv2Button,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.noneFst2Button,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.noneCi2Button,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.noneSad2Button,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.noneDd2Button,SIGNAL("clicked()"),self.allPressed)
        ## all and none for admixtures
        #QObject.connect(self.ui.allMl3Button,SIGNAL("clicked()"),self.allPressed)
        #QObject.connect(self.ui.noneMl3Button,SIGNAL("clicked()"),self.allPressed)

        QObject.connect(self.ui.addAdmixButton,SIGNAL("clicked()"),self.addAdmix)

        self.ui.horizontalLayout_4.setAlignment(Qt.AlignLeft)
        self.ui.horizontalLayout_2.setAlignment(Qt.AlignLeft)
        self.ui.horizontalLayout_5.setAlignment(Qt.AlignLeft)

        #self.ui.frame_12.hide()
        #self.ui.frame_11.hide()
        #self.ui.frame_9.hide()

    def addOneSample(self,num):
        """ methode d'ajout d'un bloc dans 'one sample sum stats'
        """
        pass

        #self.oneSampleList.append(frame_9)

    def addTwoSample(self,num1,num2):
        pass

        #self.twoSampleList.append(frame_11)

    def allPressed(self):
        button_name = str(self.sender().objectName())
        checkname = button_name.replace('Button','').replace('all','').replace('none','').lower()
        if checkname == "ci2":
            self.checkAll(('all' in button_name),"%sRightCheck"%checkname)
            self.checkAll(('all' in button_name),"%sLeftCheck"%checkname)
        else:
            self.checkAll(('all' in button_name),"%sCheck"%checkname)

    def checkAll(self,yesno,objname):
        for chkbox in self.findChildren(QCheckBox,objname):
            chkbox.setChecked(yesno)

    def addAdmixSampleGui(self,num1,num2,num3):
        frame_12 = QtGui.QFrame(self.ui.scrollAreaWidgetContents_3)
        frame_12.setMinimumSize(QtCore.QSize(55, 0))
        frame_12.setMaximumSize(QtCore.QSize(55, 16777215))
        frame_12.setFrameShape(QtGui.QFrame.StyledPanel)
        frame_12.setFrameShadow(QtGui.QFrame.Raised)
        frame_12.setObjectName("frame_12")
        verticalLayout_6 = QtGui.QVBoxLayout(frame_12)
        verticalLayout_6.setSpacing(1)
        verticalLayout_6.setAlignment(Qt.AlignHCenter)
        verticalLayout_6.setContentsMargins(1, 1, 1, 1)
        verticalLayout_6.setObjectName("verticalLayout_6")
        threeSampleLabel = QtGui.QLabel("Samp\n%i&%i&%i"%(num1,num2,num3),frame_12)
        font = QtGui.QFont()
        font.setPointSize(8)
        threeSampleLabel.setFont(font)
        threeSampleLabel.setObjectName("threeSampleLabel")
        verticalLayout_6.addWidget(threeSampleLabel)
        rmButton = QtGui.QPushButton("remove",frame_12)
        font = QtGui.QFont()
        font.setPointSize(8)
        rmButton.setFont(font)
        rmButton.setObjectName("rmButton")
        verticalLayout_6.addWidget(rmButton)
        horizontalLayout_17 = QtGui.QHBoxLayout()
        horizontalLayout_17.setObjectName("horizontalLayout_17")
        spacerItem20 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_17.addItem(spacerItem20)
        ml3Check = QtGui.QCheckBox(frame_12)
        ml3Check.setMinimumSize(QtCore.QSize(20, 0))
        ml3Check.setMaximumSize(QtCore.QSize(20, 16777215))
        ml3Check.setObjectName("ml3Check")
        horizontalLayout_17.addWidget(ml3Check)
        spacerItem21 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_17.addItem(spacerItem21)
        verticalLayout_6.addLayout(horizontalLayout_17)
        self.ui.horizontalLayout_2.addWidget(frame_12)

        self.admixSampleList.append(frame_12)

        QObject.connect(rmButton,SIGNAL("clicked()"),self.rmAdmix)

    def rmAdmix(self):
        box = self.sender().parent()
        box.hide()
        self.admixSampleList.remove(box)

    def addAdmix(self):
        val1 = str(self.ui.adm1Combo.currentText())
        val2 = str(self.ui.adm2Combo.currentText())
        val3 = str(self.ui.adm3Combo.currentText())
        # verif de la coherence
        if val1 == val2 or val2 == val3 or val3 == val1:
            QMessageBox.information(self,"Value error","Each value must be different to add an admixture summary statistic")
        else:
            # verif qu'il n'existe pas d'équivalent
            ok = True
            for box in self.admixSampleList:
                txt = str(box.findChild(QLabel,"threeSampleLabel").text()).replace('Samp\n','')
                valbox = (txt.split('&')[0],txt.split('&')[1],txt.split('&')[2])
                if valbox == (val1,val2,val3):
                    ok = False
                    break
            if not ok:
                QMessageBox.information(self,"Value error","This admixture summary statistic is already set")
            else:
                self.addAdmixSampleGui(int(val1),int(val2),int(val3))



    def exit(self):
        # reactivation des onglets
        self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
        self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))



