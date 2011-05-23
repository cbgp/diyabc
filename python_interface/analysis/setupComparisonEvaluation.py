# -*- coding: utf-8 -*-

import os
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.setupComparisonEvaluation_ui import Ui_Frame
from genericScenarioSelection import GenericScenarioSelection
import output

formSetupComparisonEvaluation,baseSetupComparisonEvaluation = uic.loadUiType("uis/setupComparisonEvaluation.ui")

class SetupComparisonEvaluation(formSetupComparisonEvaluation,baseSetupComparisonEvaluation):
    """ dernière étape de définition d'une analyse de type comparison ou evaluate
    """
    def __init__(self,analysis,parent=None):
        super(SetupComparisonEvaluation,self).__init__(parent)
        self.parent=parent
        self.analysis = analysis
        self.scNumList = []
        self.dico_values = {}
        self.createWidgets()
        self.ui.verticalLayout_2.setAlignment(Qt.AlignHCenter)
        self.ui.verticalLayout_2.setAlignment(Qt.AlignTop)



    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        self.ui.projectNameEdit.setText(self.parent.parent.dir)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.redefButton,SIGNAL("clicked()"),self.redefineScenarios)

        if self.analysis.category == "evaluate":
            self.ui.label.setText("Confidence in scenario choice")
            self.setScenarios([self.analysis.chosenSc])
            self.setCandidateScenarios(self.analysis.candidateScList)
            self.ui.redefButton.hide()
            self.ui.numRegCombo.clear()
            self.ui.numRegCombo.addItem("0")
            self.ui.numRegCombo.addItem("1")
            self.ui.numRegCombo.setCurrentIndex(1)
            self.ui.notdsEdit.setText("500")
        else:
            self.ui.notdsEdit.hide()
            self.ui.notdsLabel.hide()

        self.ui.cnosdEdit.setText(self.parent.parent.ui.nbSetsDoneEdit.text())
        self.ui.deEdit.setText("500")
        onePc = int(self.parent.parent.ui.nbSetsDoneEdit.text()) / 100
        if onePc < 1000:
            if int(self.parent.parent.ui.nbSetsDoneEdit.text()) < 1000:
                onePc = nbSetsDone
            else:
                onePc = 1000
        self.ui.lrEdit.setText(str(onePc))

        self.ui.totNumSimEdit.setText(self.parent.parent.ui.nbSetsDoneEdit.text())

    def checkAll(self):
        problems = ""
        try:
            if (self.ui.totNumSimEdit.text() != "" or self.ui.totNumSimEdit.text() != "0") and int(self.ui.totNumSimEdit.text()) < int(self.ui.cnosdEdit.text()):
                problems += "Impossible to select more data than it exists in the reference table\n"
            if self.analysis.category == "evaluate":
                notds = int(self.ui.notdsEdit.text())
            lr = int(self.ui.lrEdit.text())
            de = int(self.ui.deEdit.text())
            cnosd = int(self.ui.cnosdEdit.text())

        except Exception,e:
            problems += "Only non-empty integer values are accepted\n"

        if problems == "":
            return True
        else:
            output.notify(self,"value problem",problems)
            return False


    def validate(self):
        """ défini les computation parameters de l'analyse et ajoute celle-ci au projet
        """
        #self.parent.parent.addRow("scenario choice","params","4","new")
        self.analysis.candidateScList = self.scNumList
        self.majDicoValues()
        #self.analysis.append(self.dico_values)
        chosen_scs_txt = ""
        for cs in self.scNumList:
            chosen_scs_txt+="%s,"%str(cs)
        chosen_scs_txt = chosen_scs_txt[:-1]
        #dico_comp = self.analysis[-1]
        if self.checkAll():
            if self.analysis.category == "compare":
                self.analysis.computationParameters = "s:%s;n:%s;d:%s;l:%s;m:%s"%(chosen_scs_txt,self.dico_values['choNumberOfsimData'],self.dico_values['de'],self.dico_values['lr'],self.dico_values['numReg'])
            elif self.analysis.category == "evaluate":
                candListTxt = ""
                for cs in self.analysis.candidateScList:
                    candListTxt+="%s,"%str(cs)
                candListTxt = candListTxt[:-1]
                strparam = "s:%s;"%candListTxt
                strparam += "r:%s;"%self.analysis.chosenSc
                strparam += "n:%s;"%self.dico_values['choNumberOfsimData']
                strparam += "m:%s;"%self.dico_values['numReg']
                strparam += "d:%s;"%self.dico_values['de']
                strparam += "l:%s;"%self.dico_values['lr']
                strparam += "t:%s;"%self.dico_values['notds']
                #strparam += "p:%s;"%self.dico_values['choice']
                #print "robert ", self.analysis
                strparam += "h:"
                for paramname in self.analysis.histParams.keys():
                    l = self.analysis.histParams[paramname]
                    strparam += "%s="%paramname
                    if len(l) == 2:
                        strparam += "%s "%l[1]
                    else:
                        strparam += "%s[%s,%s,%s,%s] "%(l[1],l[2],l[3],l[4],l[5])
                for ctxt in self.analysis.condTxtList:
                    strparam += "%s "%ctxt
                strparam = strparam[:-1]
                strparam += ";u:"
                #print "jaco:%s "%len(self.analysis[5]), self.analysis[5]
                if len(self.analysis.mutationModel)>0:
                    if type(self.analysis.mutationModel[0]) == type(u'plop'):
                        for ind,gr in enumerate(self.analysis.mutationModel):
                            strparam += "g%s("%(ind+1)
                            strgr = gr.strip()
                            strgr = strgr.split('\n')
                            print "\nstrgr %s\n"%strgr
                            for j,elem in enumerate(strgr):
                                if elem.split(' ')[0] != "MODEL":
                                    to_add = strgr[j].split(' ')[1]
                                    strparam += "%s "%to_add
                            # virer le dernier espace
                            strparam = strparam[:-1]
                            strparam += ")*"
                    else:
                        for ind,gr in enumerate(self.analysis.mutationModel):
                            strparam += "g%s("%(ind+1)
                            for num in gr:
                                strparam += "%s "%num
                            # virer le dernier espace
                            strparam = strparam[:-1]
                            strparam += ")*"
                    # virer le dernier '-'
                    strparam = strparam[:-1]
                #print "ursulla : %s"%strparam

                self.analysis.computationParameters = strparam
            self.parent.parent.addAnalysis(self.analysis)
            self.exit()

    def setCandidateScenarios(self,scList):
        """ écrit la liste des scenarios candidats
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

        txt = "Candidate scenario%s : %s"%(plur,lstxt)
        self.ui.candidateLabel.setText(txt)

    def setScenarios(self,scList):
        """ écrit la liste des scenarios à comparer
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
        """ retourne au choix des scenarios en lui redonnant moi même comme next_widget
        """
        compFrame = self
        genSel = GenericScenarioSelection(len(self.parent.parent.hist_model_win.scList),"Select the scenarios that you wish to compare",compFrame,"Comparison of scenarios",2,self.analysis,self.parent)
        #self.parent.parent.addTab(genSel,"Scenario selection")
        #self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        #self.parent.parent.setCurrentWidget(genSel)
        self.parent.parent.ui.analysisStack.addWidget(genSel)
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentWidget(genSel)

    def majDicoValues(self):
        self.dico_values["choNumberOfsimData"] = str(self.ui.cnosdEdit.text())
        self.dico_values["de"] = str(self.ui.deEdit.text())
        self.dico_values["lr"] = str(self.ui.lrEdit.text())
        self.dico_values["numReg"] = str(self.ui.numRegCombo.currentText())
        self.dico_values["notds"] = str(self.ui.notdsEdit.text())

    def exit(self):
        ## reactivation des onglets
        #self.parent.parent.setTabEnabled(1,True)
        #self.parent.parent.setTabEnabled(0,True)
        #self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        #self.parent.parent.setCurrentIndex(1)
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentIndex(0)

