# -*- coding: utf-8 -*-

import os,re
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.setupEstimationBias_ui import Ui_Frame
from genericScenarioSelection import GenericScenarioSelection
from summaryStatistics.setSummaryStatisticsMsatAnalysis import SetSummaryStatisticsMsatAnalysis
from summaryStatistics.setSummaryStatisticsSeqAnalysis import SetSummaryStatisticsSeqAnalysis
from summaryStatistics.setSummaryStatisticsSnpAnalysis import SetSummaryStatisticsSnpAnalysis
import output
from utils.cbgpUtils import log

formSetupEstimationBias,baseSetupEstimationBias = uic.loadUiType("uis/setupEstimationBias.ui")

class SetupEstimationBias(formSetupEstimationBias,baseSetupEstimationBias):
    """ dernière étape de définition d'une analyse de type estimation ou bias precision ou model checking
    """
    def __init__(self,analysis,parent=None):
        super(SetupEstimationBias,self).__init__(parent)
        self.parent=parent
        self.analysis = analysis
        self.scNumList = []
        self.dico_values = {}
        self.createWidgets()
        self.ui.verticalLayout_3.setAlignment(Qt.AlignHCenter)
        self.ui.verticalLayout_3.setAlignment(Qt.AlignTop)

        self.restoreAnalysisValues()

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        if self.analysis.category == "bias":
            self.ui.label.setText("Bias and mean square error")
            self.ui.redefButton.hide()
            self.ui.candidateLabel.hide()
            self.setScenarios([self.analysis.chosenSc])
            self.ui.redefSumStatsFrame.hide()
            self.ui.notdsEdit.setText("500")
        elif self.analysis.category == "modelChecking":
            self.ui.label.setText("Model Checking")
            self.ui.candidateLabel.hide()
            self.ui.notdsSubTitleLabel.hide()
            self.ui.notdsTitleLabel.hide()
            self.ui.notdsEdit.hide()
            self.ui.paramChoiceBox.hide()
            # remplissage de la liste des groupes
            groups = self.parent.parent.getLocusGroups()
            groups.sort()
            for i in groups:
                self.ui.redefSumStatsCombo.addItem(i)
        else:
            self.ui.candidateLabel.hide()
            self.ui.notdsSubTitleLabel.hide()
            self.ui.notdsTitleLabel.hide()
            self.ui.notdsEdit.hide()
            self.ui.redefSumStatsFrame.hide()

        self.ui.projectNameEdit.setText(self.parent.parent.dir)
        
        if self.parent.parent.isSnp():
            #self.ui.paramChoiceBox.hide()
            self.ui.cCheck.setChecked(False)
            self.ui.cCheck.hide()

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.redefButton,SIGNAL("clicked()"),self.redefineScenarios)
        QObject.connect(self.ui.redefSumStatsButton,SIGNAL("clicked()"),self.redefineSumStats)


        #nbSetsDone = str(self.parent.parent.ui.nbSetsDoneEdit.text()).strip()
        #self.ui.totNumSimLabel.setText(nbSetsDone)
        #self.ui.cnosdEdit.setText(nbSetsDone)
        #onePc = int(nbSetsDone) / 100
        #if onePc < 1000:
        #    if nbSetsDone < 1000:
        #        onePc = nbSetsDone
        #    else:
        #        onePc = 1000
        #self.ui.nosdEdit.setText(str(onePc))

        self.ui.analysisNameLabel.setText(self.analysis.name)

    def restoreAnalysisValues(self):
        if self.analysis.computationParameters != "":
            cp = self.analysis.computationParameters
            self.ui.nosdEdit.setText(cp.split('m:')[1].split(';')[0])
            self.ui.cnosdEdit.setText(cp.split('n:')[1].split(';')[0])

            trans = cp.split('t:')[1].split(';')[0]
            if trans == "1":
                self.ui.noRadio.setChecked(True)
            elif trans == "2":
                self.ui.logRadio.setChecked(True)
            elif trans == "3":
                self.ui.logitRadio.setChecked(True)
            elif trans == "4":
                self.ui.logtgRadio.setChecked(True)

            choice = cp.split('p:')[1].split(';')[0]
            self.ui.oCheck.setChecked('o' in choice)
            self.ui.sCheck.setChecked('s' in choice)
            self.ui.cCheck.setChecked('c' in choice)
            if self.analysis.category == "bias":
                self.ui.notdsEdit.setText(cp.split('d:')[1].split(';')[0])
            elif self.analysis.category == "modelChecking":
                self.ui.nodssftpEdit.setText(cp.split('q:')[1].split(';')[0])

    def redefineSumStats(self):
        """ appel de la methode de projectSnp ou projectMsatSeq
        qui appelera ensuite la bonne méthode de setupEstimationBias
        """
        self.parent.parent.redefineSumStatsAnalysis(self)

    def redefineSumStatsMsatSeq(self):
        """ clic sur le bouton de redéfinition des sumstats pour le groupe sélectionné
        """
        num_gr = int(self.ui.redefSumStatsCombo.currentText())
        # test pour savoir si msat ou seq
        groupList = self.parent.parent.gen_data_win.groupList
        greft = None
        for g in groupList:
            #print g.title()
            if "Group %s"%num_gr in g.title():
                greft = g
                break
        if "Microsat" in greft.title():
            sumStatFrame = SetSummaryStatisticsMsatAnalysis(self.parent,self,num_gr)
            #print self.parent.parent.gen_data_win.setSum_dico[g].getSumConf()[1]
            if self.analysis.sumStatsConfDico.has_key(num_gr):
                sumStatFrame.setSumConf(self.analysis.sumStatsConfDico[num_gr][1].strip().split('\n'))
            else:
                sumStatFrame.setSumConf(self.parent.parent.gen_data_win.setSum_dico[g].getSumConf()[1].strip().split('\n'))
        elif "Sequence" in greft.title():
            sumStatFrame = SetSummaryStatisticsSeqAnalysis(self.parent,self,num_gr)
            if self.analysis.sumStatsConfDico.has_key(num_gr):
                sumStatFrame.setSumConf(self.analysis.sumStatsConfDico[num_gr][1].strip().split('\n'))
            else:
                sumStatFrame.setSumConf(self.parent.parent.gen_data_win.setSumSeq_dico[g].getSumConf()[1].strip().split('\n'))
        else:
            return

        sumStatFrame.ui.sumStatLabel.setText("%s of group %s"%(sumStatFrame.ui.sumStatLabel.text(),num_gr))

        self.parent.parent.ui.analysisStack.addWidget(sumStatFrame)
        self.parent.parent.ui.analysisStack.setCurrentWidget(sumStatFrame)

    def redefineSumStatsSnp(self):
        """ clic sur le bouton de redéfinition des sumstats pour le groupe sélectionné
        """
        num_gr = str(self.ui.redefSumStatsCombo.currentText())

        sumStatFrame = SetSummaryStatisticsSnpAnalysis(self.parent,self,num_gr)
        if self.analysis.sumStatsConfDico.has_key(num_gr):
            sumStatFrame.setSumConf(self.analysis.sumStatsConfDico[num_gr][1].strip().split('\n'))
        else:
            sumStatFrame.setSumConf(self.parent.parent.sum_stat_wins[num_gr].getSumConf()[1].strip().split('\n'))

        sumStatFrame.ui.sumStatLabel.setText("%s"%(sumStatFrame.ui.sumStatLabel.text()))

        self.parent.parent.ui.analysisStack.addWidget(sumStatFrame)
        self.parent.parent.ui.analysisStack.setCurrentWidget(sumStatFrame)


    def checkAll(self):
        problems = ""
        try:
            #if (self.ui.totNumSimLabel.text() != "" or self.ui.totNumSimLabel.text() != "0") and int(self.ui.totNumSimLabel.text()) < int(self.ui.cnosdEdit.text()):
            #    problems += "Impossible to select more data than it exists in the reference table\n"
            if self.analysis.category == "bias":
                notds = int(self.ui.notdsEdit.text())
            elif self.analysis.category == "modelChecking":
                nodssftp = int(self.ui.nodssftpEdit.text())
            nosd = int(self.ui.nosdEdit.text())
            cnosd = int(self.ui.cnosdEdit.text())

        except Exception as e:
            problems += "Only non-empty integer values are accepted\n"

        if problems == "":
            return True
        else:
            output.notify(self,"value problem",problems)
            return False

    def validate(self):
        """ termine la définition de l'analyse en lui ajoutant la chaine
        de paramètres
        """
        analysis_in_edition = (self.analysis.computationParameters != "")
        #self.analysis.append(self.scNumList)
        self.majDicoValues()
        if self.checkAll():
            #self.analysis.append(self.dico_values)
            chosen_scs_txt = ""
            #print "analysis: %s"%self.analysis
            for cs in self.analysis.candidateScList:
                chosen_scs_txt+="%s,"%str(cs)
            chosen_scs_txt = chosen_scs_txt[:-1]
            #dico_est = self.analysis[-1]
            if self.analysis.category == "estimate" or self.analysis.category == "modelChecking":
                self.analysis.computationParameters = "s:%s;n:%s;m:%s;t:%s"%(chosen_scs_txt,self.dico_values['choNumberOfsimData'],self.dico_values['numberOfselData'],self.dico_values['transformation'])
                self.analysis.computationParameters += ";p:%s"%self.dico_values['choice']
                if self.analysis.category == "modelChecking":
                    pat = re.compile(r'\s+')
                    statsStr = ""
                    for k in self.analysis.sumStatsTHDico.keys():
                        statsStr += pat.sub(' ',self.analysis.sumStatsTHDico[k])
                        statsStr += " "

                    self.analysis.computationParameters += ";v:%s;q:%s;"%(statsStr.strip(),self.dico_values["numberOfDataFromPost"])
                    self.analysis.computationParameters += self.analysis.aParams
            elif self.analysis.category == "bias":
                strparam = "s:%s;"%self.analysis.chosenSc
                strparam += "n:%s;"%self.dico_values['choNumberOfsimData']
                strparam += "m:%s;"%self.dico_values['numberOfselData']
                strparam += "d:%s;"%self.dico_values['numberOfTestData']
                strparam += "t:%s;"%self.dico_values['transformation']
                strparam += "p:%s;"%self.dico_values['choice']
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
                #print "jaco:%s "%len(self.analysis[5]), self.analysis[5]
                if len(self.analysis.mutationModel)>0:
                    strparam += ";u:"
                    if type(self.analysis.mutationModel[0]) == type(u'plop'):
                        for ind,gr in enumerate(self.analysis.mutationModel):
                            strparam += "g%s("%(ind+1)
                            strgr = gr.strip()
                            strgr = strgr.split('\n')
                            #print "\nstrgr %s\n"%strgr
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
            if not analysis_in_edition:
                self.parent.parent.addAnalysis(self.analysis)
            self.exit()

    def setScenarios(self,scList):
        """ écrit la liste des scenarios à estimer
        """
        sumRec = 0
        #print scList
        plur= ""
        if len(scList)>1:
            plur = "s"
            
        lstxt=""
        self.scNumList = []
        for i in scList:
            lstxt+="%s, "%i
            sumRec+=self.parent.parent.readNbRecordsOfScenario(int(i))
            log(3,"Sum of scenarios records : %s"%sumRec)
            self.scNumList.append(i)
        lstxt = lstxt[:-2]

        txt = "Chosen scenario%s : %s"%(plur,lstxt)
        self.ui.scenariosLabel.setText(txt)
        self.ui.totNumSimLabel.setText(str(sumRec))
        # on le fait seulement si on n'est pas en édition
        if self.analysis.computationParameters == "":
            self.ui.cnosdEdit.setText(str(sumRec))
            nosdDefault = sumRec/100
            if nosdDefault < 1000:
                nosdDefault = 1000
            self.ui.nosdEdit.setText(str(nosdDefault))

    def redefineScenarios(self):
        """ repasse sur le choix des scenarios en lui redonnant moi même comme next_widget
        """
        estimateFrame = self
        #print type(self.parent)
        genSel = GenericScenarioSelection(len(self.parent.parent.hist_model_win.scList),"Parameters will be estimated considering data sets simulated with",estimateFrame,"ABC parameter estimation",1,self.analysis,self.parent)
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
        self.dico_values["numberOfTestData"] = str(self.ui.notdsEdit.text())
        self.dico_values["numberOfDataFromPost"] = str(self.ui.nodssftpEdit.text())
        choice = ""
        if self.ui.oCheck.isChecked():
            choice += "o"
        if self.ui.cCheck.isChecked():
            choice += "c"
        if self.ui.sCheck.isChecked():
            choice += "s"
        self.dico_values["choice"] = choice

    def exit(self):
        ## reactivation des onglets
        #self.parent.parent.setTabEnabled(1,True)
        #self.parent.parent.setTabEnabled(0,True)
        #self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        #self.parent.parent.setCurrentIndex(1)
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentIndex(0)

