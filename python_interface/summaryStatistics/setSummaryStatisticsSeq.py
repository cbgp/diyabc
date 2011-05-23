# -*- coding: utf-8 -*-

import sys
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from PyQt4 import uic
#from uis.setSummaryStatisticsSeq_ui import Ui_Frame
from setSummaryStatistics import SetSummaryStatistics
import output

formSetSummaryStatisticsSeq,baseSetSummaryStatisticsSeq = uic.loadUiType("uis/setSummaryStatisticsSeq.ui")

class SetSummaryStatisticsSeq(SetSummaryStatistics,formSetSummaryStatisticsSeq,baseSetSummaryStatisticsSeq):
    """ ecran de selection des summary statistics pour les sequences
    """
    def __init__(self,parent=None,box_group=None,numGroup=0):
        super(SetSummaryStatisticsSeq,self).__init__(parent,box_group,numGroup)

        self.statList = ["NHA","NSS","MPD","VPD","DTA","PSS","MNS","VNS","NH2","NS2","MP2","MPB","HST","SML"]

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.clearButton,SIGNAL("clicked()"),self.clear)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)

        # all buttons
        QObject.connect(self.ui.allNohButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allNossButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allMopdButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allVopdButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allTdButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allPssButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allMonotrButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allVonotrButton,SIGNAL("clicked()"),self.allPressed)
        # none buttons
        QObject.connect(self.ui.noneNohButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneNossButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneMopdButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneVopdButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneTdButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.nonePssButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneMonotrButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneVonotrButton,SIGNAL("clicked()"),self.allPressed)

        # all buttons 2
        QObject.connect(self.ui.allNoh2Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allNoss2Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allMopdw2Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allMopdb2Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allFst2Button,SIGNAL("clicked()"),self.allPressed)
        #none buttons 2
        QObject.connect(self.ui.noneNoh2Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneNoss2Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneMopdw2Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneMopdb2Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneFst2Button,SIGNAL("clicked()"),self.allPressed)
        # all and none for admixtures
        QObject.connect(self.ui.allMl3Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneMl3Button,SIGNAL("clicked()"),self.allPressed)

        QObject.connect(self.ui.addAdmixButton,SIGNAL("clicked()"),self.addAdmix)

        self.ui.horizontalLayout_4.setAlignment(Qt.AlignLeft)
        self.ui.horizontalLayout_2.setAlignment(Qt.AlignLeft)
        self.ui.horizontalLayout_5.setAlignment(Qt.AlignLeft)

        self.ui.frame_12.hide()
        self.ui.frame_11.hide()
        self.ui.frame_9.hide()

    def addOneSample(self,num):
        """ methode d'ajout d'un bloc dans 'one sample sum stats'
        """
        frame_9 = QtGui.QFrame(self.ui.scrollAreaWidgetContents)
        frame_9.setMinimumSize(QtCore.QSize(60, 0))
        frame_9.setMaximumSize(QtCore.QSize(60, 16777215))
        frame_9.setFrameShape(QtGui.QFrame.StyledPanel)
        frame_9.setFrameShadow(QtGui.QFrame.Raised)
        frame_9.setObjectName("frame_9")
        verticalLayout_4 = QtGui.QVBoxLayout(frame_9)
        verticalLayout_4.setSpacing(1)
        verticalLayout_4.setMargin(1)
        verticalLayout_4.setObjectName("verticalLayout_4")
        oneSampleLabel = QtGui.QLabel("Samp %i"%num,frame_9)
        oneSampleLabel.setMinimumSize(QtCore.QSize(0, 14))
        oneSampleLabel.setMaximumSize(QtCore.QSize(5000, 14))
        font = QtGui.QFont()
        font.setPointSize(8)
        oneSampleLabel.setFont(font)
        oneSampleLabel.setAlignment(QtCore.Qt.AlignCenter)
        oneSampleLabel.setObjectName("oneSampleLabel")
        verticalLayout_4.addWidget(oneSampleLabel)
        horizontalLayout_27 = QtGui.QHBoxLayout()
        horizontalLayout_27.setObjectName("horizontalLayout_27")
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_27.addItem(spacerItem)
        nohCheck = QtGui.QCheckBox(frame_9)
        nohCheck.setMinimumSize(QtCore.QSize(20, 14))
        nohCheck.setMaximumSize(QtCore.QSize(20, 14))
        nohCheck.setLayoutDirection(QtCore.Qt.LeftToRight)
        nohCheck.setObjectName("nohCheck")
        horizontalLayout_27.addWidget(nohCheck)
        spacerItem1 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_27.addItem(spacerItem1)
        verticalLayout_4.addLayout(horizontalLayout_27)
        horizontalLayout_11 = QtGui.QHBoxLayout()
        horizontalLayout_11.setObjectName("horizontalLayout_11")
        spacerItem2 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_11.addItem(spacerItem2)
        nossCheck = QtGui.QCheckBox(frame_9)
        nossCheck.setMinimumSize(QtCore.QSize(20, 14))
        nossCheck.setMaximumSize(QtCore.QSize(20, 14))
        nossCheck.setLayoutDirection(QtCore.Qt.LeftToRight)
        nossCheck.setObjectName("nossCheck")
        horizontalLayout_11.addWidget(nossCheck)
        spacerItem3 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_11.addItem(spacerItem3)
        verticalLayout_4.addLayout(horizontalLayout_11)
        horizontalLayout_6 = QtGui.QHBoxLayout()
        horizontalLayout_6.setObjectName("horizontalLayout_6")
        spacerItem4 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_6.addItem(spacerItem4)
        mopdCheck = QtGui.QCheckBox(frame_9)
        mopdCheck.setMinimumSize(QtCore.QSize(20, 14))
        mopdCheck.setMaximumSize(QtCore.QSize(20, 14))
        mopdCheck.setLayoutDirection(QtCore.Qt.LeftToRight)
        mopdCheck.setObjectName("mopdCheck")
        horizontalLayout_6.addWidget(mopdCheck)
        spacerItem5 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_6.addItem(spacerItem5)
        verticalLayout_4.addLayout(horizontalLayout_6)
        horizontalLayout_7 = QtGui.QHBoxLayout()
        horizontalLayout_7.setObjectName("horizontalLayout_7")
        spacerItem6 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_7.addItem(spacerItem6)
        vopdCheck = QtGui.QCheckBox(frame_9)
        vopdCheck.setMinimumSize(QtCore.QSize(20, 14))
        vopdCheck.setMaximumSize(QtCore.QSize(20, 14))
        vopdCheck.setObjectName("vopdCheck")
        horizontalLayout_7.addWidget(vopdCheck)
        spacerItem7 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_7.addItem(spacerItem7)
        verticalLayout_4.addLayout(horizontalLayout_7)
        horizontalLayout_8 = QtGui.QHBoxLayout()
        horizontalLayout_8.setObjectName("horizontalLayout_8")
        spacerItem8 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_8.addItem(spacerItem8)
        tdCheck = QtGui.QCheckBox(frame_9)
        tdCheck.setMinimumSize(QtCore.QSize(20, 14))
        tdCheck.setMaximumSize(QtCore.QSize(20, 14))
        tdCheck.setObjectName("tdCheck")
        horizontalLayout_8.addWidget(tdCheck)
        spacerItem9 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_8.addItem(spacerItem9)
        verticalLayout_4.addLayout(horizontalLayout_8)
        horizontalLayout_34 = QtGui.QHBoxLayout()
        horizontalLayout_34.setObjectName("horizontalLayout_34")
        spacerItem10 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_34.addItem(spacerItem10)
        pssCheck = QtGui.QCheckBox(frame_9)
        pssCheck.setMinimumSize(QtCore.QSize(20, 14))
        pssCheck.setMaximumSize(QtCore.QSize(20, 14))
        pssCheck.setObjectName("pssCheck")
        horizontalLayout_34.addWidget(pssCheck)
        spacerItem11 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_34.addItem(spacerItem11)
        verticalLayout_4.addLayout(horizontalLayout_34)
        horizontalLayout_10 = QtGui.QHBoxLayout()
        horizontalLayout_10.setObjectName("horizontalLayout_10")
        spacerItem12 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_10.addItem(spacerItem12)
        monotrCheck = QtGui.QCheckBox(frame_9)
        monotrCheck.setMinimumSize(QtCore.QSize(20, 28))
        monotrCheck.setMaximumSize(QtCore.QSize(20, 28))
        monotrCheck.setObjectName("monotrCheck")
        horizontalLayout_10.addWidget(monotrCheck)
        spacerItem13 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_10.addItem(spacerItem13)
        verticalLayout_4.addLayout(horizontalLayout_10)
        horizontalLayout_28 = QtGui.QHBoxLayout()
        horizontalLayout_28.setObjectName("horizontalLayout_28")
        spacerItem14 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_28.addItem(spacerItem14)
        vonotrCheck = QtGui.QCheckBox(frame_9)
        vonotrCheck.setMinimumSize(QtCore.QSize(20, 28))
        vonotrCheck.setMaximumSize(QtCore.QSize(20, 28))
        vonotrCheck.setObjectName("vonotrCheck")
        horizontalLayout_28.addWidget(vonotrCheck)
        spacerItem15 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_28.addItem(spacerItem15)
        verticalLayout_4.addLayout(horizontalLayout_28)
        self.ui.horizontalLayout_4.addWidget(frame_9)

        self.oneSampleList.append(frame_9)

    def addTwoSample(self,num1,num2):
        """ methode d'ajout d'un bloc dans 'two samples sum stats'
        """
        frame_11 = QtGui.QFrame(self.ui.scrollAreaWidgetContents_2)
        frame_11.setMinimumSize(QtCore.QSize(70, 0))
        frame_11.setMaximumSize(QtCore.QSize(70, 16777215))
        frame_11.setFrameShape(QtGui.QFrame.StyledPanel)
        frame_11.setFrameShadow(QtGui.QFrame.Raised)
        frame_11.setObjectName("frame_11")
        verticalLayout_5 = QtGui.QVBoxLayout(frame_11)
        verticalLayout_5.setSpacing(1)
        verticalLayout_5.setMargin(1)
        verticalLayout_5.setObjectName("verticalLayout_5")
        twoSampleLabel = QtGui.QLabel("Samp %i&%i"%(num1,num2),frame_11)
        twoSampleLabel.setMinimumSize(QtCore.QSize(0, 14))
        twoSampleLabel.setMaximumSize(QtCore.QSize(16777215, 14))
        font = QtGui.QFont()
        font.setPointSize(8)
        twoSampleLabel.setFont(font)
        twoSampleLabel.setObjectName("twoSampleLabel")
        verticalLayout_5.addWidget(twoSampleLabel)
        horizontalLayout_16 = QtGui.QHBoxLayout()
        horizontalLayout_16.setObjectName("horizontalLayout_16")
        spacerItem16 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_16.addItem(spacerItem16)
        noh2Check = QtGui.QCheckBox(frame_11)
        noh2Check.setMinimumSize(QtCore.QSize(20, 14))
        noh2Check.setMaximumSize(QtCore.QSize(20, 14))
        noh2Check.setObjectName("noh2Check")
        horizontalLayout_16.addWidget(noh2Check)
        spacerItem17 = QtGui.QSpacerItem(17, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_16.addItem(spacerItem17)
        verticalLayout_5.addLayout(horizontalLayout_16)
        horizontalLayout_15 = QtGui.QHBoxLayout()
        horizontalLayout_15.setObjectName("horizontalLayout_15")
        spacerItem18 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_15.addItem(spacerItem18)
        noss2Check = QtGui.QCheckBox(frame_11)
        noss2Check.setMinimumSize(QtCore.QSize(20, 14))
        noss2Check.setMaximumSize(QtCore.QSize(20, 14))
        noss2Check.setObjectName("noss2Check")
        horizontalLayout_15.addWidget(noss2Check)
        spacerItem19 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_15.addItem(spacerItem19)
        verticalLayout_5.addLayout(horizontalLayout_15)
        horizontalLayout_14 = QtGui.QHBoxLayout()
        horizontalLayout_14.setObjectName("horizontalLayout_14")
        spacerItem20 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_14.addItem(spacerItem20)
        mopdw2Check = QtGui.QCheckBox(frame_11)
        mopdw2Check.setMinimumSize(QtCore.QSize(20, 14))
        mopdw2Check.setMaximumSize(QtCore.QSize(20, 14))
        mopdw2Check.setObjectName("mopdw2Check")
        horizontalLayout_14.addWidget(mopdw2Check)
        spacerItem21 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_14.addItem(spacerItem21)
        verticalLayout_5.addLayout(horizontalLayout_14)
        horizontalLayout_13 = QtGui.QHBoxLayout()
        horizontalLayout_13.setObjectName("horizontalLayout_13")
        spacerItem22 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_13.addItem(spacerItem22)
        mopdb2Check = QtGui.QCheckBox(frame_11)
        mopdb2Check.setMinimumSize(QtCore.QSize(20, 14))
        mopdb2Check.setMaximumSize(QtCore.QSize(20, 14))
        mopdb2Check.setObjectName("mopdb2Check")
        horizontalLayout_13.addWidget(mopdb2Check)
        spacerItem23 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_13.addItem(spacerItem23)
        verticalLayout_5.addLayout(horizontalLayout_13)
        horizontalLayout_12 = QtGui.QHBoxLayout()
        horizontalLayout_12.setObjectName("horizontalLayout_12")
        spacerItem24 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_12.addItem(spacerItem24)
        fst2Check = QtGui.QCheckBox(frame_11)
        fst2Check.setMinimumSize(QtCore.QSize(20, 14))
        fst2Check.setMaximumSize(QtCore.QSize(20, 14))
        fst2Check.setObjectName("fst2Check")
        horizontalLayout_12.addWidget(fst2Check)
        spacerItem25 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_12.addItem(spacerItem25)
        verticalLayout_5.addLayout(horizontalLayout_12)
        self.ui.horizontalLayout_5.addWidget(frame_11)

        self.twoSampleList.append(frame_11)

    def getStats(self):
        """ retourne les stats sous la forme d'un dico indexé par les noms de ligne
        """
        nstat = 0
        # dico indexé par les noms de ligne (NAL,FST...) qui donne la liste des samples concernés
        dico_stats = {}
        dico_stats["NHA"] = []
        dico_stats["NSS"] = []
        dico_stats["MPD"] = []
        dico_stats["VPD"] = []
        dico_stats["DTA"] = []
        dico_stats["PSS"] = []
        dico_stats["MNS"] = []
        dico_stats["VNS"] = []

        dico_stats["NH2"] = []
        dico_stats["NS2"] = []
        dico_stats["MP2"] = []
        dico_stats["MPB"] = []
        dico_stats["HST"] = []

        dico_stats["SML"] = []
        for box in self.oneSampleList:
            lab = str(box.findChild(QLabel,"oneSampleLabel").text()).split(' ')[1]
            if box.findChild(QCheckBox,"nohCheck").isChecked():
                dico_stats["NHA"].append(lab)
                nstat += 1
            if box.findChild(QCheckBox,"nossCheck").isChecked():
                dico_stats["NSS"].append(lab)
                nstat += 1
            if box.findChild(QCheckBox,"mopdCheck").isChecked():
                dico_stats["MPD"].append(lab)
                nstat += 1
            if box.findChild(QCheckBox,"vopdCheck").isChecked():
                dico_stats["VPD"].append(lab)
                nstat += 1
            if box.findChild(QCheckBox,"tdCheck").isChecked():
                dico_stats["DTA"].append(lab)
                nstat += 1
            if box.findChild(QCheckBox,"pssCheck").isChecked():
                dico_stats["PSS"].append(lab)
                nstat += 1
            if box.findChild(QCheckBox,"monotrCheck").isChecked():
                dico_stats["MNS"].append(lab)
                nstat += 1
            if box.findChild(QCheckBox,"vonotrCheck").isChecked():
                dico_stats["VNS"].append(lab)
                nstat += 1
        for box in self.twoSampleList:
            lab = str(box.findChild(QLabel,"twoSampleLabel").text()).split(' ')[1]
            if box.findChild(QCheckBox,"noh2Check").isChecked():
                dico_stats["NH2"].append(lab)
                nstat += 1
            if box.findChild(QCheckBox,"noss2Check").isChecked():
                dico_stats["NS2"].append(lab)
                nstat += 1
            if box.findChild(QCheckBox,"mopdw2Check").isChecked():
                dico_stats["MP2"].append(lab)
                nstat += 1
            if box.findChild(QCheckBox,"mopdb2Check").isChecked():
                dico_stats["MPB"].append(lab)
                nstat += 1
            if box.findChild(QCheckBox,"fst2Check").isChecked():
                dico_stats["HST"].append(lab)
                nstat += 1
        for box in self.admixSampleList:
            lab = str(box.findChild(QLabel,"threeSampleLabel").text()).split('\n')[1]
            if box.findChild(QCheckBox,"ml3Check").isChecked():
                dico_stats["SML"].append(lab)
                nstat +=1
        return (nstat,dico_stats)

    def setSumConf(self,lines):
        """ grace aux lignes du fichier de conf, remet les sum stats
        """
        confToStat = {"NHA" : "noh","NSS" : "noss", "MPD": "mopd","VPD" : "vopd", "DTA" : "td", "PSS" : "pss", "MNS" : "monotr",\
                "VNS" : "vonotr", "NH2" : "noh2", "NS2" : "noss2", "MP2" : "mopdw2", "MPB" : "mopdb2", "HST" : "fst2", "SML" : "ml3" }

        # construction du dico de stats (le même que dans getSumConf)
        dico_stats = {}
        for line in lines:
            if line.strip():
                t = line.split(' ')[0]
                dico_stats[t] = []
                for sample in line.split(' ')[1:]:
                    dico_stats[t].append(sample)
        #print "dico stats :",dico_stats

        # pour chaque ligne (de case à cocher)
        for k in dico_stats.keys():
            name_chk_box = confToStat[k]
            # pour chaque colonne chaque sample dans one Sample
            if k in ["NHA","NSS","MPD","VPD","DTA","PSS","MNS","VNS"]:
                for box in self.oneSampleList:
                    num_sample = str(box.findChild(QLabel,"oneSampleLabel").text()).split(' ')[1].strip()
                    if num_sample in dico_stats[k]:
                        # on coche
                        box.findChild(QCheckBox,"%sCheck"%name_chk_box).setChecked(True)
            elif k in ["NH2","NS2","MP2","MPB","HST"]:
                for box in self.twoSampleList:
                    num_sample = str(box.findChild(QLabel,"twoSampleLabel").text()).split(' ')[1].strip()
                    if num_sample in dico_stats[k]:
                        # on coche
                        box.findChild(QCheckBox,"%sCheck"%name_chk_box).setChecked(True)
            elif k == "SML":
                for aml in dico_stats[k]:
                    num1 = aml.strip().split('&')[0]
                    num2 = aml.strip().split('&')[1]
                    num3 = aml.strip().split('&')[2]
                    self.addAdmixSampleGui(int(num1),int(num2),int(num3))
                    self.admixSampleList[-1].findChild(QCheckBox,"%sCheck"%name_chk_box).setChecked(True)

    def clear(self):
        self.parent.clearSummaryStatsSeq(self.box_group)

    def getSumStatsTableHeader(self):
        result = u""
        if self.numGroup == 0:
            gnumber = self.parent.groupList.index(self.box_group)+1
        else:
            gnumber = self.numGroup
        dico = {"NHA":[],"NSS":[],"MPD":[],"VPD":[],"DTA":[],"PSS":[],"MNS":[],"VNS":[],"NH2":[],"NS2":[],"MP2":[],"MPB":[],"HST":[],"SML":[]}
        for box in self.oneSampleList:
            lab = str(box.findChild(QLabel,"oneSampleLabel").text()).split(' ')[1]
            if box.findChild(QCheckBox,"nohCheck").isChecked():
                stat_name = "NHA_%s_%s"%(gnumber,lab)
                dico["NHA"].append(stat_name)
            if box.findChild(QCheckBox,"nossCheck").isChecked():
                stat_name = "NSS_%s_%s"%(gnumber,lab)
                dico["NSS"].append(stat_name)
            if box.findChild(QCheckBox,"mopdCheck").isChecked():
                stat_name = "MPD_%s_%s"%(gnumber,lab)
                dico["MPD"].append(stat_name)
            if box.findChild(QCheckBox,"vopdCheck").isChecked():
                stat_name = "VPD_%s_%s"%(gnumber,lab)
                dico["VPD"].append(stat_name)
            if box.findChild(QCheckBox,"tdCheck").isChecked():
                stat_name = "DTA_%s_%s"%(gnumber,lab)
                dico["DTA"].append(stat_name)
            if box.findChild(QCheckBox,"pssCheck").isChecked():
                stat_name = "PSS_%s_%s"%(gnumber,lab)
                dico["PSS"].append(stat_name)
            if box.findChild(QCheckBox,"monotrCheck").isChecked():
                stat_name = "MNS_%s_%s"%(gnumber,lab)
                dico["MNS"].append(stat_name)
            if box.findChild(QCheckBox,"vonotrCheck").isChecked():
                stat_name = "VNS_%s_%s"%(gnumber,lab)
                dico["VNS"].append(stat_name)
        for box in self.twoSampleList:
            lab = str(box.findChild(QLabel,"twoSampleLabel").text()).split(' ')[1]
            if box.findChild(QCheckBox,"noh2Check").isChecked():
                stat_name = "NH2_%s_%s"%(gnumber,lab)
                dico["NH2"].append(stat_name)
            if box.findChild(QCheckBox,"noss2Check").isChecked():
                stat_name = "NS2_%s_%s"%(gnumber,lab)
                dico["NS2"].append(stat_name)
            if box.findChild(QCheckBox,"mopdw2Check").isChecked():
                stat_name = "MP2_%s_%s"%(gnumber,lab)
                dico["MP2"].append(stat_name)
            if box.findChild(QCheckBox,"mopdb2Check").isChecked():
                stat_name = "MPB_%s_%s"%(gnumber,lab)
                dico["MPB"].append(stat_name)
            if box.findChild(QCheckBox,"fst2Check").isChecked():
                stat_name = "HST_%s_%s"%(gnumber,lab)
                dico["HST"].append(stat_name)
        for box in self.admixSampleList:
            lab = str(box.findChild(QLabel,"threeSampleLabel").text()).split('\n')[1]
            if box.findChild(QCheckBox,"ml3Check").isChecked():
                stat_name = "SML_%s_%s"%(gnumber,lab)
                dico["SML"].append(stat_name)

        for t in ["NHA","NSS","MPD","VPD","DTA","PSS","MNS","VNS","NH2","NS2","MP2","MPB","HST","SML"]:
            l = dico[t]
            for s in l:
                result += output.centerHeader(s,14)
        return result
