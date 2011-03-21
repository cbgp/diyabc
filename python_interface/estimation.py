# -*- coding: utf-8 -*-

import os
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from uis.estimation_ui import Ui_Frame
from genericScenarioSelection import GenericScenarioSelection

class Estimation(QFrame):
    """ dernière étape de définition d'une analyse de type estimation
    """
    def __init__(self,analysis,parent=None):
        super(Estimation,self).__init__(parent)
        self.parent=parent
        self.analysis = analysis
        self.scNumList = []
        self.dico_values = {}
        self.createWidgets()
        self.ui.verticalLayout_3.setAlignment(Qt.AlignHCenter)
        self.ui.verticalLayout_3.setAlignment(Qt.AlignTop)



    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        if self.analysis[0] == "bias":
            self.ui.label.setText("Bias and mean square error")
            self.ui.redefButton.hide()
            self.ui.candidateLabel.hide()
            self.setScenarios([self.analysis[2]])
        elif self.analysis[0] == "evaluate":
            self.ui.label.setText("Confidence in scenario choice")
            sctxt = ""
            for s in self.analysis[3]:
                sctxt += " %s"%s

            self.ui.candidateLabel.setText("%s %s"%(self.ui.candidateLabel.text(),sctxt))
            self.ui.redefButton.hide()
            self.setScenarios([self.analysis[2]])
        else:
            self.ui.candidateLabel.hide()
            self.ui.label_5.hide()
            self.ui.label_4.hide()
            self.ui.notdsEdit.hide()

        self.ui.projectNameEdit.setText(self.parent.parent.dir)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.redefButton,SIGNAL("clicked()"),self.redefineScenarios)


    def validate(self):
        """ termine la définition de l'analyse
        """
        self.analysis.append(self.scNumList)
        self.majDicoValues()
        self.analysis.append(self.dico_values)
        chosen_scs_txt = ""
        for cs in self.analysis[-2]:
            chosen_scs_txt+="%s,"%str(cs)
        chosen_scs_txt = chosen_scs_txt[:-1]
        dico_est = self.analysis[-1]
        self.analysis.append("s:%s;n:%s;m:%s;t:%s;p:%s"%(chosen_scs_txt,dico_est['numberOfselData'],dico_est['choNumberOfsimData'],dico_est['transformation'],dico_est['choice']))
        self.parent.parent.addAnalysis(self.analysis)
        self.exit()

    def setScenarios(self,scList):
        """ écrit la liste des scenarios à estimer
        """
        plur= ""
        if len(scList)>1:
            plur = "s"
            
        lstxt=""
        self.scNumList = []
        for i in scList:
            lstxt+="%s, "%i
            self.scNumList.append(i)
        lstxt = lstxt[:-2]

        txt = "Chosen scenario%s : %s"%(plur,lstxt)
        self.ui.scenariosLabel.setText(txt)

    def redefineScenarios(self):
        """ repasse sur le choix des scenarios en lui redonnant moi même comme next_widget
        """
        estimateFrame = self
        genSel = GenericScenarioSelection(len(self.parent.parent.hist_model_win.scList),"Parameters will be estimated considering data sets simulated with",estimateFrame,"ABC parameter estimation",1,self.parent)
        #self.parent.parent.addTab(genSel,"Scenario selection")
        #self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        #self.parent.parent.setCurrentWidget(genSel)
        self.parent.parent.ui.analysisStack.addWidget(genSel)
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentWidget(genSel)


    def majDicoValues(self):
        if self.ui.logRadio.isChecked():
            #trans = "log"
            trans = "2"
        elif self.ui.logtgRadio.isChecked():
            #trans = "logtg"
            trans = "4"
        elif self.ui.noRadio.isChecked():
            #trans = "no"
            trans = "1"
        elif self.ui.logitRadio.isChecked():
            #trans = "logit"
            trans = "3"
        self.dico_values["transformation"] = trans
        self.dico_values["numberOfselData"] = str(self.ui.nosdEdit.text())
        self.dico_values["choNumberOfsimData"] = str(self.ui.cnosdEdit.text())
        choice = ""
        if self.ui.oCheck.isChecked():
            choice += "o"
        if self.ui.cCheck.isChecked():
            choice += "c"
        self.dico_values["choice"] = choice

    def exit(self):
        ## reactivation des onglets
        #self.parent.parent.setTabEnabled(1,True)
        #self.parent.parent.setTabEnabled(0,True)
        #self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        #self.parent.parent.setCurrentIndex(1)
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentIndex(0)

