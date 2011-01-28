# -*- coding: utf-8 -*-

import os
import codecs
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from setGenData_ui import Ui_Frame
from setMutationModel import SetMutationModel
from setMutationModelSequences import SetMutationModelSequences
from setSummaryStatistics import SetSummaryStatistics
from setSummaryStatisticsSeq import SetSummaryStatisticsSeq
from visualizescenario import *
from data import *

class SetGeneticData(QFrame):
    """ Frame qui est ouverte dans un onglet pour faire des groupes de locus,
    déterminer leur modèle mutationnel et leur summary statistics
    """
    def __init__(self,parent=None):
        print LOC_TYP
        super(SetGeneticData,self).__init__(parent)
        self.parent=parent
        # liste des box
        self.groupList = []
        # dicos de frames indexé par les box
        self.setMutation_dico = {}
        self.setMutationSeq_dico = {}
        self.setSum_dico = {}
        self.setSumSeq_dico = {}
        # dico indexé par la box du groupe
        self.group_info_dico = {}
        self.nbLocusGui = 0
        # dico indexé par les noms de locus, qui donne le numero du locus et le groupe du locus
        self.dico_num_and_numgroup_locus = {}

        self.createWidgets()

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        self.ui.verticalLayout_3.setAlignment(QtCore.Qt.AlignTop)
        self.ui.tableWidget.setColumnWidth(0,230)
        self.ui.tableWidget.setColumnWidth(1,60)
        self.ui.tableWidget.setColumnWidth(2,60)
        self.ui.tableWidget.setColumnWidth(3,60)


        QObject.connect(self.ui.addGroupButton,SIGNAL("clicked()"),self.addGroup)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.clearButton,SIGNAL("clicked()"),self.clear)

    def fillLocusTableFromData(self):
        data = self.parent.data
        for i in range(data.nloc):
            if data.locuslist[i].type > 4:
                self.addRow(name=data.locuslist[i].name,type="S")
            else:
                self.addRow(name=data.locuslist[i].name,type="M")
            self.dico_num_and_numgroup_locus[data.locuslist[i].name] = [i+1,0]
            self.nbLocusGui+=1


    def addRow(self,name="locus",type="M"):
        """ ajoute un locus à la liste principale (de locus)
        """
        self.ui.tableWidget.insertRow(self.ui.tableWidget.rowCount())
        #self.ui.tableWidget.setCellWidget(self.ui.tableWidget.rowCount()-1,i,QPushButton("View"))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,0,QTableWidgetItem("%s"%(name)))
        self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,1,QTableWidgetItem("%s"%type))
        self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,0).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,0).flags() & ~Qt.ItemIsEditable)
        self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,1).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,1).flags() & ~Qt.ItemIsEditable)
        if type == "M":
            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,2,QTableWidgetItem("2"))
            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,3,QTableWidgetItem("40"))
        else:
            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,2,QTableWidgetItem(" "))
            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,3,QTableWidgetItem(" "))
            self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,2).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,2).flags() & ~Qt.ItemIsEditable)
            self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,3).setFlags(self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,3).flags() & ~Qt.ItemIsEditable)



    def addGroup(self):
        """ dessine un nouveau groupe vide
        """
        groupBox = QtGui.QGroupBox("Group %i"%(len(self.groupList)+1),self.ui.scrollAreaWidgetContents)
        groupBox.setMinimumSize(QtCore.QSize(0, 200))
        groupBox.setMaximumSize(QtCore.QSize(16777215, 200))
        groupBox.setObjectName("groupBox")
        horizontalLayout = QtGui.QHBoxLayout(groupBox)
        horizontalLayout.setObjectName("horizontalLayout")
        verticalLayout = QtGui.QVBoxLayout()
        verticalLayout.setObjectName("verticalLayout")
        addToButton = QtGui.QPushButton(">>",groupBox)
        addToButton.setObjectName("addToButton")
        verticalLayout.addWidget(addToButton)
        rmFromButton = QtGui.QPushButton("<<",groupBox)
        rmFromButton.setObjectName("rmFromButton")
        verticalLayout.addWidget(rmFromButton)
        horizontalLayout.addLayout(verticalLayout)
        verticalLayout_3 = QtGui.QVBoxLayout()
        verticalLayout_3.setObjectName("verticalLayout_3")
        rmButton = QtGui.QPushButton("Remove group",groupBox)
        rmButton.setObjectName("rmButton")
        verticalLayout_3.addWidget(rmButton)
        listWidget = QtGui.QListWidget(groupBox)
        listWidget.setObjectName("listWidget")
        verticalLayout_3.addWidget(listWidget)
        horizontalLayout.addLayout(verticalLayout_3)
        listWidget.setSelectionMode(QtGui.QAbstractItemView.MultiSelection)
        verticalLayout_2 = QtGui.QVBoxLayout()
        verticalLayout_2.setObjectName("verticalLayout_2")
        setMutationButton = QtGui.QPushButton("Set mutation\nModel",groupBox)
        setMutationButton.setObjectName("setMutationButton")
        verticalLayout_2.addWidget(setMutationButton)
        setSumButton = QtGui.QPushButton("Set Summary\nStatistics",groupBox)
        setSumButton.setObjectName("setSumButton")
        verticalLayout_2.addWidget(setSumButton)
        horizontalLayout.addLayout(verticalLayout_2)
        self.ui.verticalLayout_3.addWidget(groupBox)

        QObject.connect(addToButton,SIGNAL("clicked()"),self.addToGroup)
        QObject.connect(rmFromButton,SIGNAL("clicked()"),self.rmFromGroup)
        QObject.connect(rmButton,SIGNAL("clicked()"),self.rmGroup)
        QObject.connect(setMutationButton,SIGNAL("clicked()"),self.setMutation)
        QObject.connect(setSumButton,SIGNAL("clicked()"),self.setSum)

        self.groupList.append(groupBox)
        self.group_info_dico[groupBox] = [ "" , [] ]

        frame = SetMutationModel(self)
        self.setMutation_dico[groupBox] = frame
        frame.hide()
        frameSeq = SetMutationModelSequences(self)
        self.setMutationSeq_dico[groupBox] = frameSeq
        frameSeq.hide()
        frameSum = SetSummaryStatistics(self)
        self.setSum_dico[groupBox] = frameSum
        frameSum.hide()
        frameSumSeq = SetSummaryStatisticsSeq(self)
        self.setSumSeq_dico[groupBox] = frameSumSeq
        frameSumSeq.hide()

    def setMutation(self):
        box = self.sender().parent()
        title = str(box.title())
        if "Microsatellites" in title:
            self.parent.addTab(self.setMutation_dico[box],"Set mutation model")
            self.parent.setTabEnabled(self.parent.indexOf(self),False)
            self.parent.setCurrentWidget(self.setMutation_dico[box])
            # maj du titre de la frame
            lab = self.setMutation_dico[box].ui.setMutMsLabel
            lab.setText("Set mutation model of %s (microsatellites)"%(" ".join(str(box.title()).split()[:2])))
        elif "Sequences" in title:
            self.parent.addTab(self.setMutationSeq_dico[box],"Set mutation model")
            self.parent.setTabEnabled(self.parent.indexOf(self),False)
            self.parent.setCurrentWidget(self.setMutationSeq_dico[box])
            # maj du titre de la frame
            lab = self.setMutationSeq_dico[box].ui.setMutSeqLabel
            lab.setText("Set mutation model of %s (sequences)"%(" ".join(str(box.title()).split()[:2])))
        else:
            QMessageBox.information(self,"Set Mutation Model error","Add locus to a group before setting the mutation model")


    def setSum(self):
        box = self.sender().parent()
        title = str(box.title())
        if "Microsatellites" in title:
            self.parent.addTab(self.setSum_dico[box],"Set summary statistics")
            self.parent.setTabEnabled(self.parent.indexOf(self),False)
            self.parent.setCurrentWidget(self.setSum_dico[box])
            # maj du titre de la frame
            lab = self.setSum_dico[box].ui.sumStatLabel
            lab.setText("Set summary statistics of %s (microsatellites)"%(" ".join(str(box.title()).split()[:2])))
        elif "Sequences" in title:
            self.parent.addTab(self.setSumSeq_dico[box],"Set summary statistics")
            self.parent.setTabEnabled(self.parent.indexOf(self),False)
            self.parent.setCurrentWidget(self.setSumSeq_dico[box])
            # maj du titre de la frame
            lab = self.setSumSeq_dico[box].ui.sumStatLabel
            lab.setText("Set summary statistics of %s (sequences)"%(" ".join(str(box.title()).split()[:2])))
        else:
            QMessageBox.information(self,"Set Summary statistics error","Add locus to a group before setting the summary statistics")

    def rmGroup(self):
        """ Enlève les loci présents dans le groupe et supprime le groupe
        """
        box = self.sender().parent()
        lw = box.findChild(QListWidget,"listWidget")
        lw.selectAll()
        self.rmFromGroup(box)
        # suppression de la box
        box.hide()
        self.groupList.remove(box)
        del self.group_info_dico[box]
        # renommage des groupes
        for i,box in enumerate(self.groupList):
            old_title = str(box.title())
            box.setTitle("Group %i %s"%(i+1,' '.join(old_title.split(' ')[2:])))
            # pour chaque groupe, on maj le numero de groupe de tous les locus du groupe
            lw = box.findChild(QListWidget,"listWidget")
            for j in range(lw.count()):
                itemtxt = str(lw.item(j).text())
                self.dico_num_and_numgroup_locus[itemtxt][1] = i+1

    def addToGroup(self,box=None):
        """ ajoute les loci selectionnés au groupe du bouton pressé
        """
        all_similar = True
        first_type_found = ""
        if box == None:
            box = self.sender().parent()
        listwidget = box.findChild(QListWidget,"listWidget")
        row_list = []
        selection = self.ui.tableWidget.selectedIndexes()
        selection.sort()
        for it in selection:
            if it.row() not in row_list:
                row_list.append(it.row())
                # verif que tous les loci selectionnés ont le meme type
                if first_type_found == "":
                    first_type_found = str(self.ui.tableWidget.item(it.row(),1).text())
                else:
                    if first_type_found != str(self.ui.tableWidget.item(it.row(),1).text()):
                        all_similar = False
        # verif que les loci deja dans le groupe sont du même type que ceux sélectionnés
        # on compare le premier type trouvé dans la selection avec le type du premier
        # élément du groupe
        if listwidget.count() > 0:
            if all_similar:
                name = str(listwidget.item(0).text())
                #num = int(name.split(' ')[1])
                num = self.dico_num_and_numgroup_locus[name][0]
                # info sur le premier locus du groupe
                type = str(self.ui.tableWidget.item(num-1,1).text())
                print "\ntype deja present : %s"%type
                if type != first_type_found:
                    all_similar = False
        else:
            if all_similar:
                # le groupe est vide, on set le nom avec le type
                old_title = str(box.title())
                tt = ''
                if first_type_found == 'M':
                    tt = "Microsatellites"
                elif first_type_found == 'S':
                    tt = "Sequences"
                if tt != '':
                    box.setTitle("%s %s : %s"%(old_title.split(' ')[0],old_title.split(' ')[1],tt))
                    # on met à jour le dico des groupes
                    self.group_info_dico[box][0] = tt

        if all_similar:
            # déselection des présents
            for ind_to_unselect in range(listwidget.count()):
                listwidget.item(ind_to_unselect).setSelected(False)
            # ajout des nouveaux
            for row in row_list:
                name = self.ui.tableWidget.item(row,0).text()
                # ajout trié dans le groupe
                i = 0
                #num_to_insert = int(name.split(' ')[1])
                num_to_insert = row+1
                while i < listwidget.count() and self.dico_num_and_numgroup_locus[str(listwidget.item(i).text())][0] < num_to_insert :
                    i+=1
                #box.findChild(QListWidget,"listWidget").addItem(name)
                box.findChild(QListWidget,"listWidget").insertItem(i,name)
                box.findChild(QListWidget,"listWidget").item(i).setSelected(True)
                # on cache la ligne
                self.ui.tableWidget.setRowHidden(row,True)
                # on met à jour le dico des groupes
                self.group_info_dico[box][1].append(num_to_insert)
                # on met à jour le numero du groupe pour ce locus
                self.dico_num_and_numgroup_locus[str(name)][1] = (self.groupList.index(box)+1)
        else:
            QMessageBox.information(self,"Group error","In a group, all the loci must have the same type")

    def rmFromGroup(self,box=None):
        """ retire les loci selectionnés du groupe du bouton pressé
        """
        if box == None:
            box = self.sender().parent()
        listw_of_group = box.findChild(QListWidget,"listWidget")
        row_list = []
        selection = listw_of_group.selectedIndexes()
        for it in selection:
            if it.row() not in row_list:
                row_list.append(int(it.row()))
        row_list.sort()
        row_list.reverse()

        for row in row_list:
            name = str(listw_of_group.item(row).text())
            #num = int(name.split(' ')[1])
            num = self.dico_num_and_numgroup_locus[name][0]
            # retrait du groupe
            #item = listw_of_group.takeItem(row)
            listw_of_group.takeItem(row)
            # on decouvre la ligne
            self.ui.tableWidget.setRowHidden(num-1,False)
            # on met à jour le dico des groupes
            self.group_info_dico[box][1].remove(num)
            self.dico_num_and_numgroup_locus[name][1] = 0

        # si le groupe devient vide, on enleve le type dans le nom
        if listw_of_group.count() == 0:
            old_title = str(box.title())
            box.setTitle("%s %s"%(old_title.split(' ')[0],old_title.split(' ')[1]))
            # on met à jour le dico des groupes
            self.group_info_dico[box][0] = ""


    def exit(self):
        # reactivation des onglets
        self.parent.setTabEnabled(0,True)
        self.parent.setTabEnabled(1,True)
        self.parent.removeTab(self.parent.indexOf(self))
        self.parent.setCurrentIndex(0)

    def validate(self):
        print self.group_info_dico
        print self.dico_num_and_numgroup_locus
        self.writeGeneticConfFromGui()

    def clear(self):
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
        if os.path.exists(self.parent.ui.dirEdit.text()+"/conf.gen.tmp"):
            os.remove("%s/conf.gen.tmp" % self.parent.ui.dirEdit.text())

        f = codecs.open(self.parent.ui.dirEdit.text()+"/conf.gen.tmp",'w',"utf-8")
        # partie description des locus
        f.write("loci description (nloc=%i)\n"%self.nbLocusGui)
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
        for i,box in enumerate(self.groupList):
            if "Microsatellites" in str(box.title()):
                f.write("group G%i [M]\n"%(i+1))
                f.write(u'%s'%self.setMutation_dico[box].getMutationConf())
                print self.setSum_dico[box].getSumConf()
            elif "Sequences" in str(box.title()):
                f.write("group G%i [S]\n"%(i+1))
                to_write = u'%s'%self.setMutationSeq_dico[box].getMutationConf()
                f.write(to_write)
                print self.setSumSeq_dico[box].getSumConf()

        # partie sum stats
        stat_txt_list = []
        nstat_tot = 0
        for i,box in enumerate(self.groupList):
            stat_txt = ""
            if "Microsatellites" in str(box.title()):
                (nstat_tmp,stat_txt) = self.setSum_dico[box].getSumConf()
                stat_header_txt = "group G%i [M] (%i)\n"%((i+1),nstat_tmp)
                nstat_tot += nstat_tmp
                # construction de la chaine a ecrire pour ce groupe TODO
                stat_to_write = stat_header_txt+stat_txt
            elif "Sequences" in str(box.title()):
                stat_txt = "group G%i [S] (%i)"%((i+1),self.setSumSeq_dico[box].getNstat())
                nstat_tot += self.setSumSeq_dico[box].getNstat()
                stat_txt += self.setSumSeq_dico[box].getSumConf()
            stat_txt_list.append(stat_to_write)

        f.write("\ngroup summary statistics (%i)\n"%nstat_tot)
        for txt in stat_txt_list:
            f.write(txt)
            print txt


    def loadGeneticConf(self):
        """ Charge le fichier conf.gen.tmp
        """
        # de toute façon, on rempli le tableau de locus
        self.fillLocusTableFromData()
        if os.path.exists(self.parent.dir):
            if os.path.exists("%s/conf.gen.tmp"%(self.parent.dir)):
                f = codecs.open("%s/conf.gen.tmp"%(self.parent.dir),"r","utf-8")
                lines = f.readlines()
                nloc = int(lines[0].split("nloc=")[1].split(')')[0])
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
                    print 'ligne:',lines[l]
                    nb_groupes = int(lines[l].split('(')[1].split(')')[0])
                    num_group_max_done = 0
                    l+=1
                    # on va jusqu'à la prochaine ligne vide ou la fin du fichier
                    while l<len(lines) and lines[l].strip() != "":
                        print 'prem ligne du groupe:',lines[l]
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

