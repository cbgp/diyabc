# -*- coding: utf-8 -*-

import os
import codecs
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from setGenData import SetGeneticData
from setMutationModelRefTable import SetMutationModelRefTable
from setMutationModelSequencesRefTable import SetMutationModelSequencesRefTable
from setMutationModelSequences import SetMutationModelSequences
from setSummaryStatistics import SetSummaryStatistics
from setSummaryStatisticsSeq import SetSummaryStatisticsSeq
from visualizescenario import *
from data import *

class SetGeneticDataRefTable(SetGeneticData):
    """ set genetic data pour les informations concernant la reftable
    """
    def __init__(self,parent=None):
        super(SetGeneticDataRefTable,self).__init__(parent)


    def majProjectGui(self,m=None,s=None,g=None,ss=None):
        if m != None:
            self.parent.ui.nbMicrosatLabel.setText("%s microsatellite loci"%m)
        if s != None:
            self.parent.ui.nbSequencesLabel.setText("%s DNA sequence loci"%s)
        if g != None:
            self.parent.ui.nbGroupLabel.setText("%s locus groups"%g)
        if ss != None:
            self.parent.ui.nbSumStatsLabel.setText("%s summary statistics"%ss)


    def addGroup(self):
        """ surcharge de addgroup de la classe mère qui crée les mutation model et 
        les setsumstats 
        """
        groupBox = super(SetGeneticDataRefTable,self).addGroup()

        frame = SetMutationModelRefTable(self,groupBox)
        frame.setMutationConf(self.parent.parent.preferences_win.mutmodM.getMutationConf().split("\n"))
        self.setMutation_dico[groupBox] = frame
        frame.hide()

        frameSeq = SetMutationModelSequencesRefTable(self,groupBox)
        frameSeq.setMutationConf(self.parent.parent.preferences_win.mutmodS.getMutationConf().split('\n'))
        self.setMutationSeq_dico[groupBox] = frameSeq
        frameSeq.hide()

        frameSum = SetSummaryStatistics(self,groupBox)
        self.setSum_dico[groupBox] = frameSum
        frameSum.hide()

        frameSumSeq = SetSummaryStatisticsSeq(self,groupBox)
        self.setSumSeq_dico[groupBox] = frameSumSeq
        frameSumSeq.hide()

        return groupBox

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
        """ on switche dans la stack reftable
        """
        self.parent.ui.refTableStack.addWidget(widget)
        self.parent.ui.refTableStack.setCurrentWidget(widget)

    def exit(self):
        """ on ferme l'onglet 'set genetic data' et on retourne sur l'onglet principal du projet
        """
        ## reactivation des onglets
        #self.parent.setTabEnabled(0,True)
        #self.parent.setTabEnabled(1,True)
        #self.parent.removeTab(self.parent.indexOf(self))
        #self.parent.setCurrentIndex(0)
        self.parent.ui.refTableStack.removeWidget(self)
        self.parent.ui.refTableStack.setCurrentIndex(0)

    def validate(self):
        """ clic sur le bouton validate
        verifie la validité de tous les groupes (mutmodel valide && nbsumstats > 0 && nblocus > 0)
        si valide : sort , maj de l'icone de validité et sauve
        si non valide : informe et sauve
        """
        #print self.group_info_dico
        #print self.dico_num_and_numgroup_locus
        problem = u""
        for i,box in enumerate(self.groupList):
            title = str(box.title())
            if "Microsatellites" in title:
                if box not in self.setMutationValid_dico.keys() or not self.setMutationValid_dico[box]:
                    problem += u"Mutation model of group %s is not considered as valid\n"%(i+1)
                # recup du nb de stats
                (nstat,stat_txt) = self.setSum_dico[box].getSumConf()
                if nstat == 0:
                    problem += u"No summary statistic asked for group %s\n"%(i+1)
            elif "Sequences" in title:
                if box not in self.setMutationSeqValid_dico.keys() or not self.setMutationSeqValid_dico[box]:
                    problem += u"Mutation model of group %s is not considered as valid\n"%(i+1)
                # recup du nb de stats
                (nstat,stat_txt) = self.setSumSeq_dico[box].getSumConf()
                if nstat == 0:
                    problem += u"No summary statistic asked for group %s\n"%(i+1)
            else:
                problem += u"Group %s is empty\n"%(i+1)
        if problem != u"":
            QMessageBox.information(self,"Impossible to validate the genetic data",problem)
            self.parent.setGenValid(False)
        else:
            self.exit()
            self.parent.setGenValid(True)
        # dans tous les cas, on écrit la conf
        self.writeGeneticConfFromGui()

    def clear(self):
        """ On supprime tous les groupes
        """
        #for i in range(len(self.groupList)):
        #    self.groupList[i].hide()
        for box in self.groupList:
            box.hide()
            lw = box.findChild(QListWidget,"listWidget")
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
        if os.path.exists(self.parent.dir+"/conf.gen.tmp"):
            os.remove("%s/conf.gen.tmp" % self.parent.dir)

        f = codecs.open(self.parent.dir+"/conf.gen.tmp",'w',"utf-8")
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
                    f.write(u'%s'%self.setMutationSeq_dico[box].getMutationConf())

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

        f.write("\ngroup summary statistics (%i)\n"%nstat_tot)
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
            if os.path.exists("%s/conf.gen.tmp"%(self.parent.dir)):
                f = codecs.open("%s/conf.gen.tmp"%(self.parent.dir),"r","utf-8")
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
                    ltype = lines[l].split(' ')[1]
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
                    nb_groupes = int(lines[l].split('(')[1].split(')')[0])
                    num_group_max_done = 0
                    l+=1
                    # on va jusqu'à la prochaine ligne vide ou la fin du fichier
                    while l<len(lines) and lines[l].strip() != "":
                        #print 'prem ligne du groupe:',lines[l]
                        num_group = int(lines[l].split('group G')[1].split(' ')[0])
                        num_group_max_done = num_group
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
                    nb_sum_stat = int(lines[l].split('(')[1].split(')')[0])
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


    def getParamTableHeader(self):
        """ retourne la chaine des noms des paramètres pour le conf.th.tmp
        """
        result = u""
        for box in self.groupList:
            if "Microsatellites" in str(box.title()):
                params_txt = self.setMutation_dico[box].getParamTableHeader()
            elif "Sequences" in str(box.title()):
                params_txt = self.setMutationSeq_dico[box].getParamTableHeader()
            result += params_txt
        return result
    def getSumStatsTableHeader(self):
        result = u""
        for box in self.groupList:
            if "Microsatellites" in str(box.title()):
                sums_txt = self.setSum_dico[box].getSumStatsTableHeader()
            elif "Sequences" in str(box.title()):
                sums_txt = self.setSumSeq_dico[box].getSumStatsTableHeader()
            result += sums_txt
        return result


    def clearMutationModel(self,box):
        self.setMutation_dico[box].exit()
        self.setMutation_dico[box] = SetMutationModelRefTable(self,box)
        self.setMutation_dico[box].setMutationConf(self.parent.parent.preferences_win.mutmodM.getMutationConf().split("\n"))
        self.setMutation(box)

    def clearMutationModelSeq(self,box):
        self.setMutationSeq_dico[box].exit()
        self.setMutationSeq_dico[box] = SetMutationModelSequences(self,box)
        self.setMutationSeq_dico[box].setMutationConf(self.parent.parent.preferences_win.mutmodS.getMutationConf().split('\n'))
        self.setMutation(box)

    def clearSummaryStats(self,box):
        self.setSum_dico[box].exit()
        self.setSum_dico[box] = SetSummaryStatistics(self,box)
        self.setSum(box)

    def clearSummaryStatsSeq(self,box):
        self.setSumSeq_dico[box].exit()
        self.setSumSeq_dico[box] = SetSummaryStatisticsSeq(self,box)
        self.setSum(box)


