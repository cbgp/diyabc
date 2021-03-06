# -*- coding: utf-8 -*-

import sys, codecs
import time
import os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from PyQt4 import uic
#from uis.setSummaryStatisticsMsat_ui import Ui_Frame
from setSummaryStatistics import SetSummaryStatistics
import output
import variables
from variables import UIPATH
from utils.cbgpUtils import getFsEncoding

formSetSummaryStatisticsSnp,baseSetSummaryStatisticsSnp = uic.loadUiType((u"%s/setSummaryStatisticsSnp.ui"%UIPATH).encode(getFsEncoding(logLevel=False)))

class SetSummaryStatisticsSnp(SetSummaryStatistics,formSetSummaryStatisticsSnp,baseSetSummaryStatisticsSnp):
    """ ecran de selection des summary statistics pour les SNP
    """
    def __init__(self,parent=None,box_group=None,numGroup=0):
        self.statList1 = ["HP0","HM1","HV1","HMO"]
        self.confToStat1 = {
                "HP0" : "mgd",
                "HM1" : "vgd",
                "HV1": "fgd",
                "HMO" : "mod"}
        self.statList2 = ["FP0","FM1","FV1","FMO","NP0","NM1","NV1","NMO"]
        self.confToStat2 = {
                "NP0" : "mnd2",
                "NM1": "vnd2",
                "NV1" : "fnd2",
                "NMO" : "modnd2",
                "FP0" : "mfd2",
                "FM1" : "vfd2",
                "FV1" : "ffd2",
                "FMO" : "modfd2"}
        self.statList3 = ["AP0","AM1","AV1","AMO"]
        self.confToStat3 = {
                "AP0" : "mae3",
                "AM1" : "vae3",
                "AV1" : "fae3",
                "AMO" : "mod3"}
        self.statList = self.statList1 + self.statList2 + self.statList3
        super(SetSummaryStatisticsSnp,self).__init__(parent,box_group,numGroup)

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.clearButton,SIGNAL("clicked()"),self.clear)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)

        # all buttons
        for dicoConf in [self.confToStat1,self.confToStat2,self.confToStat3]:
            for stat in dicoConf.keys():
                name = dicoConf[stat]
                exec('QObject.connect(self.ui.all%s%sButton,SIGNAL("clicked()"),self.allPressed)'%(name[0].upper(),name[1:]))
                exec('QObject.connect(self.ui.none%s%sButton,SIGNAL("clicked()"),self.allPressed)'%(name[0].upper(),name[1:]))

        QObject.connect(self.ui.addAdmixButton,SIGNAL("clicked()"),self.addAdmix)

        self.ui.horizontalLayout_4.setAlignment(Qt.AlignLeft)
        self.ui.horizontalLayout_2.setAlignment(Qt.AlignLeft)
        self.ui.horizontalLayout_5.setAlignment(Qt.AlignLeft)

        self.ui.frame_12.hide()
        self.ui.frame_11.hide()
        self.ui.frame_9.hide()

        #self.ui.sumStatLabel.setText("Set summary statistics for all loci"%self.numGroup)
        for ty in self.parent.parent.typesOrdered :
            if ty in self.parent.parent.data.ntypeloc.keys() :
                exec("self.ui.available%sLabel.setText(str(self.parent.parent.data.ntypeloc[ty]))"%ty)
                exec("self.ui.taken%sEdit.setText(str(self.parent.parent.data.ntypeloc[ty]))"%ty)
        self.lockLociSelection()



    def exit(self):
        self.parent.parent.ui.refTableStack.removeWidget(self)
        self.parent.parent.ui.refTableStack.setCurrentIndex(0)
        self.parent.parent.updateNbStats()

    def validate(self,silent=False):
        try:
            # pour voir si ça déclenche une exception
            (nbstats,b) = self.getStats()
            for ty in self.parent.parent.typesOrdered :
                exec("av = int(str(self.ui.available%sLabel.text()))"%ty)
                exec("ta = int(str(self.ui.taken%sEdit.text()))"%ty)
                #if fro+ta > av+1 or ta <= 0 or fro <= 0: kkpz-rm
                if ta > av or ta < 0 :
                    raise Exception("Number of taken loci must be positive and at most equal to available loci number")
            exec("hchoosen = int(str(self.ui.takenHEdit.text()))")
            fro = int(str(self.ui.fromEdit.text()))
            if fro <= 0 :
                raise Exception('"from locus" numver must be positive')
            #if self.parent.parent.getNbSumStats() == 0:
            if nbstats == 0:
                raise Exception("You must select at least one summary statistic")
            self.exit()
        except Exception as e:
            if not silent:
                output.notify(self,"Input error",str(e))

    def lockLociSelection(self):
        """Disabled loci selection if not present in the datafile"""
        for ty in self.parent.parent.typesOrdered:
            if ty not in self.parent.parent.data.ntypeloc.keys():
                exec("self.ui.taken%sEdit.setText('0')"%ty)
                exec("self.ui.taken%sEdit.setDisabled(True)"%ty)

    def addOneSample(self,num):
        """ methode d'ajout d'un bloc dans 'one sample sum stats'
        """
        frame_9 = QtGui.QFrame(self.ui.oneFrame)
        frame_9.setMinimumSize(QtCore.QSize(60, 0))
        frame_9.setMaximumSize(QtCore.QSize(60, 16777215))
        frame_9.setFrameShape(QtGui.QFrame.StyledPanel)
        frame_9.setFrameShadow(QtGui.QFrame.Raised)
        frame_9.setObjectName("frame_9")
        verticalLayout_4 = QtGui.QVBoxLayout(frame_9)
        verticalLayout_4.setObjectName("verticalLayout_4")
        verticalLayout_4.setSpacing(1)
        verticalLayout_4.setContentsMargins(1, 1, 1, 1)
        oneSampleLabel = QtGui.QLabel("Samp %i"%num,frame_9)
        font = QtGui.QFont()
        font.setPointSize(8)
        oneSampleLabel.setFont(font)
        oneSampleLabel.setMinimumSize(QtCore.QSize(0, 14))
        oneSampleLabel.setMaximumSize(QtCore.QSize(50000, 14))
        oneSampleLabel.setAlignment(QtCore.Qt.AlignCenter)
        oneSampleLabel.setObjectName("oneSampleLabel")
        verticalLayout_4.addWidget(oneSampleLabel)

        for stat in self.statList1:
            horizontalLayout_7 = QtGui.QHBoxLayout()
            horizontalLayout_7.setObjectName("horizontalLayout_7")
            spacerItem2 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
            horizontalLayout_7.addItem(spacerItem2)
            check = QtGui.QCheckBox(frame_9)
            check.setMinimumSize(QtCore.QSize(20, 14))
            check.setMaximumSize(QtCore.QSize(20, 14))
            check.setObjectName("%sCheck"%self.confToStat1[stat])
            horizontalLayout_7.addWidget(check)
            spacerItem3 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
            horizontalLayout_7.addItem(spacerItem3)
            verticalLayout_4.addLayout(horizontalLayout_7)

        self.ui.horizontalLayout_4.addWidget(frame_9)

        self.oneSampleList.append(frame_9)

    def addTwoSample(self,num1,num2):
        """ methode d'ajout d'un bloc dans 'two samples sum stats'
        """
        frame_11 = QtGui.QFrame(self.ui.twoFrame)
        frame_11.setMinimumSize(QtCore.QSize(70, 0))
        frame_11.setMaximumSize(QtCore.QSize(70, 16777215))
        frame_11.setFrameShape(QtGui.QFrame.StyledPanel)
        frame_11.setFrameShadow(QtGui.QFrame.Raised)
        frame_11.setObjectName("frame_11")
        verticalLayout_5 = QtGui.QVBoxLayout(frame_11)
        verticalLayout_5.setSpacing(1)
        verticalLayout_5.setContentsMargins(1, 1, 1, 1)
        verticalLayout_5.setObjectName("verticalLayout_5")
        twoSampleLabel = QtGui.QLabel("Samp %i&%i"%(num1,num2),frame_11)
        font = QtGui.QFont()
        font.setPointSize(8)
        twoSampleLabel.setFont(font)
        twoSampleLabel.setMinimumSize(QtCore.QSize(0, 14))
        twoSampleLabel.setMaximumSize(QtCore.QSize(16777215, 14))
        twoSampleLabel.setObjectName("twoSampleLabel")
        verticalLayout_5.addWidget(twoSampleLabel)

        for stat in self.statList2:
            if stat == "NP0":
                lab = QLabel("")
                lab.setMinimumSize(QtCore.QSize(0, 14))
                lab.setMaximumSize(QtCore.QSize(0, 14))
                verticalLayout_5.addWidget(lab)
            horizontalLayout_14 = QtGui.QHBoxLayout()
            horizontalLayout_14.setObjectName("horizontalLayout_14")
            spacerItem12 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
            horizontalLayout_14.addItem(spacerItem12)
            check = QtGui.QCheckBox(frame_11)
            check.setMinimumSize(QtCore.QSize(20, 14))
            check.setMaximumSize(QtCore.QSize(20, 14))
            check.setObjectName("%sCheck"%self.confToStat2[stat])
            horizontalLayout_14.addWidget(check)
            spacerItem13 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
            horizontalLayout_14.addItem(spacerItem13)
            verticalLayout_5.addLayout(horizontalLayout_14)

        self.ui.horizontalLayout_5.addWidget(frame_11)

        self.twoSampleList.append(frame_11)

    def addAdmixSampleGui(self,num1,num2,num3):
        """ methode d'ajout d'un bloc dans 'three sample admix sum stats'
        """
        frame_12 = QtGui.QFrame(self.ui.admixFrame)
        frame_12.setMinimumSize(QtCore.QSize(85, 0))
        frame_12.setMaximumSize(QtCore.QSize(85, 16777215))
        frame_12.setFrameShape(QtGui.QFrame.StyledPanel)
        frame_12.setFrameShadow(QtGui.QFrame.Raised)
        frame_12.setObjectName("frame_12")
        verticalLayout_6 = QtGui.QVBoxLayout(frame_12)
        verticalLayout_6.setSpacing(1)
        #verticalLayout_6.setAlignment(Qt.AlignHCenter)
        verticalLayout_6.setContentsMargins(1, 1, 1, 1)
        verticalLayout_6.setObjectName("verticalLayout_6")
        threeSampleLabel = QtGui.QLabel("Samp\n%i&%i&%i"%(num1,num2,num3),frame_12)
        threeSampleLabel.setAlignment(Qt.AlignHCenter)
        font = QtGui.QFont()
        font.setPointSize(8)
        threeSampleLabel.setFont(font)
        threeSampleLabel.setObjectName("threeSampleLabel")
        threeSampleLabel.setMinimumSize(QtCore.QSize(0, 48))
        threeSampleLabel.setMaximumSize(QtCore.QSize(16777215, 48))
        verticalLayout_6.addWidget(threeSampleLabel)
        rmButton = QtGui.QPushButton("remove",frame_12)
        font = QtGui.QFont()
        font.setPointSize(8)
        rmButton.setFont(font)
        rmButton.setObjectName("rmAdmixButton")
        verticalLayout_6.addWidget(rmButton)
        verticalLayout_6.addWidget(QLabel(" "))
        spacer = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        verticalLayout_6.addItem(spacer)


        for stat in self.statList3:
            horizontalLayout_17 = QtGui.QHBoxLayout()
            horizontalLayout_17.setObjectName("horizontalLayout_17")
            #lab = QtGui.QLabel("Mean",frame_12)
            #horizontalLayout_17.addWidget(lab)
            spacerItem20 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
            horizontalLayout_17.addItem(spacerItem20)
            check = QtGui.QCheckBox(frame_12)
            check.setMinimumSize(QtCore.QSize(20, 27))
            check.setMaximumSize(QtCore.QSize(20, 27))
            check.setObjectName("%sCheck"%self.confToStat3[stat])
            #ml3Check.setChecked(True)
            #ml3Check.setDisabled(True)
            horizontalLayout_17.addWidget(check)
            spacerItem21 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
            horizontalLayout_17.addItem(spacerItem21)
            verticalLayout_6.addLayout(horizontalLayout_17)

        #verticalLayout_6.setContentsMargins(0,0,0,0)

        self.ui.horizontalLayout_2.addWidget(frame_12)

        self.admixSampleList.append(frame_12)

        QObject.connect(rmButton,SIGNAL("clicked()"),self.rmAdmix)

    def getStats(self):
        """ retourne les stats sous la forme d'un dico indexé par les noms de ligne
        """
        nstat = 0
        # dico indexé par les noms de ligne (NAL,FST...) qui donne la liste des samples concernés
        dico_stats = {}
        for stat in self.statList1 + self.statList2 + self.statList3:
            dico_stats[stat] = []

        for box in self.oneSampleList:
            lab = str(box.findChild(QLabel,"oneSampleLabel").text()).split()[1]
            for stat in self.statList1:
                if box.findChild(QCheckBox,"%sCheck"%self.confToStat1[stat]).isChecked():
                    dico_stats[stat].append(lab)
                    nstat += 1
        for box in self.twoSampleList:
            lab = str(box.findChild(QLabel,"twoSampleLabel").text()).split()[1]
            for stat in self.statList2:
                if box.findChild(QCheckBox,"%sCheck"%self.confToStat2[stat]).isChecked():
                    dico_stats[stat].append(lab)
                    nstat += 1
        for box in self.admixSampleList:
            lab = str(box.findChild(QLabel,"threeSampleLabel").text()).split('\n')[1]
            empty = True
            for stat in self.statList3:
                if box.findChild(QCheckBox,"%sCheck"%self.confToStat3[stat]).isChecked():
                    empty = False
                    dico_stats[stat].append(lab)
                    nstat +=1
            if empty:
                raise Exception("All admixture statistic must not be empty (%s)"%lab)
        return (nstat,dico_stats)

    def setSumConf(self,lines):
        """ grace aux lignes du fichier de conf, remet les sum stats
        """
        # construction du dico de stats (le même que dans getSumConf)
        dico_stats = {}
        for line in lines:
            sline = line.strip()
            if sline.strip() != "":
                t = sline.split()[0]
                dico_stats[t] = []
                for sample in sline.split()[1:]:
                    dico_stats[t].append(sample)
        #print "dico stats :",dico_stats

        admixAlreadyDone = []
        # pour chaque ligne (de case à cocher)
        for k in dico_stats.keys():
            if k in self.statList1:
                name_chk_box = self.confToStat1[k]
                for box in self.oneSampleList:
                    num_sample = str(box.findChild(QLabel,"oneSampleLabel").text()).split()[1].strip()
                    if num_sample in dico_stats[k]:
                        # on coche
                        box.findChild(QCheckBox,"%sCheck"%name_chk_box).setChecked(True)

            elif k in self.statList2:
                name_chk_box = self.confToStat2[k]
                for box in self.twoSampleList:
                    num_sample = str(box.findChild(QLabel,"twoSampleLabel").text()).split()[1].strip()
                    if num_sample in dico_stats[k]:
                        # on coche
                        box.findChild(QCheckBox,"%sCheck"%name_chk_box).setChecked(True)

            elif k in self.statList3:
                name_chk_box = self.confToStat3[k]
                for aml in dico_stats[k]:
                    num1 = aml.strip().split('&')[0]
                    num2 = aml.strip().split('&')[1]
                    num3 = aml.strip().split('&')[2]
                    if (num1,num2,num3) not in admixAlreadyDone:
                        self.addAdmixSampleGui(int(num1),int(num2),int(num3))
                        self.admixSampleList[-1].findChild(QCheckBox,"%sCheck"%name_chk_box).setChecked(True)
                        admixAlreadyDone.append((num1,num2,num3))
                    else:
                        for box in self.admixSampleList:
                            if "%s&%s&%s"%(num1,num2,num3) == str(box.findChild(QLabel,"threeSampleLabel").text()).strip().split('\n')[1]:
                                box.findChild(QCheckBox,"%sCheck"%name_chk_box).setChecked(True)
                                break

    def clear(self):
        self.parent.parent.clearSummaryStats()
        self.exit()

    def getSumStatsTableHeader(self,numGroup):
        """ retourne l'en-tête pour le tableau à écrire dans conf.th
        """
        result = u""
        dico = {}
        for stat in self.statList1 + self.statList2 + self.statList3:
            dico[stat] = []

        gnumber = numGroup

        for box in self.oneSampleList:
            lab = str(box.findChild(QLabel,"oneSampleLabel").text()).split()[1]
            for statstr in self.statList1:
                if box.findChild(QCheckBox,"%sCheck"%self.confToStat1[statstr]).isChecked():
                    stat_name = "%s_%s_%s"%(statstr,gnumber,lab)
                    dico[statstr].append(stat_name)
        for box in self.twoSampleList:
            lab = str(box.findChild(QLabel,"twoSampleLabel").text()).split()[1]
            for statstr in self.statList2:
                if box.findChild(QCheckBox,"%sCheck"%self.confToStat2[statstr]).isChecked():
                    stat_name = "%s_%s_%s"%(statstr,gnumber,lab)
                    dico[statstr].append(stat_name)
        for box in self.admixSampleList:
            lab = str(box.findChild(QLabel,"threeSampleLabel").text()).split('\n')[1]
            for statstr in self.statList3:
                if box.findChild(QCheckBox,"%sCheck"%self.confToStat3[statstr]).isChecked():
                    stat_name = "%s_%s_%s"%(statstr,gnumber,lab)
                    dico[statstr].append(stat_name)

        for st in self.statList1 + self.statList2 + self.statList3:
            l = dico[st]
            for s in l:
                result += output.centerHeader(s,14)

        return result

