# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from setSummaryStatistics_ui import Ui_Frame

class SetSummaryStatistics(QFrame):
    def __init__(self,parent=None):
        super(SetSummaryStatistics,self).__init__(parent)
        self.parent=parent
        self.createWidgets()

        #for i in range(self.parent.parent.data.nsample):
        for i in range(100):
            self.addOneSample(i+1)
        #if self.parent.parent.data.nsample >= 2:
        #    for i in range(self.parent.parent.data.nsample):
        #        j = i+1
        #        while j < self.parent.parent.data.nsample:
        #            self.addTwoSample(i+1,j+1)
        #            j+=1
        #    if self.parent.parent.data.nsample >= 3:


    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)

        self.ui.horizontalLayout_4.setAlignment(Qt.AlignLeft)
        self.ui.horizontalLayout_2.setAlignment(Qt.AlignLeft)
        self.ui.horizontalLayout_5.setAlignment(Qt.AlignLeft)

    def addOneSample(self,num):
        """ methode d'ajout d'un bloc dans 'one sample sum stats'
        """
        frame_9 = QtGui.QFrame(self.ui.scrollAreaWidgetContents)
        frame_9.setMinimumSize(QtCore.QSize(100, 0))
        frame_9.setMaximumSize(QtCore.QSize(100, 16777215))
        frame_9.setFrameShape(QtGui.QFrame.StyledPanel)
        frame_9.setFrameShadow(QtGui.QFrame.Raised)
        frame_9.setObjectName("frame_9")
        verticalLayout_4 = QtGui.QVBoxLayout(frame_9)
        verticalLayout_4.setObjectName("verticalLayout_4")
        verticalLayout_4.setSpacing(1)
        verticalLayout_4.setContentsMargins(-1, 1, -1, 1)
        oneSampleLabel = QtGui.QLabel("Sample %i"%num,frame_9)
        oneSampleLabel.setAlignment(QtCore.Qt.AlignCenter)
        oneSampleLabel.setObjectName("oneSampleLabel")
        verticalLayout_4.addWidget(oneSampleLabel)
        horizontalLayout_3 = QtGui.QHBoxLayout()
        horizontalLayout_3.setObjectName("horizontalLayout_3")
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_3.addItem(spacerItem)
        mnoaCheck = QtGui.QCheckBox(frame_9)
        mnoaCheck.setMinimumSize(QtCore.QSize(20, 0))
        mnoaCheck.setMaximumSize(QtCore.QSize(20, 16777215))
        mnoaCheck.setLayoutDirection(QtCore.Qt.LeftToRight)
        mnoaCheck.setObjectName("mnoaCheck")
        horizontalLayout_3.addWidget(mnoaCheck)
        spacerItem1 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_3.addItem(spacerItem1)
        verticalLayout_4.addLayout(horizontalLayout_3)
        horizontalLayout_7 = QtGui.QHBoxLayout()
        horizontalLayout_7.setObjectName("horizontalLayout_7")
        spacerItem2 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_7.addItem(spacerItem2)
        mgdCheck = QtGui.QCheckBox(frame_9)
        mgdCheck.setMinimumSize(QtCore.QSize(20, 0))
        mgdCheck.setMaximumSize(QtCore.QSize(20, 16777215))
        mgdCheck.setObjectName("mgdCheck")
        horizontalLayout_7.addWidget(mgdCheck)
        spacerItem3 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_7.addItem(spacerItem3)
        verticalLayout_4.addLayout(horizontalLayout_7)
        horizontalLayout_8 = QtGui.QHBoxLayout()
        horizontalLayout_8.setObjectName("horizontalLayout_8")
        spacerItem4 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_8.addItem(spacerItem4)
        msvCheck = QtGui.QCheckBox(frame_9)
        msvCheck.setMinimumSize(QtCore.QSize(20, 0))
        msvCheck.setMaximumSize(QtCore.QSize(20, 16777215))
        msvCheck.setObjectName("msvCheck")
        horizontalLayout_8.addWidget(msvCheck)
        spacerItem5 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_8.addItem(spacerItem5)
        verticalLayout_4.addLayout(horizontalLayout_8)
        horizontalLayout_10 = QtGui.QHBoxLayout()
        horizontalLayout_10.setObjectName("horizontalLayout_10")
        spacerItem6 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_10.addItem(spacerItem6)
        mgwCheck = QtGui.QCheckBox(frame_9)
        mgwCheck.setMinimumSize(QtCore.QSize(20, 0))
        mgwCheck.setMaximumSize(QtCore.QSize(20, 16777215))
        mgwCheck.setObjectName("mgwCheck")
        horizontalLayout_10.addWidget(mgwCheck)
        spacerItem7 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_10.addItem(spacerItem7)
        verticalLayout_4.addLayout(horizontalLayout_10)
        self.ui.horizontalLayout_4.addWidget(frame_9)

    def exit(self):
        # reactivation des onglets
        self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
        self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))



