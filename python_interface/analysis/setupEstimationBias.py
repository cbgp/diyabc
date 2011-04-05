# -*- coding: utf-8 -*-

import os,re
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from uis.setupEstimationBias_ui import Ui_Frame
from genericScenarioSelection import GenericScenarioSelection
from summaryStatistics.setSummaryStatisticsMsatAnalysis import SetSummaryStatisticsMsatAnalysis
from summaryStatistics.setSummaryStatisticsSeqAnalysis import SetSummaryStatisticsSeqAnalysis
import output

class SetupEstimationBias(QFrame):
    """ dernière étape de définition d'une analyse de type estimation ou bias precision
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



    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        if self.analysis.category == "bias":
            self.ui.label.setText("Bias and mean square error")
            self.ui.redefButton.hide()
            self.ui.candidateLabel.hide()
            self.setScenarios([self.analysis.chosenSc])
            self.ui.redefSumStatsFrame.hide()
        elif self.analysis.category == "modelChecking":
            self.ui.label.setText("Model Checking")
            self.ui.candidateLabel.hide()
            self.ui.label_5.hide()
            self.ui.label_4.hide()
            self.ui.notdsEdit.hide()
            self.ui.paramChoiceBox.hide()
        else:
            self.ui.candidateLabel.hide()
            self.ui.label_5.hide()
            self.ui.label_4.hide()
            self.ui.notdsEdit.hide()
            self.ui.redefSumStatsFrame.hide()

        self.ui.projectNameEdit.setText(self.parent.parent.dir)

        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.redefButton,SIGNAL("clicked()"),self.redefineScenarios)
        QObject.connect(self.ui.redefSumStatsButton,SIGNAL("clicked()"),self.redefineSumStats)

        # remplissage de la liste des groupes
        l_groups_to_add = []
        for gb in self.parent.parent.gen_data_win.groupList:
            if "Micro" in gb.title() or "Sequen" in gb.title():
                l_groups_to_add.append(str(gb.title()).split(' ')[1])

        l_groups_to_add.sort()
        for i in l_groups_to_add:
            self.ui.redefSumStatsCombo.addItem(i)

        nbSetsDone = str(self.parent.parent.ui.nbSetsDoneEdit.text()).strip()
        self.ui.totNumSimEdit.setText(nbSetsDone)

    def redefineSumStats(self):
        """ clic sur le bouton de redéfinition des sumstats pour le groupe sélectionné
        """
        num_gr = int(self.ui.redefSumStatsCombo.currentText())
        # test pour savoir si msat ou seq
        groupList = self.parent.parent.gen_data_win.groupList
        greft = None
        for g in groupList:
            print g.title()
            if "Group %s"%num_gr in g.title():
                greft = g
                break
        if "Microsat" in greft.title():
            sumStatFrame = SetSummaryStatisticsMsatAnalysis(self.parent,self,num_gr)
            print self.parent.parent.gen_data_win.setSum_dico[g].getSumConf()[1]
            sumStatFrame.setSumConf(self.parent.parent.gen_data_win.setSum_dico[g].getSumConf()[1].strip().split('\n'))
        elif "Sequence" in greft.title():
            sumStatFrame = SetSummaryStatisticsSeqAnalysis(self.parent,self,num_gr)
            sumStatFrame.setSumConf(self.parent.parent.gen_data_win.setSumSeq_dico[g].getSumConf()[1].strip().split('\n'))
        else:
            return

        sumStatFrame.ui.sumStatLabel.setText("%s of group %s"%(sumStatFrame.ui.sumStatLabel.text(),num_gr))

        self.parent.parent.ui.analysisStack.addWidget(sumStatFrame)
        self.parent.parent.ui.analysisStack.setCurrentWidget(sumStatFrame)


    def checkAll(self):
        problems = ""
        try:
            if self.ui.totNumSimEdit.text() != "" and int(self.ui.totNumSimEdit.text()) < int(self.ui.cnosdEdit.text()):
                problems += "Impossible to select more data than it exists in the reference table\n"
            if self.analysis.category == "bias":
                notds = int(self.ui.notdsEdit.text())
            nosd = int(self.ui.nosdEdit.text())
            cnosd = int(self.ui.cnosdEdit.text())

        except Exception,e:
            problems += "Only non-empty integer values are accepted\n"
        print "plop"

        if problems == "":
            return True
        else:
            output.notify(self,"value problem",problems)
            return False

    def validate(self):
        """ termine la définition de l'analyse en lui ajoutant la chaine
        de paramètres
        """
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
                self.analysis.computationParameters = "s:%s;n:%s;m:%s;t:%s;p:%s"%(chosen_scs_txt,self.dico_values['choNumberOfsimData'],self.dico_values['numberOfselData'],self.dico_values['transformation'],self.dico_values['choice'])
                if self.analysis.category == "modelChecking":
                    pat = re.compile(r'\s+')
                    statsStr = ""
                    for k in self.analysis.sumStatsDico.keys():
                        statsStr += pat.sub(' ',self.analysis.sumStatsDico[k])
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

    def setScenarios(self,scList):
        """ écrit la liste des scenarios à estimer
        """
        print scList
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
        print type(self.parent)
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
        self.dico_values["choice"] = choice

    def exit(self):
        ## reactivation des onglets
        #self.parent.parent.setTabEnabled(1,True)
        #self.parent.parent.setTabEnabled(0,True)
        #self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        #self.parent.parent.setCurrentIndex(1)
        self.parent.parent.ui.analysisStack.removeWidget(self)
        self.parent.parent.ui.analysisStack.setCurrentIndex(0)

