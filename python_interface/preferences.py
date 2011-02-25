# -*- coding: utf-8 -*-

import os
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from preferences_ui import Ui_MainWindow
from setMutationModel import SetMutationModel
from setMutationModelSequences import SetMutationModelSequences

class Preferences(QMainWindow):
    """ Classe principale qui est aussi la fenêtre principale de la GUI
    """
    def __init__(self,parent=None):
        super(Preferences,self).__init__(parent)
        self.createWidgets()

        self.ui.tabWidget.setTabText(0,"Connexion")
        self.ui.tabWidget.setTabText(1,"Historical")
        self.ui.tabWidget.setTabText(2,"Various")
        self.mutmodM = SetMutationModel(self)
        self.mutmodS = SetMutationModelSequences(self)
        self.ui.tabWidget.addTab(self.mutmodM,"MM Microsats")
        self.ui.tabWidget.addTab(self.mutmodS,"MM Sequences")

        self.mutmodM.ui.frame_6.hide()
        self.mutmodM.ui.setMutMsLabel.setText("Default values for mutation model of Microsatellites")
        self.mutmodS.ui.frame_6.hide()
        self.mutmodS.ui.setMutSeqLabel.setText("Default values for mutation model of Sequences")


    def createWidgets(self):
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        self.ui.gridLayout.setAlignment(Qt.AlignTop)
        self.ui.verticalLayout_57.setAlignment(Qt.AlignTop)
        #self.ui.verticalLayout_54.setAlignment(Qt.AlignTop)
        #self.ui.verticalLayout_29.setAlignment(Qt.AlignTop)
        #self.ui.verticalLayout_26.setAlignment(Qt.AlignTop)

        QObject.connect(self.ui.saveButton,SIGNAL("clicked()"),self.savePreferences)
        QObject.connect(self.ui.cancelButton,SIGNAL("clicked()"),self.close)

        #QObject.connect(self.ui.saveMMMButton,SIGNAL("clicked()"),self.saveMMM)
        #QObject.connect(self.ui.saveMMSButton,SIGNAL("clicked()"),self.saveMMS)

    def savePreferences(self):
        if self.allValid():
            if not os.path.exists(os.path.expanduser("~/.diyabc/")):
                os.mkdir(os.path.expanduser("~/.diyabc/"))
            self.saveMMM()
            self.saveMMS()
            self.saveConnexion()
            self.saveHM()
            self.close()

    def allValid(self):
        return (self.mutmodM.allValid() and self.mutmodS.allValid() and self.histModelValid() and self.connexionValid())

    def loadPreferences(self):
        if os.path.exists(os.path.expanduser("~/.diyabc/")):
            self.loadMMM()
            self.loadMMS()
            self.loadConnexion()
            self.loadHM()

    # TODO
    def saveHM(self):
        if os.path.exists(os.path.expanduser("~/.diyabc/hist_model_default_values")):
            os.remove(os.path.expanduser("~/.diyabc/hist_model_default_values"))

        nmin = float(self.ui.NminEdit.text())
        nmax = float(self.ui.NmaxEdit.text())
        nmean = float(self.ui.NmeanEdit.text())
        nst = float(self.ui.NstdevEdit.text())
        tmin = float(self.ui.TminEdit.text())
        tmax = float(self.ui.TmaxEdit.text())
        tmean = float(self.ui.TmeanEdit.text())
        tst = float(self.ui.TstdevEdit.text())
        amin = float(self.ui.AminEdit.text())
        amax = float(self.ui.AmaxEdit.text())
        amean = float(self.ui.AmeanEdit.text())
        ast = float(self.ui.AstdevEdit.text())

        lines = "nmin %s\n"%nmin
        lines += "nmax %s\n"%nmax
        lines += "nmean %s\n"%nmean
        lines += "nst %s\n"%nst
        lines += "tmin %s\n"%tmin
        lines += "tmax %s\n"%tmax
        lines += "tmean %s\n"%tmean
        lines += "tst %s\n"%tst
        lines += "amin %s\n"%amin
        lines += "amax %s\n"%amax
        lines += "amean %s\n"%amean
        lines += "ast %s\n"%ast
        if self.ui.NUNRadio.isChecked():
            nlaw = "UN"
        elif self.ui.NLURadio.isChecked():
            nlaw = "LU"
        elif self.ui.NNORadio.isChecked():
            nlaw = "NO"
        elif self.ui.NLNRadio.isChecked():
            nlaw = "LN"
        if self.ui.TUNRadio.isChecked():
            tlaw = "UN"
        elif self.ui.TLURadio.isChecked():
            tlaw = "LU"
        elif self.ui.TNORadio.isChecked():
            tlaw = "NO"
        elif self.ui.TLNRadio.isChecked():
            tlaw = "LN"
        if self.ui.AUNRadio.isChecked():
            alaw = "UN"
        elif self.ui.ALURadio.isChecked():
            alaw = "LU"
        elif self.ui.ANORadio.isChecked():
            alaw = "NO"
        elif self.ui.ALNRadio.isChecked():
            alaw = "LN"
        lines += "alaw %s\n"%alaw
        lines += "nlaw %s\n"%nlaw
        lines += "tlaw %s\n"%tlaw

        f = codecs.open(os.path.expanduser("~/.diyabc/hist_model_default_values"),"w","utf-8")
        f.write(lines)
        f.close()
    def loadHM(self):
        if os.path.exists(os.path.expanduser("~/.diyabc/hist_model_default_values")):
            f = codecs.open(os.path.expanduser("~/.diyabc/hist_model_default_values"),"r","utf-8")
            lines = f.readlines()
            f.close()
            dico_val = {}
            for l in lines:
                tab = l.strip().split(' ')
                if len(tab)>1:
                    dico_val[tab[0]] = tab[1]

            self.ui.NminEdit.setText(dico_val["nmin"])
            self.ui.NmaxEdit.setText(dico_val["nmax"])
            self.ui.NmeanEdit.setText(dico_val["nmean"])
            self.ui.NstdevEdit.setText(dico_val["nst"])
            self.ui.TminEdit.setText(dico_val["tmin"])
            self.ui.TmaxEdit.setText(dico_val["tmax"])
            self.ui.TmeanEdit.setText(dico_val["tmean"])
            self.ui.TstdevEdit.setText(dico_val["tst"])
            self.ui.AminEdit.setText(dico_val["amin"])
            self.ui.AmaxEdit.setText(dico_val["amax"])
            self.ui.AmeanEdit.setText(dico_val["amean"])
            self.ui.AstdevEdit.setText(dico_val["ast"])
            if dico_val["nlaw"] == "UN":
                self.ui.NUNRadio.setChecked(True)
            if dico_val["nlaw"] == "LN":
                self.ui.NLNRadio.setChecked(True)
            if dico_val["nlaw"] == "NO":
                self.ui.NNORadio.setChecked(True)
            if dico_val["nlaw"] == "LU":
                self.ui.NLURadio.setChecked(True)
            if dico_val["alaw"] == "UN":
                self.ui.AUNRadio.setChecked(True)
            if dico_val["alaw"] == "LU":
                self.ui.ALURadio.setChecked(True)
            if dico_val["alaw"] == "NO":
                self.ui.ANORadio.setChecked(True)
            if dico_val["alaw"] == "LN":
                self.ui.ALNRadio.setChecked(True)
            if dico_val["tlaw"] == "UN":
                self.ui.TUNRadio.setChecked(True)
            if dico_val["tlaw"] == "LN":
                self.ui.TLNRadio.setChecked(True)
            if dico_val["tlaw"] == "LU":
                self.ui.TLURadio.setChecked(True)
            if dico_val["tlaw"] == "NO":
                self.ui.TNORadio.setChecked(True)
        else:
            print "no hist conf found"
        
    def saveMMM(self):
        if os.path.exists(os.path.expanduser("~/.diyabc/mutationM_default_values")):
            os.remove(os.path.expanduser("~/.diyabc/mutationM_default_values"))

        lines = self.mutmodM.getMutationConf()
        f = codecs.open(os.path.expanduser("~/.diyabc/mutationM_default_values"),"w","utf-8")
        f.write(lines)
        f.close()
    def loadMMM(self):
        if os.path.exists(os.path.expanduser("~/.diyabc/mutationM_default_values")):
            f = codecs.open(os.path.expanduser("~/.diyabc/mutationM_default_values"),"r","utf-8")
            lines = f.readlines()
            f.close()
            self.mutmodM.setMutationConf(lines)
        else:
            print "no MMM conf found"



    def saveMMS(self):
        if os.path.exists(os.path.expanduser("~/.diyabc/mutationS_default_values")):
            os.remove(os.path.expanduser("~/.diyabc/mutationS_default_values"))

        lines = self.mutmodS.getMutationConf()
        f = codecs.open(os.path.expanduser("~/.diyabc/mutationS_default_values"),"w","utf-8")
        f.write(lines)
        f.close()

    def loadMMS(self):
        if os.path.exists(os.path.expanduser("~/.diyabc/mutationS_default_values")):
            f = codecs.open(os.path.expanduser("~/.diyabc/mutationS_default_values"),"r","utf-8")
            lines = f.readlines()
            f.close()
            self.mutmodS.setMutationConf(lines)
        else:
            print "no MMS conf found"

    def saveConnexion(self):
        if os.path.exists(os.path.expanduser("~/.diyabc/connexion")):
            os.remove(os.path.expanduser("~/.diyabc/connexion"))

        lines = "host %s\n"%self.ui.addrEdit.text()
        lines += "port %s\n"%self.ui.portEdit.text()

        f = codecs.open(os.path.expanduser("~/.diyabc/connexion"),"w","utf-8")
        f.write(lines)
        f.close()
    def loadConnexion(self):
        if os.path.exists(os.path.expanduser("~/.diyabc/connexion")):
            f = codecs.open(os.path.expanduser("~/.diyabc/connexion"),"r","utf-8")
            lines = f.readlines()
            f.close()
            for l in lines:
                tab = l.split(' ')
                if len(tab)>1:
                    if tab[0] == "host":
                        self.ui.addrEdit.setText(tab[1])
                    elif tab[0] == "port":
                        self.ui.portEdit.setText(tab[1])
        else:
            print "no connexion conf found"

    def connexionValid(self):
        problems = ""
        try:
            port = int(self.ui.portEdit.text())
        except Exception,e:
            problems += "%s for port connexion value"%e

        if problems == "":
            return True
        else:
            QMessageBox.information(self,"Value error","%s"%problems)
            return False

    def histModelValid(self):
        problems = ""
        try:
            nmin = float(self.ui.NminEdit.text())
            nmax = float(self.ui.NmaxEdit.text())
            nmean = float(self.ui.NmeanEdit.text())
            nst = float(self.ui.NstdevEdit.text())
            tmin = float(self.ui.TminEdit.text())
            tmax = float(self.ui.TmaxEdit.text())
            tmean = float(self.ui.TmeanEdit.text())
            tst = float(self.ui.TstdevEdit.text())
            amin = float(self.ui.AminEdit.text())
            amax = float(self.ui.AmaxEdit.text())
            amean = float(self.ui.AmeanEdit.text())
            ast = float(self.ui.AstdevEdit.text())
            if nmin > nmax or tmin > tmax or amin > amax:
                problems += "A min is superior than a max\n"
        except Exception,e:
            problems += "%s in historical model values\n"%e

        if problems == "":
            return True
        else:
            QMessageBox.information(self,"Value error","%s"%problems)
            return False





