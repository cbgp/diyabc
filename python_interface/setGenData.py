# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.setGenData_ui import Ui_Frame
#from mutationModel.setMutationModelMsatRefTable import SetMutationModelMsatRefTable
#from mutationModel.setMutationModelSequences import SetMutationModelSequences
#from setSummaryStatistics import SetSummaryStatistics
#from setSummaryStatisticsSeq import SetSummaryStatisticsSeq
from utils.visualizescenario import *
from utils.data import *
import output
from output import log

formGenData,baseGenData = uic.loadUiType("uis/setGenData.ui")

class SetGeneticData(formGenData,baseGenData):
    """ Frame qui est créée pour faire des groupes de locus,
    déterminer leur modèle mutationnel et leur summary statistics
    """
    def __init__(self,parent=None):
        super(SetGeneticData,self).__init__(parent)
        self.parent=parent
        # liste des box
        self.groupList = []
        # dicos de frames indexé par les box
        self.setMutation_dico = {}
        self.setMutationSeq_dico = {}
        self.setSum_dico = {}
        self.setSumSeq_dico = {}
        # dicos de booleens indexé par les box
        self.setMutationValid_dico = {}
        self.setMutationSeqValid_dico = {}
        # dico indexé par la box du groupe
        self.group_info_dico = {}
        self.nbLocusGui = 0
        # dico indexé par les noms de locus, qui donne le numero du locus et le groupe du locus
        self.dico_num_and_numgroup_locus = {}

        self.createWidgets()

    def createWidgets(self):
        #self.ui = Ui_Frame()
        self.ui = self
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
        """ rempli le tableau des locus en consultant l'instance de Data
        On ne peut pas le faire à l'instanciation de l'objet SetGeneticData car on a pas 
        encore lu le fichier de conf dans le cas d'un load project
        """
        data = self.parent.data
        nb_m = 0
        nb_s = 0
        for i in range(data.nloc):
            if data.locuslist[i].type > 4:
                self.addRow(name=data.locuslist[i].name,type="S")
                nb_s += 1
            else:
                self.addRow(name=data.locuslist[i].name,type="M")
                nb_m += 1
            self.dico_num_and_numgroup_locus[data.locuslist[i].name] = [i+1,0]
            self.nbLocusGui+=1
        # affichage du bon nombre de locus dans l'onglet principal du projet
        self.majProjectGui(m=nb_m,s=nb_s)

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
            #self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,2,QTableWidgetItem("2"))
            #self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,3,QTableWidgetItem("40"))
            #self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,2).setTextAlignment(Qt.AlignRight)
            #self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,3).setTextAlignment(Qt.AlignRight)

            dos = QTableWidgetItem("2")
            tres = QTableWidgetItem("40")
            dos.setTextAlignment(Qt.AlignRight|Qt.AlignTop)
            tres.setTextAlignment(Qt.AlignRight|Qt.AlignTop)
            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,2,dos)
            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount()-1,3,tres)
            log(3,"Adding a locus (%s)"%tres.textAlignment())
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

        self.majProjectGui(g=len(self.groupList))

        return groupBox

    def switchTo(self,widget):
        """ methode virtuelle a redefinir
        """
        pass

    def setMutation(self,box=None):
        """ déclenché par le clic sur le bouton 'set mutation model' ou par le clic sur 'clear'
        dans 'set mutation model'. bascule vers la saisie du modèle mutationnel
        """
        if box == None:
            box = self.sender().parent()
        title = str(box.title())
        if "Microsatellites" in title:
            #self.parent.addTab(self.setMutation_dico[box],"Set mutation model")
            #self.parent.setTabEnabled(self.parent.indexOf(self),False)
            #self.parent.setCurrentWidget(self.setMutation_dico[box])
            self.switchTo(self.setMutation_dico[box])
            # maj du titre de la frame
            lab = self.setMutation_dico[box].ui.setMutMsLabel
            lab.setText("Set mutation model of %s (microsatellites)"%(" ".join(str(box.title()).split()[:2])))
            # on considère que le setmutation n'est plus valide, il faut le revalider
            self.setMutationValid_dico[box] = False
        elif "Sequences" in title:
            #self.parent.addTab(self.setMutationSeq_dico[box],"Set mutation model")
            #self.parent.setTabEnabled(self.parent.indexOf(self),False)
            #self.parent.setCurrentWidget(self.setMutationSeq_dico[box])
            self.switchTo(self.setMutationSeq_dico[box])
            # maj du titre de la frame
            lab = self.setMutationSeq_dico[box].ui.setMutSeqLabel
            lab.setText("Set mutation model of %s (sequences)"%(" ".join(str(box.title()).split()[:2])))
            # on considère que le setmutation n'est plus valide, il faut le revalider
            self.setMutationSeqValid_dico[box] = False
        else:
            output.notify(self,"Set Mutation Model error","Add locus to a group before setting the mutation model")

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

        self.majProjectGui(g=len(self.groupList))

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
                #print "\ntype deja present : %s"%type
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
            output.notify(self,"Group error","In a group, all the loci must have the same type")

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
            # on remet en gras les boutons de mutation model et sum stats
            set_mut_button = box.findChild(QPushButton,"setMutationButton")
            set_sum_button = box.findChild(QPushButton,"setSumButton")
            set_mut_button.setStyleSheet("background-color: #EFB1B3")
            set_sum_button.setStyleSheet("background-color: #EFB1B3")
            fontt = set_mut_button.font()
            fontt.setBold(True)
            set_mut_button.setFont(fontt)
            set_sum_button.setFont(fontt)

    def getNbSumStats(self):
        """ retourne le nombre total de summary statistics (nb de cases cochées)
        """ 
        nb_sum_stats = 0
        for box in self.groupList:
            if "Microsatellites" in str(box.title()):
                (nstat,stat_txt) = self.setSum_dico[box].getSumConf()
                nb_sum_stats += nstat
            elif "Sequences" in str(box.title()):
                (nstat,stat_txt) = self.setSumSeq_dico[box].getSumConf()
                nb_sum_stats += nstat
        return nb_sum_stats

    def getNbParam(self):
        """ retourne le nombre de paramètres mutationnels réels (qui varient. min<max pour le MEAN)
        """
        nb_param = 0
        for box in self.groupList:
            if "Microsatellites" in str(box.title()):
                nb_param += self.setMutation_dico[box].getNbParam()
            elif "Sequences" in str(box.title()):
                nb_param += self.setMutationSeq_dico[box].getNbParam()
        return nb_param

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
        """ retourne la partie sumstats du table header
        """
        result = u""
        for box in self.groupList:
            if "Microsatellites" in str(box.title()):
                sums_txt = self.setSum_dico[box].getSumStatsTableHeader()
            elif "Sequences" in str(box.title()):
                sums_txt = self.setSumSeq_dico[box].getSumStatsTableHeader()
            result += sums_txt
        return result

    def clearSummaryStats(self,box):
        """ vide les summary statistics des microsats
        """
        self.setSum_dico[box].exit()
        self.setSum_dico[box] = SetSummaryStatisticsMsat(self,box)
        self.setSum(box)

    def clearSummaryStatsSeq(self,box):
        """ vide les summary statistics des sequences
        """
        self.setSumSeq_dico[box].exit()
        self.setSumSeq_dico[box] = SetSummaryStatisticsSeq(self,box)
        self.setSum(box)

