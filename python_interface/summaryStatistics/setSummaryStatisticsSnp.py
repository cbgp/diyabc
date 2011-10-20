# -*- coding: utf-8 -*-

import sys
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

formSetSummaryStatisticsSnp,baseSetSummaryStatisticsSnp = uic.loadUiType("uis/setSummaryStatisticsSnp.ui")

class SetSummaryStatisticsSnp(SetSummaryStatistics,formSetSummaryStatisticsSnp,baseSetSummaryStatisticsSnp):
    """ ecran de selection des summary statistics pour les SNP
    """
    def __init__(self,parent=None,box_group=None,numGroup=0):
        super(SetSummaryStatisticsSnp,self).__init__(parent,box_group,numGroup)

        
        self.statList = ["SHM","SHD","SNM","SND","SFM","SFD","SAM","SAD"]
        self.confToStat = {"SHM" : "mgd","SHD" : "dgd", "SNM": "mnd2","SND" : "dond2", "SFM" : "mfd2", "SFD" : "dofd2", "SAM" : "ml3", "SAD" : "mld3" }

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.clearButton,SIGNAL("clicked()"),self.clear)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)

        # all buttons
        QObject.connect(self.ui.allMgdButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allDgdButton,SIGNAL("clicked()"),self.allPressed)
        # none buttons
        QObject.connect(self.ui.noneMgdButton,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneDgdButton,SIGNAL("clicked()"),self.allPressed)

        # all buttons 2
        QObject.connect(self.ui.allMnd2Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allDond2Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allMfd2Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allDofd2Button,SIGNAL("clicked()"),self.allPressed)
        #none buttons 2
        QObject.connect(self.ui.noneMnd2Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneDond2Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneMfd2Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneDofd2Button,SIGNAL("clicked()"),self.allPressed)
        # all and none for admixtures
        QObject.connect(self.ui.allMl3Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneMl3Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.allMld3Button,SIGNAL("clicked()"),self.allPressed)
        QObject.connect(self.ui.noneMld3Button,SIGNAL("clicked()"),self.allPressed)

        QObject.connect(self.ui.addAdmixButton,SIGNAL("clicked()"),self.addAdmix)

        self.ui.horizontalLayout_4.setAlignment(Qt.AlignLeft)
        self.ui.horizontalLayout_2.setAlignment(Qt.AlignLeft)
        self.ui.horizontalLayout_5.setAlignment(Qt.AlignLeft)

        self.ui.frame_12.hide()
        self.ui.frame_11.hide()
        self.ui.frame_9.hide()
        #self.ui.allMl3Button.hide()
        #self.ui.allMld3Button.hide()
        #self.ui.noneMld3Button.hide()
        #self.ui.noneMl3Button.hide()

        self.ui.sumStatLabel.setText("Set summary statistics for '%s' locus"%self.numGroup)
        self.ui.availableEdit.setText(str(self.parent.parent.data.ntypeloc[self.numGroup]))
        self.ui.takenEdit.setText(str(self.parent.parent.data.ntypeloc[self.numGroup]))

    def exit(self):
        self.parent.parent.ui.refTableStack.removeWidget(self)
        self.parent.parent.ui.refTableStack.setCurrentIndex(0)
        self.parent.parent.updateNbStats()
    def validate(self,silent=False):
        try:
            # pour voir si ça déclenche une exception
            (a,b) = self.getStats()

            av = int(str(self.ui.availableEdit.text()))
            ta = int(str(self.ui.takenEdit.text()))
            fro = int(str(self.ui.fromEdit.text()))
            if fro+ta > av or ta <= 0 or fro <= 0:
                raise Exception("Number of taken loci must be positive and inferior to number of available loci")
            self.exit()
        except Exception,e:
            if not silent:
                output.notify(self,"Input error",str(e))

    def loadSumStatsConf(self):
        # TODO
        pass


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
        horizontalLayout_7 = QtGui.QHBoxLayout()
        horizontalLayout_7.setObjectName("horizontalLayout_7")
        spacerItem2 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_7.addItem(spacerItem2)
        mgdCheck = QtGui.QCheckBox(frame_9)
        mgdCheck.setMinimumSize(QtCore.QSize(20, 14))
        mgdCheck.setMaximumSize(QtCore.QSize(20, 14))
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
        msvCheck.setMinimumSize(QtCore.QSize(20, 14))
        msvCheck.setMaximumSize(QtCore.QSize(20, 14))
        msvCheck.setObjectName("dgdCheck")
        horizontalLayout_8.addWidget(msvCheck)
        spacerItem5 = QtGui.QSpacerItem(18, 13, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_8.addItem(spacerItem5)
        verticalLayout_4.addLayout(horizontalLayout_8)
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
        horizontalLayout_16 = QtGui.QHBoxLayout()
        horizontalLayout_16.setObjectName("horizontalLayout_16")
        spacerItem8 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_16.addItem(spacerItem8)
        mnoa2Check = QtGui.QCheckBox(frame_11)
        mnoa2Check.setMinimumSize(QtCore.QSize(20, 14))
        mnoa2Check.setMaximumSize(QtCore.QSize(20, 14))
        mnoa2Check.setObjectName("mnd2Check")
        horizontalLayout_16.addWidget(mnoa2Check)
        spacerItem9 = QtGui.QSpacerItem(17, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_16.addItem(spacerItem9)
        verticalLayout_5.addLayout(horizontalLayout_16)
        horizontalLayout_15 = QtGui.QHBoxLayout()
        horizontalLayout_15.setObjectName("horizontalLayout_15")
        spacerItem10 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_15.addItem(spacerItem10)
        mgd2Check = QtGui.QCheckBox(frame_11)
        mgd2Check.setMinimumSize(QtCore.QSize(20, 14))
        mgd2Check.setMaximumSize(QtCore.QSize(20, 14))
        mgd2Check.setObjectName("dond2Check")
        horizontalLayout_15.addWidget(mgd2Check)
        spacerItem11 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_15.addItem(spacerItem11)
        verticalLayout_5.addLayout(horizontalLayout_15)
        horizontalLayout_14 = QtGui.QHBoxLayout()
        horizontalLayout_14.setObjectName("horizontalLayout_14")
        spacerItem12 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_14.addItem(spacerItem12)
        msv2Check = QtGui.QCheckBox(frame_11)
        msv2Check.setMinimumSize(QtCore.QSize(20, 14))
        msv2Check.setMaximumSize(QtCore.QSize(20, 14))
        msv2Check.setObjectName("mfd2Check")
        horizontalLayout_14.addWidget(msv2Check)
        spacerItem13 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_14.addItem(spacerItem13)
        verticalLayout_5.addLayout(horizontalLayout_14)
        horizontalLayout_13 = QtGui.QHBoxLayout()
        horizontalLayout_13.setObjectName("horizontalLayout_13")
        spacerItem14 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_13.addItem(spacerItem14)
        fst2Check = QtGui.QCheckBox(frame_11)
        fst2Check.setMinimumSize(QtCore.QSize(20, 14))
        fst2Check.setMaximumSize(QtCore.QSize(20, 14))
        fst2Check.setObjectName("dofd2Check")
        horizontalLayout_13.addWidget(fst2Check)
        spacerItem15 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_13.addItem(spacerItem15)
        verticalLayout_5.addLayout(horizontalLayout_13)
        self.ui.horizontalLayout_5.addWidget(frame_11)

        self.twoSampleList.append(frame_11)

    def addAdmixSampleGui(self,num1,num2,num3):
        """ methode d'ajout d'un bloc dans 'three sample admix sum stats'
        """
        frame_12 = QtGui.QFrame(self.ui.scrollAreaWidgetContents_3)
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
        rmButton.setObjectName("rmButton")
        verticalLayout_6.addWidget(rmButton)
        horizontalLayout_17 = QtGui.QHBoxLayout()
        horizontalLayout_17.setObjectName("horizontalLayout_17")
        lab = QtGui.QLabel("Mean",frame_12)
        horizontalLayout_17.addWidget(lab)
        spacerItem20 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_17.addItem(spacerItem20)
        ml3Check = QtGui.QCheckBox(frame_12)
        ml3Check.setMinimumSize(QtCore.QSize(20, 30))
        ml3Check.setMaximumSize(QtCore.QSize(20, 30))
        ml3Check.setObjectName("ml3Check")
        #ml3Check.setChecked(True)
        #ml3Check.setDisabled(True)
        horizontalLayout_17.addWidget(ml3Check)
        spacerItem21 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_17.addItem(spacerItem21)
        verticalLayout_6.addLayout(horizontalLayout_17)

        spacer = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        verticalLayout_6.addItem(spacer)

        horizontalLayout_17 = QtGui.QHBoxLayout()
        horizontalLayout_17.setObjectName("horizontalLayout_17")
        lab = QtGui.QLabel("Distance",frame_12)
        horizontalLayout_17.addWidget(lab)
        spacerItem20 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_17.addItem(spacerItem20)
        ml3Check = QtGui.QCheckBox(frame_12)
        ml3Check.setMinimumSize(QtCore.QSize(20, 30))
        ml3Check.setMaximumSize(QtCore.QSize(20, 30))
        ml3Check.setObjectName("mld3Check")
        #ml3Check.setChecked(True)
        #ml3Check.setDisabled(True)
        horizontalLayout_17.addWidget(ml3Check)
        spacerItem21 = QtGui.QSpacerItem(18, 18, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        horizontalLayout_17.addItem(spacerItem21)
        verticalLayout_6.addLayout(horizontalLayout_17)
        self.ui.horizontalLayout_2.addWidget(frame_12)

        self.admixSampleList.append(frame_12)

        QObject.connect(rmButton,SIGNAL("clicked()"),self.rmAdmix)

    def getStats(self):
        """ retourne les stats sous la forme d'un dico indexé par les noms de ligne
        """
        nstat = 0
        # dico indexé par les noms de ligne (NAL,FST...) qui donne la liste des samples concernés
        dico_stats = {}
        dico_stats["SHM"] = []
        dico_stats["SHD"] = []

        dico_stats["SNM"] = []
        dico_stats["SND"] = []
        dico_stats["SFM"] = []
        dico_stats["SFD"] = []

        dico_stats["SAM"] = []
        dico_stats["SAD"] = []
        for box in self.oneSampleList:
            lab = str(box.findChild(QLabel,"oneSampleLabel").text()).split(' ')[1]
            if box.findChild(QCheckBox,"mgdCheck").isChecked():
                dico_stats["SHM"].append(lab)
                nstat += 1
            if box.findChild(QCheckBox,"dgdCheck").isChecked():
                dico_stats["SHD"].append(lab)
                nstat += 1
        for box in self.twoSampleList:
            lab = str(box.findChild(QLabel,"twoSampleLabel").text()).split(' ')[1]
            if box.findChild(QCheckBox,"mnd2Check").isChecked():
                dico_stats["SNM"].append(lab)
                nstat += 1
            if box.findChild(QCheckBox,"dond2Check").isChecked():
                dico_stats["SND"].append(lab)
                nstat += 1
            if box.findChild(QCheckBox,"mfd2Check").isChecked():
                dico_stats["SFM"].append(lab)
                nstat += 1
            if box.findChild(QCheckBox,"dofd2Check").isChecked():
                dico_stats["SFD"].append(lab)
                nstat += 1
        for box in self.admixSampleList:
            lab = str(box.findChild(QLabel,"threeSampleLabel").text()).split('\n')[1]
            if box.findChild(QCheckBox,"ml3Check").isChecked():
                dico_stats["SAM"].append(lab)
                nstat +=1
            if box.findChild(QCheckBox,"mld3Check").isChecked():
                dico_stats["SAD"].append(lab)
                nstat +=1
            if (not box.findChild(QCheckBox,"ml3Check").isChecked()) and (not box.findChild(QCheckBox,"mld3Check").isChecked()):
                raise Exception("All admixture statistic must not be empty")
        return (nstat,dico_stats)

    def setSumConf(self,lines):
        """ grace aux lignes du fichier de conf, remet les sum stats
        """
        confToStat = {"SHM" : "mgd","SHD" : "dgd", "SNM": "mnd2","SND" : "dond2", "SFM" : "mfd2", "SFD" : "dofd2", "SAM" : "ml3", "SAD" : "mld3" }

        # construction du dico de stats (le même que dans getSumConf)
        dico_stats = {}
        for line in lines:
            sline = line.strip()
            if sline.strip() != "":
                t = sline.split(' ')[0]
                dico_stats[t] = []
                for sample in sline.split(' ')[1:]:
                    dico_stats[t].append(sample)
        #print "dico stats :",dico_stats

        admixAlreadyDone = []
        # pour chaque ligne (de case à cocher)
        for k in dico_stats.keys():
            name_chk_box = confToStat[k]
            # pour chaque colonne chaque sample dans one Sample
            if k in ["SHM","SHD"]:
                for box in self.oneSampleList:
                    num_sample = str(box.findChild(QLabel,"oneSampleLabel").text()).split(' ')[1].strip()
                    if num_sample in dico_stats[k]:
                        # on coche
                        box.findChild(QCheckBox,"%sCheck"%name_chk_box).setChecked(True)
            elif k in ["SNM", "SND", "SFM", "SFD"]:
                for box in self.twoSampleList:
                    num_sample = str(box.findChild(QLabel,"twoSampleLabel").text()).split(' ')[1].strip()
                    if num_sample in dico_stats[k]:
                        # on coche
                        box.findChild(QCheckBox,"%sCheck"%name_chk_box).setChecked(True)
            elif k in ["SAM","SAD"]:
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
        self.parent.clearSummaryStats(self.box_group)

    def getSumStatsTableHeader(self,numGroup):
        """ retourne l'en-tête pour le tableau à écrire dans conf.th
        """
        result = u""
        dico = {}
        for k in self.confToStat.keys():
            dico[k] = []

        gnumber = numGroup

        for box in self.oneSampleList:
            lab = str(box.findChild(QLabel,"oneSampleLabel").text()).split(' ')[1]
            for statstr in ["SHM","SHD"]:
                if box.findChild(QCheckBox,"%sCheck"%self.confToStat[statstr]).isChecked():
                    stat_name = "%s_%s_%s"%(statstr,gnumber,lab)
                    dico[statstr].append(stat_name)
        for box in self.twoSampleList:
            lab = str(box.findChild(QLabel,"twoSampleLabel").text()).split(' ')[1]
            for statstr in ["SNM","SND","SFM","SFD"]:
                if box.findChild(QCheckBox,"%sCheck"%self.confToStat[statstr]).isChecked():
                    stat_name = "%s_%s_%s"%(statstr,gnumber,lab)
                    dico[statstr].append(stat_name)
        for box in self.admixSampleList:
            lab = str(box.findChild(QLabel,"threeSampleLabel").text()).split('\n')[1]
            for statstr in ["SAM","SAD"]:
                if box.findChild(QCheckBox,"%sCheck"%self.confToStat[statstr]).isChecked():
                    stat_name = "%s_%s_%s"%(statstr,gnumber,lab)
                    dico[statstr].append(stat_name)
        for st in self.statList:
            if st in self.confToStat.keys():
                l = dico[st]
                for s in l:
                    result += output.centerHeader(s,14)

        return result

