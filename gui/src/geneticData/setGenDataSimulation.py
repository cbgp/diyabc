# -*- coding: utf-8 -*-

import os
import sys, codecs, traceback
from PyQt4.QtCore import *
from PyQt4.QtGui import *

from utils.visualizescenario import *
from utils.data import *
from utils.cbgpUtils import log,getFsEncoding
from setGenData import SetGeneticData
from mutationModel.setMutationModelMsatSimulation import SetMutationModelMsatSimulation
from mutationModel.setMutationModelSequencesSimulation import SetMutationModelSequencesSimulation
import output

class SetGeneticDataSimulation(SetGeneticData):
    """ set genetic data pour une simulation de données
    """
    def __init__(self,parent=None):
        super(SetGeneticDataSimulation,self).__init__(parent)
        self.fsCoding = getFsEncoding(logLevel=False)

        self.ui.tableWidget.setColumnWidth(0,90)
        self.ui.tableWidget.setColumnWidth(1,40)
        self.ui.tableWidget.setColumnWidth(2,45)
        self.ui.tableWidget.setColumnWidth(3,45)
        self.ui.tableWidget.insertColumn(self.ui.tableWidget.columnCount())
        self.ui.tableWidget.setColumnWidth(4,50)
        self.ui.tableWidget.setHorizontalHeaderItem (4, QTableWidgetItem("length"))

        self.ui.tableWidget.insertColumn(self.ui.tableWidget.columnCount())
        self.ui.tableWidget.setColumnWidth(5,35)
        self.ui.tableWidget.setHorizontalHeaderItem (5, QTableWidgetItem("%A"))
        self.ui.tableWidget.insertColumn(self.ui.tableWidget.columnCount())
        self.ui.tableWidget.setColumnWidth(6,35)
        self.ui.tableWidget.setHorizontalHeaderItem (6, QTableWidgetItem("%C"))
        self.ui.tableWidget.insertColumn(self.ui.tableWidget.columnCount())
        self.ui.tableWidget.setColumnWidth(7,35)
        self.ui.tableWidget.setHorizontalHeaderItem (7, QTableWidgetItem("%G"))
        self.ui.tableWidget.insertColumn(self.ui.tableWidget.columnCount())
        self.ui.tableWidget.setColumnWidth(8,35)
        self.ui.tableWidget.setHorizontalHeaderItem (8, QTableWidgetItem("%T"))


    def fillLocusTable(self,dico):
        self.tableWidget.clearContents()
        tabindex = 0
        for beg in ["ma","mh","mx","my","mm","sa","sh","sx","sy","sm"]:
            for loctype in dico.keys():
                if loctype[:2] == beg:
                    if loctype[0] == 'm':
                        mainType = 'M'
                    else:
                        mainType = 'S'
                    for i in range(dico[loctype]):
                        self.addRow("%s_%s"%(loctype,i+1),mainType)
                        if mainType == 'S':
                            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,4,QTableWidgetItem('1000'))
                            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,5,QTableWidgetItem('25'))
                            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,6,QTableWidgetItem('25'))
                            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,7,QTableWidgetItem('25'))
                            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,8,QTableWidgetItem('25'))
                        else:
                            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,4,QTableWidgetItem(''))
                            self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,4).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,4).flags() & ~Qt.ItemIsEditable)
                            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,5,QTableWidgetItem(''))
                            self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,5).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,5).flags() & ~Qt.ItemIsEditable)
                            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,6,QTableWidgetItem(''))
                            self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,6).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,6).flags() & ~Qt.ItemIsEditable)
                            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,7,QTableWidgetItem(''))
                            self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,7).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,7).flags() & ~Qt.ItemIsEditable)
                            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,8,QTableWidgetItem(''))
                            self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,8).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,8).flags() & ~Qt.ItemIsEditable)
                        self.dico_num_and_numgroup_locus["%s_%s"%(loctype,i+1)] = [tabindex+1,0]
                        self.nbLocusGui+=1
                        tabindex += 1

    def majProjectGui(self,m=None,s=None,g=None,ss=None):
        if m != None:
            if int(m) > 1:
                plur='loci'
            else:
                plur='locus'
            self.parent.ui.nbMicrosatLabel.setText("%s microsatellite %s"%(m,plur))
        if s != None:
            if int(s) > 1:
                plur='loci'
            else:
                plur='locus'
            self.parent.ui.nbSequencesLabel.setText("%s DNA sequence %s"%(s,plur))
        if g != None:
            if int(g) > 1:
                plur='s'
            else:
                plur=''
            self.parent.ui.nbGroupLabel.setText("%s locus group%s"%(g,plur))
        if ss != None:
            if int(ss) > 1:
                plur='s'
            else:
                plur=''
            self.parent.ui.nbSumStatsLabel.setText("%s summary statistic%s"%(ss,plur))

    def setSum(self):
        pass

    def exitMutmod(self):
        self.switchTo(self)

    def addGroup(self):
        """ surcharge de addgroup de la classe mère qui crée les mutation model et
        les setsumstats
        """
        groupBox = super(SetGeneticDataSimulation,self).addGroup()

        frame = SetMutationModelMsatSimulation(self,groupBox)
        #frame.exit = self.exitMutmod
        #frame.setMutationConf(self.parent.parent.preferences_win.mutmodM.getMutationConf().split("\n"))
        self.setMutation_dico[groupBox] = frame
        frame.hide()
        self.parent.parent.updateDoc(frame)

        frameSeq = SetMutationModelSequencesSimulation(self,groupBox)
        #frameSeq.exit = self.exitMutmod
        #frameSeq.setMutationConf(self.parent.parent.preferences_win.mutmodS.getMutationConf().split('\n'))
        self.setMutationSeq_dico[groupBox] = frameSeq
        frameSeq.hide()
        self.parent.parent.updateDoc(frameSeq)

        groupBox.findChild(QPushButton,"setSumButton").hide()


        # on met les boutons en gras pour signaler qu'il faut les valider/définir
        set_mut_button = groupBox.findChild(QPushButton,"setMutationButton")
        fontt = set_mut_button.font()
        set_mut_button.setStyleSheet("background-color: #EFB1B3")
        fontt.setBold(True)
        #fontt.setPointSize(fontt.pointSize() + 1)
        set_mut_button.setFont(fontt)

        # maj de la doc pour ce groupe
        self.updateDoc(groupBox)

        return groupBox

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
        #print self.setMutationValid_dico
        #print self.setMutationSeqValid_dico

    def validate(self,silent=False):
        """ clic sur le bouton validate
        verifie la validité de tous les groupes (mutmodel valide && nbsumstats > 0 && nblocus > 0)
        si valide : sort , maj de l'icone de validité et sauve
        si non valide : informe et sauve
        """
        #print self.group_info_dico
        #print self.dico_num_and_numgroup_locus
        problem = u""
        if len(self.groupList) > 0:
            # verification des valeurs de motif et range
            problematicLocus = ""
            try:
                for i in range(self.ui.tableWidget.rowCount()):
                    # c'est un microsat
                    if str(self.ui.tableWidget.item(i,1).text())[0] == "M":
                        motif_size = int(str(self.ui.tableWidget.item(i,2).text()))
                        motif_range = int(str(self.ui.tableWidget.item(i,3).text()).strip())
                        ##print "locus %s  size:%s range:%s"%(i,motif_size,motif_range)
                        #mini = self.parent.data.locuslist[i].mini
                        #maxi = self.parent.data.locuslist[i].maxi
                        #kmoy = (mini + maxi)//2
                        #kmin = kmoy - (((motif_range/2)-1) * motif_size)
                        #kmax = kmin + ((motif_range-1) * motif_size)
                        #if (kmin > mini) or (kmax < maxi):
                        #    problematicLocus += "%s,"%(i+1)
                        #    log(4,"locus %s  size:%s range:%s   mini=%s maxi=%s kmoy=%s kmin=%s kmax=%s"%(self.parent.data.locuslist[i].name,motif_size,motif_range,mini,maxi,kmoy,kmin,kmax))
                    else:
                        length = int(str(self.ui.tableWidget.item(i,4).text()).strip())
                        pcA = int(str(self.ui.tableWidget.item(i,5).text()))
                        pcC = int(str(self.ui.tableWidget.item(i,6).text()))
                        pcG = int(str(self.ui.tableWidget.item(i,7).text()))
                        pcT = int(str(self.ui.tableWidget.item(i,8).text()))
                        if length <= 0:
                            problem += "Length has to be positive (locus %s)\n"%(i+1)
                        for val in [pcA,pcC,pcG,pcT]:
                            if val < 0 or val > 100:
                                problem += "Locus %s has incoherent values of nucleotide frequencies"%(i+1)
            except Exception as e:
                problem += "%s\n"%e
            #print "\n";
            if problematicLocus != "":
                problematicLocus = problematicLocus[:-1]
                problem += "Motif range at locus %s is not large enough to include all observed alleles\n"%problematicLocus

            for i,box in enumerate(self.groupList):
                title = str(box.title())
                if "Microsatellites" in title:
                    if box not in self.setMutationValid_dico.keys():
                        self.setMutationValid_dico[box] = self.setMutation_dico[box].allValid(silent=True)
                    if not self.setMutationValid_dico[box]:
                        problem += u"Mutation model of group %s is not considered as valid\n"%(i+1)
                    else:
                        log(3,"Mut model of group %s is valid"%box.title())
                elif "Sequences" in title:
                    if box not in self.setMutationSeqValid_dico.keys():
                        self.setMutationSeqValid_dico[box] = self.setMutationSeq_dico[box].allValid(silent=True)

                    if not self.setMutationSeqValid_dico[box]:
                        problem += u"Mutation model of group %s is not considered as valid\n"%(i+1)
                    else:
                        log(3,"Mut model of group %s is valid"%box.title())
                else:
                    problem += u"Group %s is empty\n"%(i+1)
        else:
            problem += "You must have at least one group of locus\n"
        if problem != u"":
            if not silent:
                output.notify(self,"Impossible to validate the genetic data",problem)
            self.parent.setGenValid(False)
        else:
            self.exit()
            self.parent.setGenValid(True)
        # dans tous les cas, on écrit la conf
        #self.writeGeneticConfFromGui()

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
        if os.path.exists((u"%s/%s"%(self.parent.dir,self.parent.parent.gen_conf_name)).encode(self.fsCoding)):
            os.remove((u"%s/%s" % (self.parent.dir,self.parent.parent.gen_conf_name)).encode(self.fsCoding))

        f = codecs.open((u"%s/%s"%(self.parent.dir,self.parent.parent.gen_conf_name)).encode(self.fsCoding),'w',"utf-8")
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
            if os.path.exists((u"%s/%s"%(self.parent.dir,self.parent.parent.gen_conf_name)).encode(self.fsCoding)):
                f = codecs.open((u"%s/%s"%(self.parent.dir,self.parent.parent.gen_conf_name)).encode(self.fsCoding),"rU","utf-8")
                lines = f.readlines()
                nloc = int(lines[0].split('(')[1].split(')')[0])
                # determination du nombre de groupes
                l = 1
                gmax = 0
                while l < nloc+1:
                    g = int(lines[l].split()[3].replace('G',''))
                    if g > gmax:
                        gmax = g
                    l+=1
                # creation du bon nombre de groupes
                for it in range(gmax):
                    self.addGroup()

                # recup des infos de chaque locus
                l = 1
                while l < nloc+1:
                    lname = lines[l].split()[0].replace('_',' ')
                    #ltype = lines[l].split(' ')[1]
                    #ltype_num = LOC_TYP.index(ltype)
                    lmicroSeq = lines[l].split('[')[1].split(']')[0]
                    num_group = int(lines[l].split()[3].replace('G',''))
                    # maj du dico
                    self.dico_num_and_numgroup_locus[lname] = [l,num_group]
                    if lmicroSeq == 'M':
                        # ajout ds le groupe
                        if num_group != 0:
                           self.ui.tableWidget.setItemSelected(self.ui.tableWidget.item(l-1,0),True)
                           self.addToGroup(self.groupList[num_group-1])
                        # maj des infos dans le table
                        ranger = lines[l].split()[5]
                        motif = lines[l].split()[4]
                        ran = QTableWidgetItem("%s"%(ranger))
                        mot = QTableWidgetItem("%s"%(motif))
                        ran.setTextAlignment(Qt.AlignRight|Qt.AlignVCenter)
                        mot.setTextAlignment(Qt.AlignRight|Qt.AlignVCenter)
                        self.ui.tableWidget.setItem(l-1,3,ran)
                        self.ui.tableWidget.setItem(l-1,2,mot)
                        log(3,"Loading a locus (%s)(%s)"%(mot.textAlignment(),ran.textAlignment()))
                    else:
                        # ajout ds le groupe
                        if num_group != 0:
                           self.ui.tableWidget.setItemSelected(self.ui.tableWidget.item(l-1,0),True)
                           self.addToGroup(self.groupList[num_group-1])
                    l+=1
                # chargement des infos sur les groupes
                if lines[l].strip() == "":
                    #print 'yep'
                    l+=1
                if l < len(lines):
                    #print 'ligne:',lines[l]
                    nb_groupes = int(lines[l].split('(')[1].split(')')[0])
                    num_group_max_done = 0
                    l+=1
                    # on va jusqu'à la prochaine ligne vide ou la fin du fichier
                    while l<len(lines) and lines[l].strip() != "" and num_group_max_done < nb_groupes:
                        log(4,'prem ligne du groupe:%s'%lines[l])
                        num_group = int(lines[l].split('group G')[1].split()[0])
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
                    if l<len(lines):
                        # on est sur la première ligne
                        #nb_sum_stat = int(lines[l].split('(')[1].split(')')[0])
                        pass
                    #print "nbsumstat %s"%nb_sum_stat
                    l+=1
                    # parcours de tous les groupes
                    while l<len(lines) and lines[l].strip() != "":
                        num_group = int(lines[l].split('group G')[1].split()[0])
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

    def getConf(self):
        loci_lines = []
        nb_loci_in_groups = 0
        for i in range(self.tableWidget.rowCount()):
            name = str(self.tableWidget.item(i,0).text())
            type = name[:3]
            group = self.dico_num_and_numgroup_locus[name][1]
            if int(group) != 0:
                nb_loci_in_groups += 1
                if type[0] == 's':
                    microSeq = "S"
                    typestr = type[1]
                    length = str(self.tableWidget.item(i,4).text())
                    pcA = str(self.tableWidget.item(i,5).text())
                    pcC = str(self.tableWidget.item(i,6).text())
                    pcG = str(self.tableWidget.item(i,7).text())
                    pcT = str(self.tableWidget.item(i,8).text())
                    loci_lines.append("%s <%s> [%s] G%i %s %s %s %s %s\n"%(name.replace(' ','_'),typestr.upper(),microSeq,group,length,pcA,pcC,pcG,pcT))
                else:
                    microSeq = "M"
                    typestr = type[1]
                    indice_dans_table = self.dico_num_and_numgroup_locus[name][0]
                    motif_size = str(self.ui.tableWidget.item(indice_dans_table-1,2).text())
                    motif_range = str(self.ui.tableWidget.item(indice_dans_table-1,3).text()).strip()
                    loci_lines.append("%s <%s> [%s] G%i %s %s\n"%(name.replace(' ','_'),typestr.upper(),microSeq,group,motif_size,motif_range))
        result = "loci description (%s)\n"%nb_loci_in_groups
        for beg in ["ma","mh","mx","my","mm","sa","sh","sx","sy","sm"]:
            for l in loci_lines:
                if l[:2] == beg:
                    result += "%s"%l

        # partie mutation model
        result += u"\ngroups (%i)\n"%len(self.groupList)
        if len(self.groupList) == 0:
            result += '\n'
        else:
            for i,box in enumerate(self.groupList):
                if "Microsatellites" in str(box.title()):
                    result += "group G%i [M]\n"%(i+1)
                    result += u'%s'%self.setMutation_dico[box].getMutationConf()
                elif "Sequences" in str(box.title()):
                    result += "group G%i [S]\n"%(i+1)
                    result += u'%s'%self.setMutationSeq_dico[box].getMutationConf()
        return result
