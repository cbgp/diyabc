# -*- coding: utf-8 -*-

## @package python_interface.preferences
# @author Julien Veyssier
#
# @brief Fenêtre pour gérer les préférences personnelles

import os,sys,platform,multiprocessing
import ConfigParser
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.preferences_ui import Ui_MainWindow
from mutationModel.setMutationModelMsat import SetMutationModelMsat
from mutationModel.setMutationModelSequences import SetMutationModelSequences
import output
import utils.cbgpUtils as utilsPack
from utils.cbgpUtils import log
from utils.autoPreferences import AutoPreferences

## @class Preferences
# @brief Fenêtre pour gérer les préférences personnelles
class Preferences(AutoPreferences):
    """ Classe principale qui est aussi la fenêtre principale de la GUI
    """
    def __init__(self,parent=None,configfile=None):
        super(Preferences,self).__init__(parent,configfile)

        self.tabColor = {"green": "#c3ffa6","blue":"#7373ff","red":"#ffb2a6","yellow":"#ffffb2","pink":"#ffbff2"}

        # initialisation du combo pour le nombre max de thread
        try:
            nb_core = multiprocessing.cpu_count()
        except Exception as e:
            nb_core = 1
            log(3,"Impossible to count core number on this computer")
        dicoValTxtMaxThread = {}
        for i in range(1,nb_core+1):
            dicoValTxtMaxThread[str(i)] = str(i)

        dicoValTxtLogLvl = {"1":"1 : Human actions",
                 "2":"2 : High level actions (file read, checks)",
                 "3":"3 : Small actions",
                 "4":"4 : Details"}

        formats = ["pdf","svg","jpg","png"]
        dicoValTxtFormat = {}
        for i in formats:
            dicoValTxtFormat[i] = i

        self.styles = []
        for i in QStyleFactory.keys():
            self.styles.append(str(i))
            #self.ui.styleCombo.addItem(str(i))
        default = None
        if "Cleanlooks" in self.styles:
            default = "Cleanlooks"
        
        dicoValTxtStyle = {}
        for i in self.styles:
            dicoValTxtStyle[i] = i

        colors = ["default","white"]
        colors.extend(self.tabColor.keys())
        dicoValTxtColor = {}
        for i in colors:
            dicoValTxtColor[i] = i


        # hashtable contenant les informations des champs
        dico_fields = {
                "connexion" : [
                    ["check","useServer","Use a server (don't check if you don't know what it is)",False],
                    ["lineEdit","serverAddress","Address of the server","localhost"],
                    ["lineEdit","serverPort","Port of the server","666"]
                ],
                "various" : [
                    ["check","showTrayIcon","Show tray icon",False],
                    ["check","activateWhatsThis","Activate ""what's this"" help functionnality",True],
                    ["check","debugWhatsThis","Show object name in what's this (needs restart)",False],
                    ["check","useDefaultExecutable","Use default executable check",True],
                    ["pathEdit","execPath","Path to the executable file",""],
                    ["lineEdit","particleLoopSize","Particle loop size","100"],
                    ["combo","maxThread",dicoValTxtMaxThread,[str(i) for i in range(1,nb_core+1)],str(nb_core),"Maximum thread number"],
                    ["combo","maxLogLvl",dicoValTxtLogLvl,["1","2","3","4"],"3","Maximum log level"],
                    ["combo","picturesFormat",dicoValTxtFormat,formats,"pdf","Graphics and pictures save format \\n(scenario trees, PCA graphics)"],
                    ["combo","style",dicoValTxtStyle,self.styles,default,"Style"],
                    ["combo","backgroundColor",dicoValTxtColor,colors,"default","Window background color"]
                ]
        }
        self.digest(dico_fields)

        QObject.connect(self.ui.styleCombo,SIGNAL("currentIndexChanged(QString)"),self.changeStyle)
        self.changeStyle(self.ui.styleCombo.currentText())
        QObject.connect(self.ui.maxLogLvlCombo,SIGNAL("currentIndexChanged(int)"),self.changeLogLevel)
        QObject.connect(self.ui.backgroundColorCombo,SIGNAL("currentIndexChanged(QString)"),self.changeBackgroundColor)

        QObject.connect(self.ui.useServerCheck,SIGNAL("toggled(bool)"),self.toggleServer)
        QObject.connect(self.ui.useDefaultExecutableCheck,SIGNAL("toggled(bool)"),self.toggleExeSelection)
        QObject.connect(self.ui.activateWhatsThisCheck,SIGNAL("toggled(bool)"),self.toggleWtSelection)
        QObject.connect(self.ui.showTrayIconCheck,SIGNAL("toggled(bool)"),self.toggleTrayIconCheck)

        self.ui.serverAddressEdit.setDisabled(True)
        self.ui.serverPortEdit.setDisabled(True)

        self.toggleServer(self.ui.useServerCheck.isChecked())
        self.toggleExeSelection(self.ui.useDefaultExecutableCheck.isChecked())
        self.toggleWtSelection(self.ui.activateWhatsThisCheck.isChecked())

        self.hist_model = uic.loadUi("uis/historical_preferences_frame.ui")
        self.ui.tabWidget.addTab(self.hist_model,"Historical")
        self.hist_model.verticalLayout.setAlignment(Qt.AlignTop)

        self.mutmodM = SetMutationModelMsat(self)
        self.mutmodS = SetMutationModelSequences(self)
        self.ui.tabWidget.addTab(self.mutmodM,"MM Microsats")
        self.ui.tabWidget.addTab(self.mutmodS,"MM Sequences")

        self.mutmodM.ui.frame_6.hide()
        self.mutmodM.ui.setMutMsLabel.setText("Default values for mutation model of Microsatellites")
        self.mutmodS.ui.frame_6.hide()
        self.mutmodS.ui.setMutSeqLabel.setText("Default values for mutation model of Sequences")

        #self.ui.tabWidget.setCurrentIndex(1)

    def close(self):
        if len(self.parent.project_list) == 0:
            self.parent.switchToWelcomeStack()
        else:
            self.parent.switchToMainStack()

    def savePreferences(self):
        """ sauve les préférences si elles sont valides
        """
        if self.allValid():
            self.saveMMM()
            self.saveMMS()
            self.saveHM()
            self.saveRecent()
            super(Preferences,self).savePreferences()
            self.close()

    def loadPreferences(self):
        """ charge les préférences de l'utilisateur si elles existent
        """
        self.loadMMM()
        self.loadMMS()
        self.loadHM()
        self.loadRecent()
        self.loadToolBarPosition()
        super(Preferences,self).loadPreferences()

    def changeLogLevel(self,index):
        utilsPack.LOG_LEVEL = index + 1

    def toggleServer(self,state):
        self.ui.serverAddressEdit.setDisabled(not state)
        self.ui.serverPortEdit.setDisabled(not state)

    def toggleExeSelection(self,state):
        self.ui.execPathBrowseButton.setDisabled(state)
        self.ui.execPathPathEdit.setDisabled(state)

    def toggleWtSelection(self,state):
        self.parent.toggleWt(state)
        self.debugWhatsThisCheck.setDisabled(not state)
        self.debugWhatsThisCheck.setChecked(False)

    def toggleTrayIconCheck(self,state):
        self.parent.toggleTrayIcon(state)

    def getMaxThreadNumber(self):
        return int(self.ui.maxThreadCombo.currentText())

    def getExecutablePath(self):
        exPath = ""
        if self.ui.useDefaultExecutableCheck.isChecked():
            # LINUX
            if "linux" in sys.platform:
                if "86" in platform.machine() and "64" not in platform.machine():
                    exPath = "docs/executables/diyabc-comput-linux-i386"
                else:
                    exPath = "docs/executables/diyabc-comput-linux-x64"
            # WINDOWS
            elif "win" in sys.platform and "darwin" not in sys.platform:
                if os.environ.has_key("PROCESSOR_ARCHITECTURE") and "86" not in os.environ["PROCESSOR_ARCHITECTURE"]:
                    exPath = "docs/executables/diyabc-comput-win-x64"
                else:
                    exPath = "docs/executables/diyabc-comput-win-i386"
            # MACOS
            elif "darwin" in sys.platform:
                if "86" in platform.machine() and "64" not in platform.machine():
                    exPath = "docs/executables/diyabc-comput-mac-i386"
                else:
                    exPath = "docs/executables/diyabc-comput-mac-x64"
        else:
            return str(self.ui.execPathEdit.text())
        return exPath

    def changeBackgroundColor(self,colorstr):
        if str(colorstr) in self.tabColor.keys():
            self.parent.setStyleSheet("background-color: %s;"%self.tabColor[str(colorstr)])
        elif colorstr == "default":
            output.notify(self,"advice","Restart DIYABC in order to load the default color scheme")
        else:
            self.parent.setStyleSheet("background-color: %s;"%colorstr)

    def changeStyle(self,stylestr):
        """ change le style de l'application (toutes les fenêtres)
        """
        self.parent.app.setStyle(str(stylestr))

    def saveToolBarPosition(self,pos):
        if pos == Qt.LeftToolBarArea:
            c = 1
        elif pos == Qt.RightToolBarArea:
            c = 2
        elif pos == Qt.TopToolBarArea:
            c = 3
        elif pos == Qt.BottomToolBarArea:
            c = 4
        if not self.config.has_key("various"):
            self.config["various"] = {}
        self.config["various"]["toolBarOrientation"] = c

    def loadToolBarPosition(self):
        cfg = self.config
        if cfg.has_key("various") and cfg["various"].has_key("toolBarOrientation"):
            c = cfg["various"]["toolBarOrientation"].strip()
            if c == "1":
                self.parent.setToolBarPosition(Qt.LeftToolBarArea)
            elif c == "2":
                self.parent.setToolBarPosition(Qt.RightToolBarArea)
            elif c =="3":
                self.parent.setToolBarPosition(Qt.TopToolBarArea)
            elif c == "4":
                self.parent.setToolBarPosition(Qt.BottomToolBarArea)

    def loadRecent(self):
            recent_list = []
            if self.config.has_key("recent"):
                log(3, "Loading recent list")
                for num,rec in self.config["recent"].items():
                    if len(rec) > 0 and rec.strip() != "":
                        recent_list.append(rec.strip())
                        log(4, "Loading recent : %s"%rec.strip())
                self.parent.setRecent(recent_list)

    def saveRecent(self):
        log(3,"Saving recent list")
        if not self.config.has_key("recent"):
            self.config["recent"] = {}
        recList = self.parent.getRecent()
        cfgRecentIndex = 0
        for ind,rec in enumerate(recList):
            # si on a qu'un seul exemplaire de ce recent ou bien, si on est le premier
            if (recList.count(rec) > 1 and recList.index(rec) == ind) or recList.count(rec) == 1 :
                log(4,"Saving into recent list : %s"%rec)
                self.config["recent"]["%s"%cfgRecentIndex] = rec
                cfgRecentIndex += 1


    def saveMMM(self):
        """ sauvegarde de la partie mutation model microsats des préférences
        """
        if not self.config.has_key("mutation_m_default_values"):
            self.config["mutation_m_default_values"] = {}

        lines = self.mutmodM.getMutationConf()
        for l in lines.strip().split('\n'):
            ll = l.strip()
            if ll != "":
                ti = ll.split(' ')[0]
                law = ll.split(' ')[1].split('[')[0]
                zero = ll.split('[')[1].split(',')[0]
                one = ll.split(',')[1]
                two = ll.split(',')[2]
                three = ll.split(',')[3].split(']')[0]
                self.config["mutation_m_default_values"][ti+"_law"] = law
                self.config["mutation_m_default_values"][ti+"_zero"] = zero
                self.config["mutation_m_default_values"][ti+"_one"] = one
                self.config["mutation_m_default_values"][ti+"_two"] = two
                self.config["mutation_m_default_values"][ti+"_three"] = three


    def loadMMM(self):
        """ chargement de la partie mutation model microsats des préférences
        """
        lines = []
        dico = {}
        cfg = self.config
        if cfg.has_key("mutation_m_default_values"):
            try:
                for param in ['MEANMU','GAMMU','MEANP','GAMP','MEANSNI','GAMSNI']:
                    exec('lines.append("{0} %s[%s,%s,%s,%s]"%(cfg["mutation_m_default_values"]["{0}_law"],cfg["mutation_m_default_values"]["{0}_zero"],cfg["mutation_m_default_values"]["{0}_one"],cfg["mutation_m_default_values"]["{0}_two"],cfg["mutation_m_default_values"]["{0}_three"]))'.format(param))
                self.mutmodM.setMutationConf(lines)
            except Exception as e:
                log(3,"Malformed mutation_m_default_values in configuration")
                
        else:
            log(3,"No MMM conf found")

    def saveMMS(self):
        """ sauvegarde de la partie mutation model sequences des préférences
        """
        if not self.config.has_key("mutation_s_default_values"):
            self.config["mutation_s_default_values"] = {}
        lines = self.mutmodS.getMutationConf()

        for l in lines.strip().split('\n'):
            ll = l.strip()
            if ll != "":
                ti = ll.split(' ')[0]
                if ti == "MODEL":
                    mod = ll.split(' ')[1]
                    invpc = ll.split(' ')[2]
                    gamsh = ll.split(' ')[3]
                    self.config["mutation_s_default_values"][ti+"_model"] = mod
                    self.config["mutation_s_default_values"][ti+"_inv_sites_pc"] = invpc
                    self.config["mutation_s_default_values"][ti+"_gamma_shape"] = gamsh
                else:
                    law = ll.split(' ')[1].split('[')[0]
                    zero = ll.split('[')[1].split(',')[0]
                    one = ll.split(',')[1]
                    two = ll.split(',')[2]
                    three = ll.split(',')[3].split(']')[0]
                    self.config["mutation_s_default_values"][ti+"_law"] = law
                    self.config["mutation_s_default_values"][ti+"_zero"] = zero
                    self.config["mutation_s_default_values"][ti+"_one"] = one
                    self.config["mutation_s_default_values"][ti+"_two"] = two
                    self.config["mutation_s_default_values"][ti+"_three"] = three

    def loadMMS(self):
        """ chargement de la partie mutation model sequences des préférences
        """
        lines = []
        cfg = self.config
        if cfg.has_key("mutation_s_default_values"):
            try:
                for param in ['MEANMU','GAMMU','MEANK1','GAMK1','MEANK2','GAMK2']:
                    exec('lines.append("{0} %s[%s,%s,%s,%s]"%(cfg["mutation_s_default_values"]["{0}_law"],cfg["mutation_s_default_values"]["{0}_zero"],cfg["mutation_s_default_values"]["{0}_one"],cfg["mutation_s_default_values"]["{0}_two"],cfg["mutation_s_default_values"]["{0}_three"]))'.format(param))

                lines.append("MODEL %s %s %s"%(cfg["mutation_s_default_values"]["MODEL_model"],cfg["mutation_s_default_values"]["MODEL_inv_sites_pc"],cfg["mutation_s_default_values"]["MODEL_gamma_shape"]))
                self.mutmodS.setMutationConf(lines)
            except Exception as e:
                log(3,"Malformed mutation_s_default_values in configuration")
        else:
            log(3,"No MMS conf found")

    def saveHM(self):
        """ sauvegarde de la partie historical model des préférences
        """
        if not self.config.has_key("hist_model_default_values"):
            self.config["hist_model_default_values"] = {}

        if self.hist_model.NUNRadio.isChecked():
            nlaw = "UN"
        elif self.hist_model.NLURadio.isChecked():
            nlaw = "LU"
        elif self.hist_model.NNORadio.isChecked():
            nlaw = "NO"
        elif self.hist_model.NLNRadio.isChecked():
            nlaw = "LN"

        if self.hist_model.TUNRadio.isChecked():
            tlaw = "UN"
        elif self.hist_model.TLURadio.isChecked():
            tlaw = "LU"
        elif self.hist_model.TNORadio.isChecked():
            tlaw = "NO"
        elif self.hist_model.TLNRadio.isChecked():
            tlaw = "LN"

        if self.hist_model.AUNRadio.isChecked():
            alaw = "UN"
        elif self.hist_model.ALURadio.isChecked():
            alaw = "LU"
        elif self.hist_model.ANORadio.isChecked():
            alaw = "NO"
        elif self.hist_model.ALNRadio.isChecked():
            alaw = "LN"

        toSave = {}

        for key in ["nmin","nmax","nmean","nstdev","tmin","tmax","tmean","tstdev","amin","amax","amean","astdev"]:
            editname = key[0].upper() + key[1:]
            exec('toSave["%s"] = float(self.hist_model.%sEdit.text())'%(key,editname))
        toSave["nlaw"] = nlaw
        toSave["tlaw"] = tlaw
        toSave["alaw"] = alaw
        for k in toSave:
            self.config["hist_model_default_values"][k] = toSave[k]

    def loadHM(self):
        """ chargement de la partie historical model des préférences
        """
        cfg = self.config
        if cfg.has_key("hist_model_default_values"):
            dico_val = {}
            pairs = cfg["hist_model_default_values"].items()
            for p in pairs:
                dico_val[p[0]] = p[1]

            try:
                for key in ["nmin","nmax","nmean","nstdev","tmin","tmax","tmean","tstdev","amin","amax","amean","astdev"]:
                    editname = key[0].upper() + key[1:]
                    exec('self.hist_model.%sEdit.setText(str(dico_val["%s"]))'%(editname,key))
                for pair in [("nlaw","UN"),("nlaw","LN"),("nlaw","NO"),("nlaw","LU"),
                        ("alaw","UN"),("alaw","LN"),("alaw","NO"),("alaw","LU"),
                        ("tlaw","UN"),("tlaw","LN"),("tlaw","NO"),("tlaw","LU")]:
                    exec('if dico_val["%s"] == "%s": self.hist_model.%sRadio.setChecked(True)'%(pair[0],pair[1],pair[0][0].upper()+pair[1]))
            except Exception as e:
                log(3,"Malformed hist_model_default_values section in configuration\n\n%s"%e)
        else:
            log(3,"No hist conf found")
