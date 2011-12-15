# -*- coding: utf-8 -*-

import os
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from setGenData import SetGeneticData
from mutationModel.setMutationModelMsatAnalysis import SetMutationModelMsatAnalysis
from mutationModel.setMutationModelMsatFixed import SetMutationModelMsatFixed
from mutationModel.setMutationModelSequencesFixed import SetMutationModelSequencesFixed
from mutationModel.setMutationModelSequencesAnalysis import SetMutationModelSequencesAnalysis
#from summaryStatistics.setSummaryStatisticsMsat import SetSummaryStatisticsMsat
#from summaryStatistics.setSummaryStatisticsSeq import SetSummaryStatisticsSeq
from analysis.setupEstimationBias import SetupEstimationBias
from analysis.setupComparisonConfidence import SetupComparisonConfidence
from utils.visualizescenario import *
from utils.data import *
from utils.cbgpUtils import log

class SetGeneticDataAnalysis(SetGeneticData):
    """ set genetic data pour les informations concernant une analyse bias ou confidence
    """
    def __init__(self,analysis,parent=None):
        super(SetGeneticDataAnalysis,self).__init__(parent)

        self.analysis = analysis

        self.ui.addGroupButton.hide()
        self.ui.clearButton.hide()

        self.fillLocusTableFromData()

        gen_data_ref = self.parent.gen_data_win
        # copie des valeurs de range et size
        for locusrow in range(gen_data_ref.ui.tableWidget.rowCount()):
            self.ui.tableWidget.setItem(locusrow,2,QTableWidgetItem("%s"%gen_data_ref.ui.tableWidget.item(locusrow,2).text()))
            self.ui.tableWidget.setItem(locusrow,3,QTableWidgetItem("%s"%gen_data_ref.ui.tableWidget.item(locusrow,3).text()))

        # remplissage des groupes et set des mutation model et sum stats ac valeurs
        for numgr,box in enumerate(gen_data_ref.groupList):
            title = gen_data_ref.group_info_dico[box][0]
            locus_list = gen_data_ref.group_info_dico[box][1]
            self.addGroup()
            self.groupList[-1].setTitle("Group %s : %s"%(len(self.groupList),title))
            self.groupList[-1].findChild(QPushButton,"rmLocusGroupButton").hide()

            # selection pour ajouter dans le groupe
            for locus in locus_list:
                #index = gen_data_ref.dico_num_and_numgroup_locus[locus]
                self.ui.tableWidget.setItemSelected(self.ui.tableWidget.item(locus-1,0),True)
            self.addToGroup(self.groupList[-1])

            # creation des objets mutation model
            if self.analysis.drawn == True:
                # on instancie des mutationmodel normaux (mais fait pour analysis)
                if "Microsatellites" in str(box.title()):
                    self.setMutation_dico[self.groupList[-1]] = SetMutationModelMsatAnalysis(self,self.groupList[-1])
                    self.setMutation_dico[self.groupList[-1]].hide()
                    # on transfere la conf depuis le mutmod reftable
                    #print "mut conf : %s"%gen_data_ref.setMutation_dico[box].getMutationConf()
                    self.setMutation_dico[self.groupList[-1]].setMutationConf(gen_data_ref.setMutation_dico[box].getMutationConf().split('\n'))
                    self.setMutationValid_dico[self.groupList[-1]] = True
                    # on grise ce qui ne varie pas
                    if float(str(gen_data_ref.setMutation_dico[box].mmrMinEdit.text())) == float(str(gen_data_ref.setMutation_dico[box].mmrMaxEdit.text())):
                        self.setMutation_dico[self.groupList[-1]].frame_5.setDisabled(True)
                    if float(str(gen_data_ref.setMutation_dico[box].mcpMinEdit.text())) == float(str(gen_data_ref.setMutation_dico[box].mcpMaxEdit.text())):
                        self.setMutation_dico[self.groupList[-1]].frame_2.setDisabled(True)
                    if float(str(gen_data_ref.setMutation_dico[box].msrMinEdit.text())) == float(str(gen_data_ref.setMutation_dico[box].msrMaxEdit.text())):
                        self.setMutation_dico[self.groupList[-1]].frame_7.setDisabled(True)

                elif "Sequences" in str(box.title()):
                    self.setMutationSeq_dico[self.groupList[-1]] = SetMutationModelSequencesAnalysis(self,self.groupList[-1])
                    self.setMutationSeq_dico[self.groupList[-1]].hide()
                    # on transfere la conf depuis le mutmod reftable
                    #print "mut conf : %s"%gen_data_ref.setMutationSeq_dico[box].getMutationConf()
                    self.setMutationSeq_dico[self.groupList[-1]].setMutationConf(gen_data_ref.setMutationSeq_dico[box].getMutationConf().split('\n'))
                    self.setMutationSeqValid_dico[self.groupList[-1]] = True
                    # on grise ce qui ne varie pas
                    if float(str(gen_data_ref.setMutationSeq_dico[box].mmrMinEdit.text())) == float(str(gen_data_ref.setMutationSeq_dico[box].mmrMaxEdit.text())):
                        self.setMutationSeq_dico[self.groupList[-1]].firstFrame.setDisabled(True)
                    if float(str(gen_data_ref.setMutationSeq_dico[box].mc1MinEdit.text())) == float(str(gen_data_ref.setMutationSeq_dico[box].mc1MaxEdit.text())):
                        self.setMutationSeq_dico[self.groupList[-1]].thirdFrame.setDisabled(True)
                    if float(str(gen_data_ref.setMutationSeq_dico[box].mc2MinEdit.text())) == float(str(gen_data_ref.setMutationSeq_dico[box].mc2MaxEdit.text())):
                        self.setMutationSeq_dico[self.groupList[-1]].fifthFrame.setDisabled(True)

            else:
                # on instancie des mutationmodel fixed
                if "Microsatellites" in str(box.title()):
                    self.setMutation_dico[self.groupList[-1]] = SetMutationModelMsatFixed(self,self.groupList[-1])
                    self.setMutation_dico[self.groupList[-1]].hide()
                    # on transfere la conf depuis le mutmod reftable
                    #print "mut conf : %s"%gen_data_ref.setMutation_dico[box].getMutationConf()
                    self.setMutation_dico[self.groupList[-1]].setMutationConf(gen_data_ref.setMutation_dico[box].getMutationConf().split('\n'))
                    self.setMutationValid_dico[self.groupList[-1]] = True
                    # on grise ce qui ne varie pas
                    if float(str(gen_data_ref.setMutation_dico[box].mmrMinEdit.text())) == float(str(gen_data_ref.setMutation_dico[box].mmrMaxEdit.text())):
                        self.setMutation_dico[self.groupList[-1]].frame_5.setDisabled(True)
                    if float(str(gen_data_ref.setMutation_dico[box].mcpMinEdit.text())) == float(str(gen_data_ref.setMutation_dico[box].mcpMaxEdit.text())):
                        self.setMutation_dico[self.groupList[-1]].frame_2.setDisabled(True)
                    if float(str(gen_data_ref.setMutation_dico[box].msrMinEdit.text())) == float(str(gen_data_ref.setMutation_dico[box].msrMaxEdit.text())):
                        self.setMutation_dico[self.groupList[-1]].frame_7.setDisabled(True)
                elif "Sequences" in str(box.title()):
                    self.setMutationSeq_dico[self.groupList[-1]] = SetMutationModelSequencesFixed(self,self.groupList[-1])
                    self.setMutationSeq_dico[self.groupList[-1]].hide()
                    # on transfere la conf depuis le mutmod reftable
                    #print "mut conf : %s"%gen_data_ref.setMutationSeq_dico[box].getMutationConf()
                    self.setMutationSeq_dico[self.groupList[-1]].setMutationConf(gen_data_ref.setMutationSeq_dico[box].getMutationConf().split('\n'))
                    self.setMutationSeqValid_dico[self.groupList[-1]] = True
                    # on grise ce qui ne varie pas
                    if float(str(gen_data_ref.setMutationSeq_dico[box].mmrMinEdit.text())) == float(str(gen_data_ref.setMutationSeq_dico[box].mmrMaxEdit.text())):
                        self.setMutationSeq_dico[self.groupList[-1]].firstFrame.setDisabled(True)
                    if float(str(gen_data_ref.setMutationSeq_dico[box].mc1MinEdit.text())) == float(str(gen_data_ref.setMutationSeq_dico[box].mc1MaxEdit.text())):
                        self.setMutationSeq_dico[self.groupList[-1]].thirdFrame.setDisabled(True)
                    if float(str(gen_data_ref.setMutationSeq_dico[box].mc2MinEdit.text())) == float(str(gen_data_ref.setMutationSeq_dico[box].mc2MaxEdit.text())):
                        self.setMutationSeq_dico[self.groupList[-1]].fifthFrame.setDisabled(True)



        for box in self.groupList:
            box.findChild(QPushButton,"addToGroupButton").hide()
            box.findChild(QPushButton,"rmFromGroupButton").hide()
            box.findChild(QPushButton,"setSumButton").hide()



    def majProjectGui(self,m=None,s=None,g=None,ss=None):
        pass
    #    if m != None:
    #        self.parent.ui.nbMicrosatLabel.setText("%s microsatellite loci"%m)
    #    if s != None:
    #        self.parent.ui.nbSequencesLabel.setText("%s DNA sequence loci"%s)
    #    if g != None:
    #        self.parent.ui.nbGroupLabel.setText("%s locus groups"%g)
    #    if ss != None:
    #        self.parent.ui.nbSumStatsLabel.setText("%s summary statistics"%ss)


    def setSum(self,box=None):
        """ déclenché par le clic sur le bouton 'set summary statistics' ou par le clic sur 'clear'
        dans 'set summary statistics'. bascule vers la saisie des summary statistics
        """
        if box == None:
            box = self.sender().parent()
        title = str(box.title())
        if "Microsatellites" in title:
            #self.parent.addTab(self.setSum_dico[box],"Set summary statistics")
            #self.parent.setTabEnabled(self.parent.indexOf(self),False)
            #self.parent.setCurrentWidget(self.setSum_dico[box])
            self.switchTo(self.setSum_dico[box])
            # maj du titre de la frame
            lab = self.setSum_dico[box].ui.sumStatLabel
            lab.setText("Set summary statistics of %s (microsatellites)"%(" ".join(str(box.title()).split()[:2])))
        elif "Sequences" in title:
            #self.parent.addTab(self.setSumSeq_dico[box],"Set summary statistics")
            #self.parent.setTabEnabled(self.parent.indexOf(self),False)
            #self.parent.setCurrentWidget(self.setSumSeq_dico[box])
            self.switchTo(self.setSumSeq_dico[box])
            # maj du titre de la frame
            lab = self.setSumSeq_dico[box].ui.sumStatLabel
            lab.setText("Set summary statistics of %s (sequences)"%(" ".join(str(box.title()).split()[:2])))
        else:
            QMessageBox.information(self,"Set Summary statistics error","Add locus to a group before setting the summary statistics")



    def switchTo(self,widget):
        """ on switche dans la stack analysis
        """
        self.parent.ui.analysisStack.addWidget(widget)
        self.parent.ui.analysisStack.setCurrentWidget(widget)

    def exit(self):
        ## reactivation des onglets
        #self.parent.setTabEnabled(0,True)
        #self.parent.setTabEnabled(1,True)
        #self.parent.removeTab(self.parent.indexOf(self))
        #self.parent.setCurrentIndex(1)
        self.parent.ui.analysisStack.removeWidget(self)
        self.parent.ui.analysisStack.setCurrentIndex(0)

    def validate(self):
        """ clic sur le bouton validate
        verifie la validité de tous les groupes (mutmodel valide && nbsumstats > 0 && nblocus > 0)
        si valide : sort , maj de l'icone de validité et sauve
        si non valide : informe et sauve
        """
        log(3,"Validation of Genetic Data of analysis")
        #print self.group_info_dico
        #print self.dico_num_and_numgroup_locus
        mutconf_list = []
        problem = u""
        for i,box in enumerate(self.groupList):
            title = str(box.title())
            if "Microsatellites" in title:
                mutconf_list.append(self.setMutation_dico[box].getMutationConf())
                if box not in self.setMutationValid_dico.keys() or not self.setMutationValid_dico[box]:
                    problem += u"Mutation model of group %s is not considered as valid\n"%(i+1)
            elif "Sequences" in title:
                mutconf_list.append(self.setMutationSeq_dico[box].getMutationConf())
                if box not in self.setMutationSeqValid_dico.keys() or not self.setMutationSeqValid_dico[box]:
                    problem += u"Mutation model of group %s is not considered as valid\n"%(i+1)
            else:
                problem += u"Group %s is empty\n"%(i+1)
        if problem != u"":
            QMessageBox.information(self,"Impossible to validate the genetic data",problem)
            self.parent.setGenValid(False)
        else:
            # c'est valide, on passe à la dernière phase de paramètrage
            self.analysis.mutationModel = mutconf_list
            if self.analysis.category == "bias":
                #next_title = "bias and precision"
                last_parametrisation = SetupEstimationBias(self.analysis,self)
            else:
                #next_title = "evaluate confidence"
                last_parametrisation = SetupComparisonConfidence(self.analysis,self)
            #self.parent.addTab(last_parametrisation,next_title)
            #self.parent.removeTab(self.parent.indexOf(self))
            #self.parent.setCurrentWidget(last_parametrisation)
            self.parent.ui.analysisStack.addWidget(last_parametrisation)
            self.parent.ui.analysisStack.removeWidget(self)
            self.parent.ui.analysisStack.setCurrentWidget(last_parametrisation)
            #self.parent.addAnalysis(self.analysis)
            #self.exit()

    def clear(self):
        """ On supprime tous les groupes
        """
        #for i in range(len(self.groupList)):
        #    self.groupList[i].hide()
        for box in self.groupList:
            box.hide()
            lw = box.findChild(QListWidget,"locusGroupList")
            for row in range(lw.count()):
                name = str(lw.item(row).text())
                #num = int(name.split(' ')[1])
                num = self.dico_num_and_numgroup_locus[name][0]
                # on decouvre la ligne
                self.ui.tableWidget.setRowHidden(num-1,False)
        self.groupList = []

    def writeGeneticConfFromGui(self):
        """ on ecrit l'etat actuel des genetic data dans conf.gen.tmp
        """
        if os.path.exists(self.parent.dir+"/%s"%self.parent.gen_conf_name):
            os.remove("%s/%s"%(self.parent.dir,self.parent.gen_conf_name))

        f = codecs.open(self.parent.dir+"/%s"%self.parent.gen_conf_name,'w',"utf-8")
        # partie description des locus
        f.write("loci description (%i)\n"%self.nbLocusGui)
        data = self.parent.data
        for i in range(data.nloc):
            name = data.locuslist[i].name
            type = data.locuslist[i].type
            group = self.dico_num_and_numgroup_locus[name][1]
            if type > 4:
                microSeq = "S"
                typestr = LOC_TYP[type-5]
                length = data.locuslist[i].dnalength
                f.write("%s %s [%s] G%i %s\n"%(name.replace(' ','_'),typestr,microSeq,group,length))
            else:
                microSeq = "M"
                typestr = LOC_TYP[type]
                indice_dans_table = self.dico_num_and_numgroup_locus[name][0]
                motif_size = str(self.ui.tableWidget.item(indice_dans_table-1,2).text())
                motif_range = str(self.ui.tableWidget.item(indice_dans_table-1,3).text()).strip()
                f.write("%s %s [%s] G%i %s %s\n"%(name.replace(' ','_'),typestr,microSeq,group,motif_size,motif_range))

        # partie mutation model
        f.write("\ngroup priors (%i)\n"%len(self.groupList))
        if len(self.groupList) == 0:
            f.write("\n")
        else:
            for i,box in enumerate(self.groupList):
                if "Microsatellites" in str(box.title()):
                    f.write("group G%i [M]\n"%(i+1))
                    f.write(u'%s'%self.setMutation_dico[box].getMutationConf())
                elif "Sequences" in str(box.title()):
                    f.write("group G%i [S]\n"%(i+1))
                    to_write = u'%s'%self.setMutationSeq_dico[box].getMutationConf()
                    f.write(to_write)

        # partie sum stats
        stat_txt_list = []
        nstat_tot = 0
        for i,box in enumerate(self.groupList):
            stat_txt = ""
            if "Microsatellites" in str(box.title()):
                (nstat_tmp,stat_txt) = self.setSum_dico[box].getSumConf()
                stat_header_txt = "group G%i [M] (%i)\n"%((i+1),nstat_tmp)
                nstat_tot += nstat_tmp
                # construction de la chaine a ecrire pour ce groupe 
                stat_to_write = stat_header_txt+stat_txt
            elif "Sequences" in str(box.title()):
                (nstat_tmp,stat_txt) = self.setSumSeq_dico[box].getSumConf()
                stat_header_txt = "group G%i [S] (%i)\n"%((i+1),nstat_tmp)
                nstat_tot += nstat_tmp
                # construction de la chaine a ecrire pour ce groupe 
                stat_to_write = stat_header_txt+stat_txt
            stat_txt_list.append(stat_to_write)

        f.write("group summary statistics (%i)\n"%nstat_tot)
        for txt in stat_txt_list:
            f.write(txt)
            #print txt
        f.write('\n')


    def loadGeneticConf(self):
        """ Charge le fichier conf.gen.tmp
        """
        # de toute façon, on rempli le tableau de locus
        #self.fillLocusTableFromData()
        if os.path.exists(self.parent.dir):
            if os.path.exists("%s/%s"%(self.parent.dir,self.parent.gen_conf_name)):
                f = codecs.open("%s/%s"%(self.parent.dir,self.parent.gen_conf_name),"r","utf-8")
                lines = f.readlines()
                nloc = int(lines[0].split('(')[1].split(')')[0])
                # determination du nombre de groupes
                l = 1
                gmax = 0
                while l < nloc+1:
                    g = int(lines[l].split(' ')[3].replace('G',''))
                    if g > gmax:
                        gmax = g
                    l+=1
                # creation du bon nombre de groupes
                for it in range(gmax):
                    self.addGroup()

                # recup des infos de chaque locus
                l = 1
                while l < nloc+1:
                    lname = lines[l].split(' ')[0].replace('_',' ')
                    #ltype = lines[l].split(' ')[1]
                    #ltype_num = LOC_TYP.index(ltype)
                    lmicroSeq = lines[l].split('[')[1].split(']')[0]
                    num_group = int(lines[l].split(' ')[3].replace('G',''))
                    # maj du dico
                    self.dico_num_and_numgroup_locus[lname] = [l,num_group]
                    if lmicroSeq == 'M':
                        # ajout ds le groupe
                        if num_group != 0:
                           self.ui.tableWidget.setItemSelected(self.ui.tableWidget.item(l-1,0),True)
                           self.addToGroup(self.groupList[num_group-1])
                        # maj des infos dans le table
                        ranger = lines[l].split(' ')[4]
                        motif = lines[l].split(' ')[5]
                        self.ui.tableWidget.setItem(l-1,2,QTableWidgetItem("%s"%(ranger)))
                        self.ui.tableWidget.setItem(l-1,3,QTableWidgetItem("%s"%(motif)))
                    else:
                        # ajout ds le groupe
                        if num_group != 0:
                           self.ui.tableWidget.setItemSelected(self.ui.tableWidget.item(l-1,0),True)
                           self.addToGroup(self.groupList[num_group-1])
                    l+=1
                # chargement des infos sur les groupes
                l+=1
                if l < len(lines):
                    #print 'ligne:',lines[l]
                    #nb_groupes = int(lines[l].split('(')[1].split(')')[0])
                    #num_group_max_done = 0
                    l+=1
                    # on va jusqu'à la prochaine ligne vide ou la fin du fichier
                    while l<len(lines) and lines[l].strip() != "":
                        #print 'prem ligne du groupe:',lines[l]
                        num_group = int(lines[l].split('group G')[1].split(' ')[0])
                        #num_group_max_done = num_group
                        type_group = lines[l].split('[')[1].split(']')[0]
                        l+=1
                        if type_group == 'S':
                            self.setMutationSeq_dico[self.groupList[num_group-1]].setMutationConf(lines[l:(l+7)])
                            l+=7
                        else:
                            self.setMutation_dico[self.groupList[num_group-1]].setMutationConf(lines[l:(l+6)])
                            l+=6
                    # on avance jusqu'au summary stats
                    while l<len(lines) and "group summary statistics" not in lines[l]:
                        l+=1
                    # on est sur la première ligne
                    #nb_sum_stat = int(lines[l].split('(')[1].split(')')[0])
                    #print "nbsumstat %s"%nb_sum_stat
                    l+=1
                    # parcours de tous les groupes
                    while l<len(lines) and lines[l].strip() != "":
                        num_group = int(lines[l].split('group G')[1].split(' ')[0])
                        type_group = lines[l].split('[')[1].split(']')[0]
                        l+=1
                        # parcours des lignes du groupe
                        lines_group = []
                        while l<len(lines) and "group" not in lines[l]:
                            lines_group.append(lines[l].strip())
                            l+=1
                        if len(lines_group) > 0:
                            if type_group == 'S':
                                self.setSumSeq_dico[self.groupList[num_group-1]].setSumConf(lines_group)
                            else:
                                self.setSum_dico[self.groupList[num_group-1]].setSumConf(lines_group)
                self.majProjectGui(ss=self.getNbSumStats())

    def clearMutationModel(self,box):
        self.setMutation_dico[box].exit()
        self.setMutation_dico[box] = SetMutationModelMsat(self,box)
        self.setMutation(box)

    def clearMutationModelSeq(self,box):
        self.setMutationSeq_dico[box].exit()
        self.setMutationSeq_dico[box] = SetMutationModelSequences(self,box)
        self.setMutation(box)

