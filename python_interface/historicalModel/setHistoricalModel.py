# -*- coding: utf-8 -*-

import os,sys
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.setHistFrame_ui import Ui_Frame
from drawScenario import DrawScenario
from utils.visualizescenario import *
from utils.history import *
from setCondition import SetCondition
import output
from utils.cbgpUtils import log

formHistModel,baseHistModel = uic.loadUiType("uis/setHistFrame.ui")

class SetHistoricalModel(formHistModel,baseHistModel):
    """ Classe pour la définition du modèle historique dans le cadre 
    de la génération de la table de référence
    """
    def __init__(self,parent=None):
        super(SetHistoricalModel,self).__init__(parent)
        self.parent=parent
        # [GUI] liste des scenarios, pourcentages et conditions non effacés
        self.scList = []
        self.rpList = []
        self.condList = []
        self.paramList = []

        self.scenarios_modified_since_define_priors = True

        # liste des infos sur les scenarios valides (donc déjà vérifiés)
        # voir format dans writeHistoricalConf
        self.scenarios_info_list = []
        self.param_info_dico = {}

        self.createWidgets()

        self.pixList = []
        self.previewLabelList = []

    def createWidgets(self):
        #self.ui = Ui_Frame()
        #self.ui.setupUi(self)
        self.setupUi(self)
        self.ui = self

        QObject.connect(self.ui.addScButton,SIGNAL("clicked()"),self.addSc)
        QObject.connect(self.ui.uniformRadio,SIGNAL("clicked()"),self.setUniformRp)
        QObject.connect(self.ui.otherRadio,SIGNAL("clicked()"),self.setOtherRp)
        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.validate)
        QObject.connect(self.ui.exitButton,SIGNAL("clicked()"),self.close)
        QObject.connect(self.ui.clearButton,SIGNAL("clicked()"),self.close)
        QObject.connect(self.ui.chkScButton,SIGNAL("clicked()"),self.checkToDraw)
        QObject.connect(self.ui.defPrButton,SIGNAL("clicked()"),self.definePriors)

        QObject.connect(self.ui.drawPreviewsCheck,SIGNAL("stateChanged(int)"),self.displayPreviews)
        QObject.connect(self.ui.refreshPreviewsButton,SIGNAL("clicked()"),self.displayPreviews)

        self.ui.refreshPreviewsButton.hide()

        self.ui.verticalLayout_6.setAlignment(QtCore.Qt.AlignTop)
        self.ui.horizontalLayout_6.setAlignment(QtCore.Qt.AlignLeft)
        self.ui.horizontalLayout_2.setAlignment(QtCore.Qt.AlignLeft)
        self.ui.horizontalLayout_3.setAlignment(QtCore.Qt.AlignLeft)
        if "darwin" in sys.platform:
        	for lab in [self.ui.label_2,self.ui.label_3,self.ui.label_4,self.ui.label_5,self.ui.label_6,self.ui.label_7,self.ui.label_8,self.ui.label_9,self.ui.label_10]:
			ff = lab.font()
			ff.setPointSize(10)
			lab.setFont(ff)

    def displayPreviews(self,state=None):
        """ Met à jour ou fait apparaitre ou disparaitre les previews
        Si state n'est pas spécifié, regarde si "draw previews" est coché
        """
        if state == None:
            if self.ui.drawPreviewsCheck.isChecked():
                state = 2
            else:
                state = 0

        if state == 0:
            # décoché
            for l in self.previewLabelList:
                l.hide()
                self.ui.horizontalLayout_3.removeWidget(l)
            self.previewLabelList = []
            self.pixList = []
            self.ui.refreshPreviewsButton.hide()
        else:
            self.drawPreviews()
            self.ui.refreshPreviewsButton.show()

    def addSc(self):
        """ ajout graphique d'un scenario vide
        """
        # le numero du nouveau scenario est la taille du tableau actuel de scenarios
        num_scenario = len(self.scList)+1
        
        # creation de la groupbox a ajouter
        groupBox = QtGui.QGroupBox(self.ui.scScrollContent)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox.sizePolicy().hasHeightForWidth())
        groupBox.setSizePolicy(sizePolicy)
        groupBox.setObjectName("groupBox_"+str(num_scenario))
        verticalLayout_6 = QtGui.QVBoxLayout(groupBox)
        verticalLayout_6.setObjectName("verticalLayout_6")
        horizontalLayout_6 = QtGui.QHBoxLayout()
        horizontalLayout_6.setObjectName("horizontalLayout_6")
        label_3 = QtGui.QLabel("scenario %i"%num_scenario,groupBox)
        # ne mettre qu'un seul label du nom scLabel pour pouvoir le retrouver avec findChild()
        label_3.setObjectName("scLabel")
        horizontalLayout_6.addWidget(label_3)
        pushButton_6 = QtGui.QPushButton("remove",groupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(pushButton_6.sizePolicy().hasHeightForWidth())
        pushButton_6.setSizePolicy(sizePolicy)
        pushButton_6.setMaximumSize(QtCore.QSize(80, 16777215))
        pushButton_6.setObjectName("rmScButton")
        horizontalLayout_6.addWidget(pushButton_6)
        verticalLayout_6.addLayout(horizontalLayout_6)
        plainTextEdit = QtGui.QPlainTextEdit(groupBox)
        plainTextEdit.setLineWrapMode(QtGui.QPlainTextEdit.NoWrap)
        plainTextEdit.setObjectName("scplainTextEdit")
        #if "darwin" not in sys.platform:
        #    ff = plainTextEdit.font()
        #    ff.setPointSize(8)
        #    plainTextEdit.setFont(ff)
        #else:
        ff = plainTextEdit.font()
        ff.setPointSize(9)
        plainTextEdit.setFont(ff)
        verticalLayout_6.addWidget(plainTextEdit)
        # ajout de la groupbox
        self.ui.horizontalLayout_3.addWidget(groupBox)

        # evennement de suppression du scenario
        QObject.connect(pushButton_6,SIGNAL("clicked()"),self.rmSc)
        # modif du texte modifie attribut scenarios_modified_since_define_priors
        QObject.connect(plainTextEdit,SIGNAL("textChanged()"),self.modifOnScenarios)
        #QObject.connect(plainTextEdit,SIGNAL("textChanged()"),self.drawPreviews)


        # ajout du scenario dans la liste locale (plus facile à manipuler)
        self.scList.append(groupBox)

        # ajout de la groupbox de repartition
        groupBox_r = QtGui.QGroupBox(self.ui.repScrollContent)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_r.sizePolicy().hasHeightForWidth())
        groupBox_r.setSizePolicy(sizePolicy)
        groupBox_r.setMinimumSize(QtCore.QSize(16777215, 48))
        groupBox_r.setMaximumSize(QtCore.QSize(16777215, 48))
        groupBox_r.setObjectName("groupBox_r")
        verticalLayout_6 = QtGui.QVBoxLayout(groupBox_r)
        verticalLayout_6.setSpacing(1)
        verticalLayout_6.setContentsMargins(-1, 1, -1, 1)
        verticalLayout_6.setObjectName("verticalLayout_6")
        label_3 = QtGui.QLabel("scenario "+str(num_scenario),groupBox_r)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(label_3.sizePolicy().hasHeightForWidth())
        label_3.setSizePolicy(sizePolicy)
        label_3.setObjectName("rpLabel")
        verticalLayout_6.addWidget(label_3)
        lineEdit = QtGui.QLineEdit(groupBox_r)
        lineEdit.setAlignment(Qt.AlignRight)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(lineEdit.sizePolicy().hasHeightForWidth())
        lineEdit.setSizePolicy(sizePolicy)
        lineEdit.setMinimumSize(QtCore.QSize(0, 26))
        lineEdit.setMaximumSize(QtCore.QSize(60, 26))
        lineEdit.setObjectName("rpEdit")
        verticalLayout_6.addWidget(lineEdit)
        self.ui.horizontalLayout_6.addWidget(groupBox_r)

        font = QFont()
        if "darwin" in sys.platform:
        	si = 9
        else:
        	si = 7
        font.setPointSize(si)
        groupBox_r.setFont(font)

        # ajout des rp dans la liste locale (plus facile à manipuler)
        self.rpList.append(groupBox_r)

        if self.ui.uniformRadio.isChecked():
            self.setUniformRp()

        # maj de la doc seulement sur les box de scenario
        self.updateDoc(groupBox)
        self.updateDoc(groupBox_r)
        # on a modifié les scenarios
        self.modifOnScenarios()

    def modifOnScenarios(self):
        """ déclenché par une modification dans le contenu d'un des scenarios
        """
        self.scenarios_modified_since_define_priors = True
        fontt = self.ui.defPrButton.font()
        fontt.setBold(True)
        self.ui.defPrButton.setStyleSheet("background-color: #EFB1B3")
        #fontt.setPointSize(fontt.pointSize() + 1)
        #self.ui.defPrButton.setText("Define\npriors")
        self.ui.defPrButton.setFont(fontt)
    
    def rmSc(self):
        """ Suppression d'un scenario dans l'affichage et dans la liste locale
        """
        self.sender().parent().hide()
        num_scenar = self.scList.index(self.sender().parent())
        self.scList.remove(self.sender().parent())
        ## mise a jour des index dans le label
        for i in range(len(self.scList)):
            self.scList[i].findChild(QLabel,"scLabel").setText("scenario %i"% (i+1))
            # manière moins sure mais tout de même intéressante dans le principe
            #self.scList[i].layout().itemAt(0).layout().itemAt(0).widget().setText("scenario %i"% i)

        # suppression et disparition de l'indice de répartition pour le scenario à supprimer
        self.rpList.pop(num_scenar).hide()
        for i in range(len(self.rpList)):
            self.rpList[i].findChild(QLabel,"rpLabel").setText("scenario %i"% (i+1))

        if self.ui.uniformRadio.isChecked():
            self.setUniformRp()

        # on a modifié les scenarios
        self.modifOnScenarios()

    def setUniformRp(self):
        """ met les pourcentages à une valeur identique 
        """
        if len(self.scList) != 0:
            val = 1.0/float(len(self.scList))
            # pour chaque pourcentage
            for rp in self.rpList:
                lineEdit = rp.findChild(QLineEdit,"rpEdit")
                lineEdit.setText(str(round(val,5)))
                lineEdit.setDisabled(True)

            

    def setOtherRp(self):
        """ redonne la possibilité de modifier à sa guise les pourcentages
        """
        for rp in self.rpList:
            lineEdit = rp.findChild(QLineEdit,"rpEdit")
            lineEdit.setDisabled(False)

    def addCondition(self,cond_str=""):
        """ ajout d'une condition dans la zone du bas réservée à cet effet
        """
        groupBox_cond = QtGui.QGroupBox(self.ui.scrollAreaWidgetContents_3)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_cond.sizePolicy().hasHeightForWidth())
        groupBox_cond.setSizePolicy(sizePolicy)
        groupBox_cond.setTitle("")
        groupBox_cond.setObjectName("groupBox_6")
        groupBox_cond.setMaximumSize(QtCore.QSize(100, 44))
        verticalLayout_2 = QtGui.QVBoxLayout(groupBox_cond)
        verticalLayout_2.setObjectName("verticalLayout_2")
        verticalLayout_2.setContentsMargins(-1, 1, -1, 1)
        label_2 = QtGui.QLabel(cond_str,groupBox_cond)
        label_2.setObjectName("condLabel")
        label_2.setAlignment(QtCore.Qt.AlignCenter)
        label_2.setMinimumSize(QtCore.QSize(52, 11))
        label_2.setMaximumSize(QtCore.QSize(52, 11))
        verticalLayout_2.addWidget(label_2)
        pushButton_5 = QtGui.QPushButton("remove",groupBox_cond)
        #sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Fixed)
        #sizePolicy.setHorizontalStretch(0)
        #sizePolicy.setVerticalStretch(0)
        #sizePolicy.setHeightForWidth(pushButton_5.sizePolicy().hasHeightForWidth())
        #pushButton_5.setSizePolicy(sizePolicy)
        pushButton_5.setMinimumSize(QtCore.QSize(52, 17))
        pushButton_5.setMaximumSize(QtCore.QSize(52, 17))
        pushButton_5.setObjectName("rmCondButton")
        verticalLayout_2.addWidget(pushButton_5)
        # ajout dans la GUI
        self.ui.horizontalLayout_2.addWidget(groupBox_cond)

        # evennement de suppression de la condition
        QObject.connect(pushButton_5,SIGNAL("clicked()"),self.rmCond)

        self.condList.append(groupBox_cond)

        # maj de la doc pour la condition
        self.updateDoc(groupBox_cond)

    def rmCond(self,condBox=None):
        """ Suppression d'une condition sur les variables
        dans l'affichage et dans la liste locale
        """
        if condBox == None:
            condBox = self.sender().parent()
        condBox.hide()
        self.condList.remove(condBox)

    def checkToDraw(self):
        """ clic sur le bouton check scenario pour dessiner les scenarios
        """
        self.majParamInfoDico()
        chk_list = self.checkScenariosGraphic()
        if chk_list != None:
            self.drawScenarios(chk_list)
        else:
            output.notify(self,"Scenario error","Correct your scenarios to be able to draw them.")

    def drawPreviews(self):
        """ Dessine les aperçus à côté des textes des scenarios
        """
        if self.ui.drawPreviewsCheck.isChecked():
            # nettoyage
            for l in self.previewLabelList:
                self.ui.horizontalLayout_3.removeWidget(l)
                l.hide()
            self.previewLabelList = []
            self.pixList = []

            # dessin
            for num,sc in enumerate(self.scList):
                sctxt = str(sc.findChild(QPlainTextEdit,"scplainTextEdit").toPlainText())
                sc_info = self.checkOneScenario(num,sctxt,True)
                if sc_info != None and sc_info["tree"] != None and sc_info["checker"].nsamp <= 5:
                    segments = sc_info["tree"].segments
                    scc = sc_info["checker"]
                    t = sc_info["tree"]

                    xmax = 500
                    ymax = 450
                    pix = QPixmap(xmax,ymax)
                    self.pixList.append(pix)
                    pix.fill(Qt.white)

                    painter = QPainter(pix)
                    pen = QPen(Qt.black,2)
                    painter.setPen(pen)

                    dr = DrawScenario()
                    dr.paintScenario(painter,segments,scc,t,xmax,ymax,12)

                    label = QLabel()
                    pix2 = pix.scaled(200,170)
                    label.setPixmap(pix2)
                    self.previewLabelList.append(label)
                    self.ui.horizontalLayout_3.insertWidget((scc.number*2)-1,label)
                else:
                    lab = QLabel("No preview \navailable. You\n may have\n it by\n'Check scenario'")
                    font = QFont()
                    font.setPixelSize(9)
                    lab.setFont(font)
                    self.previewLabelList.append(lab)
                    self.ui.horizontalLayout_3.insertWidget(((num+1)*2)-1,lab)

    def definePriors(self,silent=False):
        """ clic sur le bouton de définition des priors
        """
        self.majParamInfoDico()
        chk_list = self.checkScenarios(silent=silent)
        if chk_list != None:
            self.putParameters(chk_list)
            self.scenarios_modified_since_define_priors = False
            fontt = self.ui.defPrButton.font()
            fontt.setBold(False)
            #fontt.setPointSize(fontt.pointSize() - 1)
            self.ui.defPrButton.setFont(fontt)
            self.ui.defPrButton.setStyleSheet("border-color: #000000")
            #self.ui.defPrButton.setText("Define priors")
            # dessin des aperçus
            self.drawPreviews()
        else:
            if not silent:
                output.notify(self,"Scenario error","Correct your scenarios to be able to extract the parameters.")

    def putParameters(self,chk_list):
        """ A partir de la liste des scenarios (vérifiés donc valides), on ajoute les paramètres dans la GUI
        La liste de paramètres est vidée avant cette opération
        La liste des conditions est nettoyée des conditions concernant un paramètre qui n'existe plus
        """
        # on enleve les groupbox de la GUI
        for paramBox in self.paramList:
            paramBox.hide()
            self.ui.verticalLayout_6.removeWidget(paramBox)
        # localement, on garde le dico d'infos pour restaurer les valeurs si un param existe encore
        dico_tmp = self.param_info_dico
        # on vide la liste locale de paramètres
        self.paramList = []
        self.param_info_dico = {}
        dico_count_per_category = {}
        params_order_list = []
        # liste des premiers params de chaque categorie
        lprem = []
        for sc in chk_list:
            #print "param list",sc["checker"].parameters
            for param in sc["checker"].parameters:
                params_order_list.append(param.name)
                # si le paramètre était deja la, il reprend ses valeurs, 
                # sinon les valeurs seront consultées plus tard à partir de la GUI
                if param.name in dico_tmp.keys() and len(dico_tmp[param.name])>6:
                    #self.param_info_dico[param.name] = [param.category,dico_tmp[param.name][1],dico_tmp[param.name][2],dico_tmp[param.name][3],dico_tmp[param.name][4],dico_tmp[param.name][5],dico_tmp[param.name][6]]
                    self.param_info_dico[param.name] = [param.category,dico_tmp[param.name][1],dico_tmp[param.name][2],dico_tmp[param.name][3],dico_tmp[param.name][4],dico_tmp[param.name][5]]
                else:
                    self.param_info_dico[param.name] = [param.category]
                # on compte le nombre de param dans chaque categorie
                if dico_count_per_category.has_key(param.category):
                    dico_count_per_category[param.category] += 1
                else:
                    dico_count_per_category[param.category] = 1
                    # si c'est le premier, on le met ds la liste des premiers pour ne pas afficher "set condition"
                    lprem.append(param.name)
        l_already_printed = []
        #for pname in param_info_dico.keys():
        for pname in params_order_list:
            # on n'affiche que ceux qui ne l'ont pas déjà été
            if pname not in l_already_printed:
                if dico_count_per_category[self.param_info_dico[pname][0]] > 1 and (pname not in lprem):
                    box = self.addParamGui(pname,"multiple",self.param_info_dico[pname][0])
                else:
                    box = self.addParamGui(pname,"unique",self.param_info_dico[pname][0])
                l_already_printed.append(pname)
                # conservation des valeurs précédentes
                if pname in dico_tmp.keys():
                    if box.findChild(QLineEdit,"minValueParamEdit") != None:
                        box.findChild(QLineEdit,"minValueParamEdit").setText(dico_tmp[pname][2])
                    if box.findChild(QLineEdit,"maxValueParamEdit") != None:
                        box.findChild(QLineEdit,"maxValueParamEdit").setText(dico_tmp[pname][3])
                    if box.findChild(QLineEdit,"meanValueParamEdit") != None:
                        box.findChild(QLineEdit,"meanValueParamEdit").setText(dico_tmp[pname][4])
                    if box.findChild(QLineEdit,"stValueParamEdit") != None:
                        box.findChild(QLineEdit,"stValueParamEdit").setText(dico_tmp[pname][5])
                    #box.findChild(QLineEdit,"stepValueParamEdit").setText(dico_tmp[pname][6])
                    if box.findChild(QRadioButton,"uniformParamRadio") != None and dico_tmp[pname][1] == "UN":
                        box.findChild(QRadioButton,"uniformParamRadio").setChecked(True)
                    elif box.findChild(QRadioButton,"logUniformRadio") != None and dico_tmp[pname][1] == "LU":
                        box.findChild(QRadioButton,"logUniformRadio").setChecked(True)
                    elif box.findChild(QRadioButton,"normalRadio") != None and dico_tmp[pname][1] == "NO":
                        box.findChild(QRadioButton,"normalRadio").setChecked(True)
                    elif box.findChild(QRadioButton,"logNormalRadio") != None and dico_tmp[pname][1] == "LN":
                        box.findChild(QRadioButton,"logNormalRadio").setChecked(True)
        # nettoyage des conditions obsolètes
        cond_list_to_del = []
        for cond in self.condList:
            ctxt = str(cond.findChild(QLabel,"condLabel").text())
            if '<=' in ctxt:
                cl = ctxt.split('<=')
            elif '>=' in ctxt:
                cl = ctxt.split('>=')
            elif '>' in ctxt:
                cl = ctxt.split('>')
            elif '<' in ctxt:
                cl = ctxt.split('<')
            name1 = cl[0].strip()
            name2 = cl[1].strip()
            if name1 not in self.param_info_dico.keys() or\
                name2 not in self.param_info_dico.keys():
                cond_list_to_del.append(cond)
        for cond in cond_list_to_del:
            self.rmCond(cond)

        # maj de la doc pour les paramètres historiques
        self.updateDoc(self.ui.scrollArea)

    def updateDoc(self,obj=None):
        self.parent.parent.updateDoc(obj)

    def checkOneScenario(self,num,sc,silent=False):
        """ verifie un seul scenario et construit son tree pour dessiner les previews
        """
        dico_sc_infos = None
        scChecker = Scenario(number=num+1,prior_proba=str(self.rpList[num].findChild(QLineEdit,"rpEdit").text()))
        try:
            scChecker.checkread(sc.strip().split('\n'),self.parent.data)
            scChecker.checklogic()
            dico_sc_infos = {}
            dico_sc_infos["text"] = sc.strip().split('\n')
            dico_sc_infos["checker"] = scChecker
            t = PopTree(scChecker)
            t.do_tree()
            dico_sc_infos["tree"] = t
        except IOScreenError, e:
            if not silent:
                output.notify(self,"Scenario error","%s"%e)
        except PopTreeError,e:
            dico_sc_infos["tree"] = None
        return dico_sc_infos

    def checkScenariosGraphic(self,silent=False):
        """ action de verification des scenarios (dessin inclu)
        retourne la liste des infos sur les scenarios si ceux ci sont tous bons
        sinon retourne None
        """
        # construction de la liste de scenarios
        sc_txt_list = []
        for sc in self.scList:
            sc_txt_list.append(str(sc.findChild(QPlainTextEdit,"scplainTextEdit").toPlainText()))
            #print sc_txt_list
        nb_scenarios_invalides = 0
        self.scenarios_info_list = []
        for num,sc in enumerate(sc_txt_list):
            scChecker = Scenario(number=num+1,prior_proba=str(self.rpList[num].findChild(QLineEdit,"rpEdit").text()))
            try:
                #print self.parent.data
                scChecker.checkread(sc.strip().split('\n'),self.parent.data)
                scChecker.checklogic()
                #for ev in scChecker.history.events : print ev
                #for  no in t.node : print no
                #print "  "
                #for  b in t.br : print b
                #print "  "
                #for s in t.segments: 
                #    print s
                #    print type(s)
                dico_sc_infos = {}
                dico_sc_infos["text"] = sc.strip().split('\n')
                dico_sc_infos["checker"] = scChecker
                #print "nb param du sc ",num," ",scChecker.nparamtot
                t = PopTree(scChecker)
                t.do_tree()
                dico_sc_infos["tree"] = t
                self.scenarios_info_list.append(dico_sc_infos)
            except IOScreenError, e:
                #print "Un scenario a une erreur : ", e
                nb_scenarios_invalides += 1
                if not silent:
                    output.notify(self,"Scenario error","%s"%e)
            except PopTreeError,e:
                dico_sc_infos["tree"] = None
                self.scenarios_info_list.append(dico_sc_infos)
        # si tous les scenarios sont bons, on renvoie les données utiles, sinon on renvoie None
        if nb_scenarios_invalides == 0:
            return self.scenarios_info_list
        else:
            return None

    def checkScenarios(self,silent=False):
        """ action de verification des scenarios (sans dessin)
        retourne la liste des infos sur les scenarios si ceux ci sont tous bons
        sinon retourne None
        """
        # construction de la liste de scenarios
        sc_txt_list = []
        for sc in self.scList:
            sc_txt_list.append(str(sc.findChild(QPlainTextEdit,"scplainTextEdit").toPlainText()))
            #print sc_txt_list
        nb_scenarios_invalides = 0
        self.scenarios_info_list = []
        for num,sc in enumerate(sc_txt_list):
            log(3,"Checking scenario. Data : %s"%self.parent.data)
            scChecker = Scenario(number=num+1,prior_proba=str(self.rpList[num].findChild(QLineEdit,"rpEdit").text()))
            try:
                #print self.parent.data
                scChecker.checkread(sc.strip().split('\n'),self.parent.data)
                scChecker.checklogic()
                dico_sc_infos = {}
                dico_sc_infos["text"] = sc.strip().split('\n')
                dico_sc_infos["checker"] = scChecker
                #print "nb param du sc ",num," ",scChecker.nparamtot
                dico_sc_infos["tree"] = None
                self.scenarios_info_list.append(dico_sc_infos)
            except IOScreenError, e:
                #print "Un scenario a une erreur : ", e
                nb_scenarios_invalides += 1
                if not silent:
                    output.notify(self,"Scenario error","%s"%e)
        # si tous les scenarios sont bons, on renvoie les données utiles, sinon on renvoie None
        if nb_scenarios_invalides == 0:
            return self.scenarios_info_list
        else:
            return None

    def drawScenarios(self,scenarios_info_list):
        """ lance la fenetre ou se trouveront les graphes des scenarios
        """
        self.draw_sc_win = DrawScenario(scenarios_info_list,self)
        self.parent.ui.refTableStack.addWidget(self.draw_sc_win)
        self.parent.ui.refTableStack.setCurrentWidget(self.draw_sc_win)

        #self.parent.setTabEnabled(0,False)
        #self.parent.setTabEnabled(1,False)
        #self.parent.setTabEnabled(2,False)
        #self.parent.setCurrentWidget(self.draw_sc_win)
        self.draw_sc_win.drawAll()

    def addParamGui(self,name,type_param,code_type_param):
        """ ajoute un paramètre à la GUI et à la liste locale de paramètres
        et retourne la groupBox créée
        """
        groupBox_8 = QtGui.QFrame(self.ui.scrollArea)
        #groupBox_8.setFrameShape(QtGui.QFrame.StyledPanel)
        #groupBox_8.setFrameShadow(QtGui.QFrame.Raised)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_8.sizePolicy().hasHeightForWidth())
        groupBox_8.setSizePolicy(sizePolicy)
        groupBox_8.setMinimumSize(QtCore.QSize(0, 26))
        groupBox_8.setMaximumSize(QtCore.QSize(16777215, 26))
        groupBox_8.setObjectName("groupBox_8")
        horizontalLayout_13 = QtGui.QHBoxLayout(groupBox_8)
        horizontalLayout_13.setObjectName("horizontalLayout_13")
        horizontalLayout_13.setContentsMargins(-1, 1, -1, 1)
        groupBox_9 = QtGui.QGroupBox(groupBox_8)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_9.sizePolicy().hasHeightForWidth())
        groupBox_9.setSizePolicy(sizePolicy)
        groupBox_9.setMinimumSize(QtCore.QSize(140, 25))
        groupBox_9.setMaximumSize(QtCore.QSize(140, 25))
        groupBox_9.setObjectName("groupBox_9")
        horizontalLayout_8 = QtGui.QHBoxLayout(groupBox_9)
        horizontalLayout_8.setObjectName("horizontalLayout_8")
        horizontalLayout_8.setContentsMargins(-1, 1, -1, 1)
        paramNameLabel = QtGui.QLabel(name,groupBox_9)
        paramNameLabel.setObjectName("paramNameLabel")
        horizontalLayout_8.addWidget(paramNameLabel)
        setCondButton = QtGui.QPushButton("set condition",groupBox_9)
        setCondButton.setObjectName("setCondButton")
        setCondButton.setMinimumSize(QtCore.QSize(80, 20))
        setCondButton.setMaximumSize(QtCore.QSize(80, 20))
        if "darwin" in sys.platform:
        	ff = setCondButton.font()
        	ff.setPointSize(10)
        	setCondButton.setFont(ff)
        else:
            ff = setCondButton.font()
            ff.setPointSize(8)
            setCondButton.setFont(ff)
        horizontalLayout_8.addWidget(setCondButton)
        horizontalLayout_13.addWidget(groupBox_9)
        groupBox_10 = QtGui.QGroupBox(groupBox_8)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_10.sizePolicy().hasHeightForWidth())
        groupBox_10.setSizePolicy(sizePolicy)
        groupBox_10.setMinimumSize(QtCore.QSize(290, 25))
        groupBox_10.setMaximumSize(QtCore.QSize(290, 25))
        groupBox_10.setObjectName("groupBox_10")
        horizontalLayout_9 = QtGui.QHBoxLayout(groupBox_10)
        horizontalLayout_9.setObjectName("horizontalLayout_9")
        horizontalLayout_9.setContentsMargins(-1, 1, -1, 1)
        uniformParamRadio = QtGui.QRadioButton(groupBox_10)
        uniformParamRadio.setChecked(True)
        uniformParamRadio.setObjectName("uniformParamRadio")
        uniformParamRadio.setMinimumSize(QtCore.QSize(20, 0))
        horizontalLayout_9.addWidget(uniformParamRadio)
        logUniformRadio = QtGui.QRadioButton(groupBox_10)
        logUniformRadio.setObjectName("logUniformRadio")
        logUniformRadio.setMinimumSize(QtCore.QSize(20, 0))
        horizontalLayout_9.addWidget(logUniformRadio)
        normalRadio = QtGui.QRadioButton(groupBox_10)
        normalRadio.setObjectName("normalRadio")
        normalRadio.setMinimumSize(QtCore.QSize(20, 0))
        horizontalLayout_9.addWidget(normalRadio)
        logNormalRadio = QtGui.QRadioButton(groupBox_10)
        logNormalRadio.setObjectName("logNormalRadio")
        logNormalRadio.setMinimumSize(QtCore.QSize(20, 0))
        ## alignement des radio
        #uniformParamRadio.setAlignment(QtCore.Qt.AlignCenter)
        #logUniformRadio.setAlignment(QtCore.Qt.AlignCenter)
        #normalRadio.setAlignment(QtCore.Qt.AlignCenter)
        #logNormalRadio.setAlignment(QtCore.Qt.AlignCenter)
        horizontalLayout_9.addWidget(logNormalRadio)
        horizontalLayout_9.insertStretch(4)
        horizontalLayout_9.insertStretch(3)
        horizontalLayout_9.insertStretch(2)
        horizontalLayout_9.insertStretch(1)
        horizontalLayout_9.insertStretch(0)
        horizontalLayout_13.addWidget(groupBox_10)
        groupBox_14 = QtGui.QGroupBox(groupBox_8)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(groupBox_14.sizePolicy().hasHeightForWidth())
        groupBox_14.setSizePolicy(sizePolicy)
        groupBox_14.setMinimumSize(QtCore.QSize(0, 25))
        groupBox_14.setMaximumSize(QtCore.QSize(16777215,25))
        groupBox_14.setObjectName("groupBox_14")
        horizontalLayout_10 = QtGui.QHBoxLayout(groupBox_14)
        horizontalLayout_10.setObjectName("horizontalLayout_10")
        horizontalLayout_10.setContentsMargins(-1, 1, -1, 1)
        minValueParamEdit = QtGui.QLineEdit(groupBox_14)
        minValueParamEdit.setAlignment(Qt.AlignRight)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(minValueParamEdit.sizePolicy().hasHeightForWidth())
        minValueParamEdit.setSizePolicy(sizePolicy)
        minValueParamEdit.setMinimumSize(QtCore.QSize(60, 0))
        minValueParamEdit.setMaximumSize(QtCore.QSize(60, 16777215))
        minValueParamEdit.setObjectName("minValueParamEdit")
        horizontalLayout_10.addWidget(minValueParamEdit)
        maxValueParamEdit = QtGui.QLineEdit(groupBox_14)
        maxValueParamEdit.setAlignment(Qt.AlignRight)
        maxValueParamEdit.setMinimumSize(QtCore.QSize(60, 0))
        maxValueParamEdit.setMaximumSize(QtCore.QSize(60, 16777215))
        maxValueParamEdit.setObjectName("maxValueParamEdit")
        horizontalLayout_10.addWidget(maxValueParamEdit)
        meanValueParamEdit = QtGui.QLineEdit(groupBox_14)
        meanValueParamEdit.setAlignment(Qt.AlignRight)
        meanValueParamEdit.setMinimumSize(QtCore.QSize(60, 0))
        meanValueParamEdit.setMaximumSize(QtCore.QSize(60, 16777215))
        meanValueParamEdit.setObjectName("meanValueParamEdit")
        horizontalLayout_10.addWidget(meanValueParamEdit)
        stValueParamEdit = QtGui.QLineEdit(groupBox_14)
        stValueParamEdit.setAlignment(Qt.AlignRight)
        stValueParamEdit.setMinimumSize(QtCore.QSize(60, 0))
        stValueParamEdit.setMaximumSize(QtCore.QSize(60, 16777215))
        stValueParamEdit.setObjectName("stValueParamEdit")
        horizontalLayout_10.addWidget(stValueParamEdit)
        #stepValueParamEdit = QtGui.QLineEdit(groupBox_14)
        #stepValueParamEdit.setMinimumSize(QtCore.QSize(60, 0))
        #stepValueParamEdit.setMaximumSize(QtCore.QSize(60, 16777215))
        #stepValueParamEdit.setObjectName("stepValueParamEdit")
        #horizontalLayout_10.addWidget(stepValueParamEdit)
        horizontalLayout_13.addWidget(groupBox_14)
        self.ui.verticalLayout_6.addWidget(groupBox_8)

        # liste locale des paramètres
        self.paramList.append(groupBox_8)

        # evennement d'ajout d'une condition sur un paramètre
        QObject.connect(setCondButton,SIGNAL("clicked()"),self.setConditionAction)
        # désactivation de mean et st en uniform et log-uniform
        QObject.connect(uniformParamRadio,SIGNAL("toggled(bool)"),self.toggleMeanSt)
        QObject.connect(logUniformRadio,SIGNAL("toggled(bool)"),self.toggleMeanSt)

        # par defaut on coche "normal" pour déclencher le signal toggle si
        # on coche ensuite unif ou logunif
        normalRadio.setChecked(True)

        if type_param == "unique":
            setCondButton.hide()
        pref = self.parent.parent.preferences_win
        if code_type_param == "N":
            minValueParamEdit.setText(pref.hist_model.NminEdit.text())
            maxValueParamEdit.setText(pref.hist_model.NmaxEdit.text())
            meanValueParamEdit.setText(pref.hist_model.NmeanEdit.text())
            stValueParamEdit.setText(pref.hist_model.NstdevEdit.text())
            if pref.hist_model.NUNRadio.isChecked():
                uniformParamRadio.setChecked(True)
            elif pref.hist_model.NLNRadio.isChecked():
                logUniformRadio.setChecked(True)
            elif pref.hist_model.NLURadio.isChecked():
                logUniformRadio.setChecked(True)
            else:
                normalRadio.setChecked(True)
        if code_type_param == "T":
            minValueParamEdit.setText(pref.hist_model.TminEdit.text())
            maxValueParamEdit.setText(pref.hist_model.TmaxEdit.text())
            meanValueParamEdit.setText(pref.hist_model.TmeanEdit.text())
            stValueParamEdit.setText(pref.hist_model.TstdevEdit.text())
            if pref.hist_model.TUNRadio.isChecked():
                uniformParamRadio.setChecked(True)
            elif pref.hist_model.TLNRadio.isChecked():
                logUniformRadio.setChecked(True)
            elif pref.hist_model.TLURadio.isChecked():
                logUniformRadio.setChecked(True)
            else:
                normalRadio.setChecked(True)
        elif code_type_param == "A":
            minValueParamEdit.setText(pref.hist_model.AminEdit.text())
            maxValueParamEdit.setText(pref.hist_model.AmaxEdit.text())
            meanValueParamEdit.setText(pref.hist_model.AmeanEdit.text())
            stValueParamEdit.setText(pref.hist_model.AstdevEdit.text())
            if pref.hist_model.AUNRadio.isChecked():
                uniformParamRadio.setChecked(True)
            elif pref.hist_model.ALNRadio.isChecked():
                logUniformRadio.setChecked(True)
            elif pref.hist_model.ALURadio.isChecked():
                logUniformRadio.setChecked(True)
            else:
                normalRadio.setChecked(True)

        return groupBox_8

    def toggleMeanSt(self,on):
        box = self.sender().parent().parent()
        box.findChild(QLineEdit,"meanValueParamEdit").setDisabled(on)
        box.findChild(QLineEdit,"stValueParamEdit").setDisabled(on)

    def setConditionAction(self):
        """ methode qui receptionne l'evennement du clic sur "set condition"
        ajout d'une condition sur un paramètre ou une paire de paramètres
        """
        param_src = self.sender().parent().findChild(QLabel,"paramNameLabel").text()
        # construction de la liste des params qui sont dans la même catégorie que le notre
        target_list = []
        for pname in self.param_info_dico.keys():
            if pname != param_src and self.param_info_dico[str(pname)][0] == self.param_info_dico[str(param_src)][0]:
                target_list.append(pname)
        self.setCondition = SetCondition(self.sender().parent().findChild(QLabel,"paramNameLabel").text(),target_list,self)
        self.setCondition.show()

    def checkAll(self,silent=False):
        """ verification de la coherence des valeurs du modèle historique
        """
        # VERIFS, si c'est valide, on change l'icone du setHistModel (verifs sur : valeurs, validite des scenarios, et modifs depuis definePriors
        # pour tous les params, min<=max, tout>0
        problems = ""
        for param in self.paramList:
            pname = str(param.findChild(QLabel,"paramNameLabel").text())
            try:
                if param.findChild(QLineEdit,"minValueParamEdit") != None:
                    min =   float(param.findChild(QLineEdit,"minValueParamEdit").text())
                if param.findChild(QLineEdit,"maxValueParamEdit") != None:
                    max =   float(param.findChild(QLineEdit,"maxValueParamEdit").text())
                if param.findChild(QLineEdit,"meanValueParamEdit") != None:
                    mean =  float(param.findChild(QLineEdit,"meanValueParamEdit").text())
                if param.findChild(QLineEdit,"stValueParamEdit") != None:
                    stdev = float(param.findChild(QLineEdit,"stValueParamEdit").text())
                #step =  float(param.findChild(QLineEdit,"stepValueParamEdit").text())
                if param.findChild(QLineEdit,"minValueParamEdit") != None and param.findChild(QLineEdit,"maxValueParamEdit") != None\
                and param.findChild(QLineEdit,"meanValueParamEdit") != None and param.findChild(QLineEdit,"stValueParamEdit") != None:
                    if min > max or min < 0 or max < 0:
                        problems += "Values of min and max for parameter %s are incoherent\n"%pname
                    if param.findChild(QLineEdit,"meanValueParamEdit").isEnabled() and mean < 0:
                        problems += "Value of mean for parameter %s is incoherent\n"%pname
                    if param.findChild(QLineEdit,"stValueParamEdit").isEnabled() and stdev < 0:
                        problems += "Value of stdev for parameter %s is incoherent\n"%pname
                    # verif min < mean < max si normal ou lognormal
                    if param.findChild(QRadioButton,"normalRadio") != None and param.findChild(QRadioButton,"logNormalRadio") != None\
                    and ( param.findChild(QRadioButton,"normalRadio").isChecked() or param.findChild(QRadioButton,"logNormalRadio").isChecked() ):
                        if mean < min or mean > max:
                            problems += "Mean of parameter %s should be included in [%s , %s]\n"%(pname,min,max)
                # pour lognormal et loguniforme le mean doit être different de zero et le stdev aussi
                if param.findChild(QRadioButton,"logNormalRadio") != None and param.findChild(QRadioButton,"logUniformRadio") != None\
                and param.findChild(QLineEdit,"meanValueParamEdit") != None:
                    if param.findChild(QRadioButton,"logNormalRadio").isChecked() or param.findChild(QRadioButton,"logUniformRadio").isChecked():
                        if param.findChild(QLineEdit,"meanValueParamEdit").isEnabled():
                            if mean <= 0:
                                problems += "Mean of parameter %s should be positive\n"%pname
                        if param.findChild(QLineEdit,"stValueParamEdit").isEnabled():
                            if stdev <= 0:
                                problems += "St-dev of parameter %s should be positive\n"%pname
            except Exception as e:
                problems += "%s\n"%e
        rpsum = 0.0
        for rp in self.rpList:
            rpscname = str(rp.findChild(QLabel,"rpLabel").text())
            try:
                rpfloat = float(str(rp.findChild(QLineEdit,"rpEdit").text()))
                rpsum+=rpfloat
            except Exception as e:
                problems += "On %s posterior probability, %s\n"%(rpscname,e)
        if rpsum < 0.999 or rpsum > 1.001 and self.ui.otherRadio.isChecked():
            problems += "The sum of all posterior probabilities is equal to %s. It should be equal to 1"%rpsum

        if problems == "":
            if self.checkScenarios(silent=silent) != None:
                if not self.scenarios_modified_since_define_priors:
                    return True
                else:
                    if not silent:
                        output.notify(self,"Error","The scenarios were modified since the last 'Define priors'\nPriors must be defined in order to validate the historical model")
            else:
                return False
        else:
            if not silent:
                output.notify(self,"Value error","%s"%problems)
            return False

    def majParamInfoDico(self):
        """ met a jour les infos des paramètres dans l'attribut param_info_dico
        """
        # recup des valeurs pour les params
        for param in self.paramList:
            pname = str(param.findChild(QLabel,"paramNameLabel").text())
            min =   str(param.findChild(QLineEdit,"minValueParamEdit").text())
            max =   str(param.findChild(QLineEdit,"maxValueParamEdit").text())
            mean =  str(param.findChild(QLineEdit,"meanValueParamEdit").text())
            stdev = str(param.findChild(QLineEdit,"stValueParamEdit").text())
            #step =  str(param.findChild(QLineEdit,"stepValueParamEdit").text())
            if param.findChild(QRadioButton,'logNormalRadio').isChecked():
                law = "LN"
            elif param.findChild(QRadioButton,'normalRadio').isChecked():
                law = "NO"
            elif param.findChild(QRadioButton,'uniformParamRadio').isChecked():
                law = "UN"
            elif param.findChild(QRadioButton,'logUniformRadio').isChecked():
                law = "LU"
            visible = param.findChild(QPushButton,"setCondButton").isVisible()
            self.param_info_dico[pname] = [self.param_info_dico[pname][0],law,min,max,mean,stdev,visible]


    def validate(self):
        """ vérifie la validité du modèle historique et quitte
        """
        #self.majParamInfoDico()
        # VERIFS, si c'est bon, on change d'onglet, sinon on reste
        if self.checkAll():
            # creation et ecriture du fichier dans le rep choisi
            self.writeHistoricalConfFromGui()
            self.parent.setHistValid(True)

            self.majProjectGui()

            self.returnToProject()

    def getNbScenario(self):
        return len(self.scList)

    def majProjectGui(self):
        """ ecrit les infos sur le nombre de scenarios et de params
        dans l'ecran general du projet
        """
        # gestion des valeurs, maj dans la GUI
        nb_sc = len(self.scList)
        pluriel = ""
        if nb_sc > 1:
            pluriel = "s"
        self.parent.setNbScenarios("%i scenario%s"%(nb_sc,pluriel))
        nb_params = len(self.paramList)
        pluriel = ""
        if nb_params > 1:
            pluriel = "s"
        self.parent.setNbParams("%i historical parameter%s"%(nb_params,pluriel))

    def returnToProject(self):
        """ reactive les onglets du projet et efface celui du model historique
        """
        ## reactivation des onglets
        #self.parent.setTabEnabled(0,True)
        #self.parent.setTabEnabled(1,True)
        #self.parent.removeTab(self.parent.indexOf(self.parent.hist_model_win))
        #self.parent.setCurrentIndex(0)
        self.parent.ui.refTableStack.removeWidget(self)
        self.parent.ui.refTableStack.setCurrentIndex(0)

    def closeEvent(self, event):
        # le cancel ne vérifie rien
        if self.sender() == self.ui.clearButton:
            self.parent.clearHistoricalModel()
        elif self.sender() == self.ui.exitButton:
            #self.majParamInfoDico()
            # si on fait exit, ça ne sauve pas
            #self.writeHistoricalConfFromGui()
            self.majProjectGui()
            self.returnToProject()

    def writeHistoricalConfFromGui(self):
        """ ecrit les valeurs dans dossierProjet/conf.hist.tmp
        """
        log(3,"Writing historical save file of project %s"%self.parent.name)
        self.majParamInfoDico()
        if os.path.exists(self.parent.ui.dirEdit.text()+"/%s"%self.parent.parent.hist_conf_name):
            os.remove("%s/%s"%(self.parent.ui.dirEdit.text(),self.parent.parent.hist_conf_name))

        f = open(self.parent.ui.dirEdit.text()+"/%s"%self.parent.parent.hist_conf_name,'w')
        f.write("%s scenarios: "%(len(self.scList)))

        # affichage des nombres de lignes des scenarios
        for scbox in self.scList:
            txt = str(scbox.findChild(QPlainTextEdit,"scplainTextEdit").toPlainText()).strip().split('\n')
            f.write("%s "%(len(txt)))
        # affichage du contenu des scenarios
        num = 0
        while num < len(self.scList):
            txt = str(self.scList[num].findChild(QPlainTextEdit,"scplainTextEdit").toPlainText()).strip().split('\n')
            prior_proba = self.rpList[num].findChild(QLineEdit,"rpEdit").text()
            # l'user fait "save project" alors qu'on vient de charger et que define priors n'a toujours pas été fait
            # : on ne renseigne pas le nombre de paramètres pour chaque scenario
            if len(self.scenarios_info_list) > 0:
                f.write("\nscenario %s [%s] (%s)"%(num+1,prior_proba,len(self.scenarios_info_list[num]["checker"].parameters)))
            else:
                f.write("\nscenario %s [%s] (X)"%(num+1,prior_proba))
            for line in txt:
                f.write("\n"+line)
            num+=1
        f.write("\n\nhistorical parameters priors (%s,%i)"%(len(self.param_info_dico.keys()),len(self.condList)))
        # consultation des valeurs des paramètres
        # la gui est mise a jour uniquement si les scenarios sont valides
        # DONC on peut se servir des infos hors GUI
        # Mais on rajoute un truc pour savoir si on affiche ou pas le setConditionButton
        # pour préserver l'ordre, on consulte la gui pour les noms
        #for pname in self.param_info_dico.keys():
        for pbox in self.paramList:
            pname = str(pbox.findChild(QLabel,"paramNameLabel").text())
            info = self.param_info_dico[pname]
            # recherche de la box du param
            name = ""
            i = 0
            #visible = 0
            while i<len(self.paramList) and name != pname:
                paramBox = self.paramList[i]
                name = paramBox.findChild(QLabel,"paramNameLabel").text()                    
                #visible = paramBox.findChild(QPushButton,"setCondButton").isVisible()
                #print "visible : %s"%visible
                i+=1
            f.write("\n%s %s %s[%s,%s,%s,%s] %s"%(pname,info[0],info[1],info[2],info[3],info[4],info[5],info[6]))
        if len(self.condList) > 0:
            for cond in self.condList:
                lab = cond.findChild(QLabel,"condLabel")
                f.write("\n%s"%lab.text().replace(' ',''))
            if self.ui.drawUntilRadio.isChecked():
                draw = "UNTIL"
            else:
                draw = "ONCE"
            f.write("\nDRAW %s"%draw)

        #print "passage à la ligne!!!"
        f.write("\n\n")
        f.close()

    def loadHistoricalConf(self):
        """ Charge l'etat de setHistoricalModel à partir de conf.hist.tmp
        """
        if os.path.exists(self.parent.dir):
            if os.path.exists("%s/%s"%(self.parent.dir,self.parent.parent.hist_conf_name)):
                f = open("%s/%s"%(self.parent.dir,self.parent.parent.hist_conf_name),"r")
                lines = f.readlines()

                self.ui.otherRadio.setChecked(True)

                nb_scenarios = int(lines[0].split(' ')[0])
                nb_line_sc = []
                for i in range(nb_scenarios):
                    nb_line_sc.append(int(lines[0].split(' ')[2+i]))
                l = 1
                for i in range(nb_scenarios):
                    txt=""
                    prior = lines[l].split('[')[1].split(']')[0]
                    ldeb_sc_cur = l+1
                    l+=1
                    while l < (ldeb_sc_cur+nb_line_sc[i]):
                        #print l," ",l+nb_line_sc[i]+1
                        txt+="%s"%lines[l]
                        l+=1
                    self.addSc()
                    self.scList[i].findChild(QPlainTextEdit,"scplainTextEdit").setPlainText(txt)
                    self.rpList[i].findChild(QLineEdit,"rpEdit").setText(prior)
                # en sortie l est (theoriquement) sur la ligne vide
                l+=1
                if l >= len(lines): return 0
                #print lines[l]
                nbparam = int(lines[l].split('(')[1].split(',')[0])
                nbcond = int(lines[l].split('(')[1].split(',')[1].split(')')[0])
                l+=1
                lfirst_param = l
                while l<(lfirst_param + nbparam):
                    pname = lines[l].split(' ')[0]
                    category = lines[l].split(' ')[1]
                    values = lines[l].split('[')[1].split(']')[0]
                    min = values.split(',')[0]
                    max = values.split(',')[1]
                    mean = values.split(',')[2]
                    stdev = values.split(',')[3]
                    #step = values.split(',')[4]
                    law = lines[l].split(' ')[2].split('[')[0]
                    visible = lines[l].split(' ')[3].strip()
                    if visible == "False":
                        setcond_visible = "unique"
                    else:
                        setcond_visible = "multiple"
                    # ajout graphique et dans l'attribut
                    self.param_info_dico[pname] = [category,law,min,max,mean,stdev]
                    box = self.addParamGui(pname,setcond_visible,self.param_info_dico[pname][0])
                    # mise au point valeurs et loi
                    box.findChild(QLineEdit,"minValueParamEdit").setText(min)
                    box.findChild(QLineEdit,"maxValueParamEdit").setText(max)
                    box.findChild(QLineEdit,"meanValueParamEdit").setText(mean)
                    box.findChild(QLineEdit,"stValueParamEdit").setText(stdev)
                    #box.findChild(QLineEdit,"stepValueParamEdit").setText(step)
                    if law == "UN":
                        box.findChild(QRadioButton,"uniformParamRadio").setChecked(True)
                    elif law == "LU":
                        box.findChild(QRadioButton,"logUniformRadio").setChecked(True)
                    elif law == "NO":
                        box.findChild(QRadioButton,"normalRadio").setChecked(True)
                    elif law == "LN":
                        box.findChild(QRadioButton,"logNormalRadio").setChecked(True)
                    l+=1
                # l est à la première condition
                num_cond = 0
                while l < len(lines) and num_cond < nbcond:
                    line = lines[l].strip()
                    if line != "":
                        if line.split(' ')[0] == "DRAW":
                            if line.split(' ')[1] == "UNTIL":
                                self.ui.drawUntilRadio.setChecked(True)
                            else:
                                self.ui.drawOnceRadio.setChecked(True)
                        else:
                            self.addCondition(line)
                    l+=1
            self.majProjectGui()                
        else:
            output.notify(self,"Error","The project directory does not exist anymore")

    def getNbParam(self):
        """ retourne le nombre de paramètres du modèle historique
        """
        return len(self.paramList)

    def getNbVariableParam(self):
        """ retourne le nombre de paramètres du modèle historique
        """
        nbParam = 0
        for box in self.paramList:
            mini = float(box.findChild(QLineEdit,"minValueParamEdit").text())
            maxi = float(box.findChild(QLineEdit,"maxValueParamEdit").text())
            if (maxi > 0) and (maxi-mini > 0.000001):
                nbParam+=1
        return nbParam

    def getParamTableHeader(self):
        """ retourne la chaine des noms des paramètres pour le conf.th.tmp
        """
        result = u""
        for box in self.paramList:
            mini = float(box.findChild(QLineEdit,"minValueParamEdit").text())
            maxi = float(box.findChild(QLineEdit,"maxValueParamEdit").text())
            if (maxi > 0) and (maxi-mini > 0.000001):
                pname = str(box.findChild(QLabel,"paramNameLabel").text()).strip()
                result += output.centerHeader(pname,14)
                #for i in range(14-len(pname)):
                #    result += " "
        return result
