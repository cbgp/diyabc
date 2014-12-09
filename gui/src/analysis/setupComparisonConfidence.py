# -*- coding: utf-8 -*-

import sys, codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.setupComparisonConfidence_ui import Ui_Frame
from genericScenarioSelection import GenericScenarioSelection
import output
import variables
from variables import UIPATH
from utils.cbgpUtils import getFsEncoding, Parents

formSetupComparisonConfidence,baseSetupComparisonConfidence = uic.loadUiType((u"%s/setupComparisonConfidence.ui"%UIPATH).encode(getFsEncoding(logLevel=False)))

class SetupComparisonConfidence(formSetupComparisonConfidence,baseSetupComparisonConfidence):
    """ dernière étape de définition d'une analyse de type comparison ou confidence
    """
    def __init__(self,analysis,parent=None):
        super(SetupComparisonConfidence,self).__init__(parent)
        self.parent=parent
        self.parents = Parents(self.parent)
        self.analysis = analysis
        self.scNumList = []
        self.dico_values = {}
        self.createWidgets()
        self.ui.verticalLayout_2.setAlignment(Qt.AlignHCenter)
        self.ui.verticalLayout_2.setAlignment(Qt.AlignTop)

        self.restoreAnalysisValues()

        self.parents.updateDoc(self)


    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        self.deInterValuesW = []
        self.lrInterValuesW = []

        self.ui.projectNameEdit.setText(self.parents.dir)
        self.ui.gridLayout.setAlignment(Qt.AlignTop)
        self.ui.posteriorDistributionFrame.hide()

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.redefButton,SIGNAL("clicked()"),self.redefineScenarios)
        QObject.connect(self.ui.numRegCombo,SIGNAL("currentIndexChanged(QString)"),self.updateInterValues)
        QObject.connect(self.ui.deEdit,SIGNAL("textChanged(QString)"),self.updateInterValues)
        QObject.connect(self.ui.lrEdit,SIGNAL("textChanged(QString)"),self.updateInterValues)
        QObject.connect(self.ui.posteriorDistributionSampleSizeEdit,SIGNAL("textChanged(QString)"),self.updateInterValues)
        QObject.connect(self.ui.logisticRegressioncheckBox,SIGNAL("stateChanged(int)"),self.updateInterValues)


        if "confidence" in self.analysis.category :
            self.ui.label.setText("Confidence in scenario choice")
            self.setScenarios([self.analysis.chosenSc])
            self.setCandidateScenarios(self.analysis.candidateScList)
            self.setRecordValues(self.analysis.candidateScList)
            self.ui.redefButton.hide()
            self.ui.numRegCombo.clear()
            self.ui.numRegCombo.addItem("0")
            self.ui.numRegCombo.addItem("1")
            self.ui.notdsEdit.setText("500")
            self.ui.intermediateValuesFrame.hide()
            if self.analysis.category == "confidence_posterior_global" :
                self.ui.posteriorDistributionFrame.show()
                self.ui.notdsLabel.setText("Number of pseudo-observed test data sets")
        else: # assume self.analysis.category == "compare"
            self.ui.logisticRegressioncheckBox.setChecked(True)
            self.ui.logisticRegressioncheckBox.hide()
            self.ui.notdsEdit.hide()
            self.ui.notdsLabel.hide()
            self.ui.candidateLabel.hide()
            self.setScenarios([self.analysis.chosenSc])
            self.setCandidateScenarios(self.analysis.candidateScList)
            self.setRecordValues(self.analysis.candidateScList)

        self.ui.numRegCombo.setCurrentIndex(1)

        self.ui.deEdit.setText("500")

        self.ui.analysisNameLabel.setText(self.analysis.name)


    def updateInterValues(self,numstr):

        if self.ui.logisticRegressioncheckBox.isChecked() :
            self.lrEdit.setEnabled(True)
            if "confidence" in self.analysis.category :
                self.ui.numRegCombo.setCurrentIndex(self.ui.numRegCombo.findText("1"))
        else :
            self.lrEdit.setDisabled(True)
            if "confidence" in self.analysis.category :
                self.ui.numRegCombo.setCurrentIndex(self.ui.numRegCombo.findText("0"))

        for w in self.lrInterValuesW:
            w.hide()
            self.ui.lrValLayout.removeWidget(w)
        for w in self.deInterValuesW:
            w.hide()
            self.ui.deValLayout.removeWidget(w)
        self.lrInterValuesW = []
        self.deInterValuesW = []


        try:
            #de = int(self.ui.deEdit.text())
            lr = int(self.ui.lrEdit.text())
            numreg = int(self.ui.numRegCombo.currentText())
            if numreg == 0:
                self.ui.lrValuesFrame.setDisabled(True)
                self.ui.lrEdit.setDisabled(True)
                self.analysis.logreg = False
            else :
                self.ui.lrValuesFrame.setDisabled(False)
                self.ui.lrEdit.setDisabled(False)
                self.analysis.logreg = True
                #dedec = de/numreg
                lrdec = lr/numreg
                for i in range(numreg):
                    lrlab = QLabel(str(lr))
                    #delab = QLabel(str(de))
                    self.lrInterValuesW.append(lrlab)
                    #self.deInterValuesW.append(delab)
                    self.ui.lrValLayout.addWidget(lrlab)
                    #self.ui.deValLayout.addWidget(delab)
                    #de -= dedec
                    lr -= lrdec
        except Exception as e:
            print e


    def restoreAnalysisValues(self):
        if self.analysis.computationParameters != "":
            cp = self.analysis.computationParameters
            self.ui.cnosdEdit.setText(cp.split('n:')[1].split(';')[0])
            self.ui.deEdit.setText(cp.split('d:')[1].split(';')[0])
            numreg = cp.split('m:')[1].split(';')[0]
            self.ui.numRegCombo.setCurrentIndex(self.ui.numRegCombo.findText(numreg))
            if numreg in [1, "1"] :
                self.ui.logisticRegressioncheckBox.setChecked(True)
            else :
                self.ui.logisticRegressioncheckBox.setChecked(False)
            self.ui.lrEdit.setText(cp.split('l:')[1].split(';')[0])
            if "confidence" in self.analysis.category :
                if self.analysis.category == "confidence_posterior_global":
                    if "z:" in cp :
                        self.ui.posteriorDistributionSampleSizeEdit.setText(cp.split('z:')[1].split(';')[0])
                    if "c:" in  cp :
                        self.ui.notdsEdit.setText(cp.split('c:')[1].split(';')[0])
                elif  self.analysis.category == "confidence_prior_global":
                    if "b:" in cp :
                        self.ui.notdsEdit.setText(cp.split('b:')[1].split(';')[0])
                elif  self.analysis.category in ["confidence_prior_specific", "confidence"]:
                    if "t:" in cp :
                        self.ui.notdsEdit.setText(cp.split('t:')[1].split(';')[0])



    def checkAll(self):
        problems = ""
        try:
            if "confidence" in self.analysis.category :
                notds = int(self.ui.notdsEdit.text())
            if self.analysis.category == "confidence_posterior_global":
                linearRegrassion = int(self.ui.posteriorDistributionSampleSizeEdit.text())
            lr = int(self.ui.lrEdit.text())
            de = int(self.ui.deEdit.text())
            cnosd = int(self.ui.cnosdEdit.text())

        except Exception as e:
            problems += "Only non-empty integer values are accepted\n"

        if problems == "":
            return True
        else:
            output.notify(self,"value problem",problems)
            return False

    def validate(self):
        """ defini les computation parameters de l'analyse et ajoute celle-ci au projet
        """
        analysis_in_edition = (self.analysis.computationParameters != "")
        self.analysis.candidateScList = self.scNumList
        self.majDicoValues()
        chosen_scs_txt = ""
        for cs in self.scNumList:
            chosen_scs_txt+="%s,"%str(cs)
        chosen_scs_txt = chosen_scs_txt[:-1]
        if self.checkAll():
            if self.analysis.category == "compare":
                self.analysis.computationParameters = "s:%s;n:%s;d:%s;l:%s;m:%s;f:%s"%(chosen_scs_txt,self.dico_values['choNumberOfsimData'],self.dico_values['de'],self.dico_values['lr'],self.dico_values['numReg'],self.analysis.fda)
            elif "confidence" in self.analysis.category : # == "confidence":

                # FIXED :
               # "/home/dehneg/diyabc/src-JMC-C++/general"
               # -p "/media/psf/Home/VMshare/example_2_microsat_sequence_data_complexe_scenarios_ghost_pop_project_2013_7_9-1/"
               # -f "s:1,2;r:1;n:203900;m:1;d:501;l:2039;t:509;f:0;h:t2=25500.0 t1=5005.0 ra=0.5 NS1=25500.0 N1=25500.0 N2=25500.0
               #         N3=5005.0 NS2=25500.0 ta=5005.0;u:g1(0.00055 2 0.2 2 5.005e-06 2)*g2(0.00055 2 0.2 2 5.005e-06 2)*g3(0.00055
               #         2 0.2 2 5.005e-06 2)*g4(5.5e-09 2 10.025 2)*g5(5.05e-08 2 10.025 2)"
               # -i fixed
               # -g 509
               # -m
               # -t 1


               #PRIOR
               #"/home/dehneg/diyabc/src-JMC-C++/general"
               # -p "/media/psf/Home/VMshare/example_2_microsat_sequence_data_complexe_scenarios_ghost_pop_project_2013_7_9-1/"
               # -f "s:1,2;r:1;n:203900;m:1;d:501;l:2039;t:509;f:0;h:t2=UN[1000,50000.0,0.0,0.0] t1=UN[10.0,10000.0,0.0,0.0] ra=UN[0.001,0.999,0.0,0.0]
               #    NS1=UN[1000,50000.0,0.0,0.0] N1=UN[1000,50000.0,0.0,0.0] N2=UN[1000,50000.0,0.0,0.0] N3=UN[10,10000.0,0.0,0.0]
               #    NS2=UN[1000,50000.0,0.0,0.0] ta=UN[10.0,10000.0,0.0,0.0] ta>t1 t2>ta;u:g1(UN[1.00E-004,1.00E-3,0.0005,2]
               #    GA[1.00E-005,1.00E-002,Mean_u,2] UN[1.00E-001,3.00E-001,0.22,2] GA[1.00E-002,9.00E-001,Mean_P,2]
               #    LU[1.00E-008,1.00E-005,1.00E-007,2] GA[1.00E-009,1.00E-004,Mean_u_SNI,2])*g2(UN[1.00E-004,1.00E-3,0.0005,2]
               #    GA[1.00E-005,1.00E-002,Mean_u,2] UN[1.00E-001,3.00E-001,0.22,2] GA[1.00E-002,9.00E-001,Mean_P,2]
               #    LU[1.00E-008,1.00E-005,1.00E-007,2] GA[1.00E-009,1.00E-004,Mean_u_SNI,2])*g3(UN[1.00E-004,1.00E-3,0.0005,2]
               #    GA[1.00E-005,1.00E-002,Mean_u,2] UN[1.00E-001,3.00E-001,0.22,2] GA[1.00E-002,9.00E-001,Mean_P,2]
               #    LU[1.00E-008,1.00E-005,1.00E-007,2]
               #    GA[1.00E-009,1.00E-004,Mean_u_SNI,2])*g4(UN[1.00E-9,1.00E-8,5E-9,2] GA[1.00E-10,1.00E-7,Mean_u,2] UN[0.050,20,10,2]
               #    GA[0.050,20,Mean_k1,2] UN[0.050,20,10,2] GA[0.050,20,Mean_k2,2])*g5(UN[1.00E-9,1.00E-7,5E-9,2] GA[1.00E-9,1.00E-6,Mean_u,2]
               #    UN[0.050,20,10,2] GA[0.050,20,Mean_k1,2] UN[0.050,20,10,2] GA[0.050,20,Mean_k2,2])"
               # -i prior
               # -g 509 -m -t 1

                candListTxt = ""
                for cs in self.analysis.candidateScList:
                    candListTxt+="%s,"%str(cs)
                candListTxt = candListTxt[:-1]
                strparam = ""
                strparam += "n:%s;"%self.dico_values['choNumberOfsimData']
                strparam += "m:%s;"%self.dico_values['numReg']
                strparam += "d:%s;"%self.dico_values['de']
                strparam += "f:%s;"%self.analysis.fda

                if self.analysis.category == "confidence_posterior_global":
                    strparam += "z:%s;" % self.dico_values['linearRegression']
                    strparam += "l:%s;" % self.dico_values['lr']
                    strparam += "c:%s;"%self.dico_values['notds']
                    strparam += ""
                elif  self.analysis.category == "confidence_prior_global":
                    strparam += "l:%s;"%self.dico_values['lr']
                    strparam += "b:%s;"%self.dico_values['notds']
                elif  self.analysis.category == "confidence_prior_specific":
                    strparam += "s:%s;"%candListTxt
                    strparam += "r:%s;"%self.analysis.chosenSc
                    strparam += "l:%s;"%self.dico_values['lr']
                    strparam += "t:%s;"%self.dico_values['notds']
                    if self.analysis.drawn is 'posterior':
                        strparam += "po;z:%s;a:%s"% (self.dico_values['posteriorNumDataSets'], self.dico_values['posteriorSimNumDataSets'])
                    else :
                        strparam += "h:"
                        if self.analysis.drawn:
                            for paramname in self.analysis.histParamsDrawn.keys():
                                l = self.analysis.histParamsDrawn[paramname]
                                strparam += "%s="%paramname
                                strparam += "%s[%s,%s,%s,%s] "%(l[1],l[2],l[3],l[4],l[5])
                            for ctxt in self.analysis.condTxtList:
                                strparam += "%s "%ctxt
                        else:
                            for paramname in self.analysis.histParamsFixed.keys():
                                l = self.analysis.histParamsFixed[paramname]
                                strparam += "%s="%paramname
                                strparam += "%s "%l[1]
                        strparam = strparam[:-1]
                        if self.analysis.drawn:
                            mutmod = self.analysis.mutationModelDrawn
                        else:
                            mutmod = self.analysis.mutationModelFixed
                        if len(mutmod)>0:
                            strparam += ";u:"
                            if type(mutmod[0]) == type(u'plop'):
                                for ind,gr in enumerate(mutmod):
                                    strparam += "g%s("%(ind+1)
                                    strgr = gr.strip()
                                    strgr = strgr.split('\n')
                                    for j,elem in enumerate(strgr):
                                        if elem.split()[0] != "MODEL":
                                            to_add = strgr[j].split()[1]
                                            strparam += "%s "%to_add
                                    # virer le dernier espace
                                    strparam = strparam[:-1]
                                    strparam += ")*"
                            else:
                                for ind,gr in enumerate(mutmod):
                                    strparam += "g%s("%(ind+1)
                                    for num in gr:
                                        strparam += "%s "%num
                                    # virer le dernier espace
                                    strparam = strparam[:-1]
                                    strparam += ")*"
                            # virer le dernier '-'
                            strparam = strparam[:-1]

                self.analysis.computationParameters = strparam
            if not analysis_in_edition:
                self.parents.addAnalysis(self.analysis)
            self.exit()


    def setRecordValues(self,scList):
        if scList in [None, False, [None], [False]] or len(scList) == 0 :
            scList = [x for x in range(1,self.parents.nb_sc+1)]
        sumRec = 0
        for i in scList:
            sumRec+=self.parents.readNbRecordsOfScenario(int(i))
        self.ui.cnosdEdit.setText(str(sumRec))
        # total number of simulated data default
        self.ui.totNumSimLabel.setText(str(sumRec))

        # logistic regression default
        onePc = sumRec / 100
        if onePc < 1000:
            if sumRec < 1000:
                onePc = sumRec
            else:
                onePc = 1000
        self.ui.lrEdit.setText(str(onePc))

        # linear regression default
        onePcScn = (sumRec / 100 ) / len(scList)
        self.ui.posteriorDistributionSampleSizeEdit.setText(str(onePcScn))


    def setCandidateScenarios(self,scList):
        """ ecrit la liste des scenarios candidats
        """
        if scList in [None, False, [None], [False]] or len(scList) == 0 :
            txt = ""
        else :
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
        """ ecrit la liste des scenarios à comparer
        """
        if scList in [None, False, [None], [False]] or len(scList) == 0 :
            txt = ""
        else :
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
        genSel = GenericScenarioSelection(len(self.parents.hist_model_win.scList),"Select the scenarios that you wish to compare",compFrame,"Comparison of scenarios",2,self.analysis,self.parent)
        self.ui.parents.analysisStack.addWidget(genSel)
        self.ui.parents.analysisStack.removeWidget(self)
        self.ui.parents.analysisStack.setCurrentWidget(genSel)

    def majDicoValues(self):
        self.dico_values["choNumberOfsimData"] = str(self.ui.cnosdEdit.text())
        self.dico_values["de"] = str(self.ui.deEdit.text())
        self.dico_values["linearRegression"] = str(self.ui.posteriorDistributionSampleSizeEdit.text())
        self.dico_values["lr"] = str(self.ui.lrEdit.text())
        self.dico_values["numReg"] = str(self.ui.numRegCombo.currentText())
        self.dico_values["notds"] = str(self.ui.notdsEdit.text())
        #look for posteriorDataSetNumberEdit in BiasNConfidenceScenarioSelection
        for child in  self.ui.parents.analysisStack.children():
            try :
                self.dico_values['posteriorNumDataSets'] = child.posteriorDataSetNumberEdit.text()
            except AttributeError as e :
                pass
            try :
                self.dico_values['posteriorSimNumDataSets'] = child.posteriorSimulatedDataSetNumberEdit.text()
            except AttributeError as e :
                pass

    def exit(self):
        ## reactivation des onglets
        self.ui.parents.analysisStack.removeWidget(self)
        self.ui.parents.analysisStack.setCurrentIndex(0)

