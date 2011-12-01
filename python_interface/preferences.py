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
from utils.configobj.configobj import *

formPreferences,basePreferences = uic.loadUiType("uis/preferences.ui")

## @class Preferences
# @brief Fenêtre pour gérer les préférences personnelles
class Preferences(formPreferences,basePreferences):
    """ Classe principale qui est aussi la fenêtre principale de la GUI
    """
    def __init__(self,parent=None):
        super(Preferences,self).__init__(parent)
        self.parent = parent
        confdir = os.path.expanduser("~/.diyabc/")
        if not os.path.exists(confdir):
            os.mkdir(confdir)

        self.configFile = confdir+"config.cfg"
        #self.config = ConfigParser.ConfigParser()
        #self.loadedConfigs = self.config.read(self.configFile)
        self.config = ConfigObj(self.configFile)

        self.createWidgets()

        self.tabColor = {"green": "#c3ffa6","blue":"#7373ff","red":"#ffb2a6","yellow":"#ffffb2","pink":"#ffbff2"}

        #self.ui.tabWidget.setTabText(0,"Connexion")
        #self.ui.tabWidget.setTabText(1,"Historical")
        #self.ui.tabWidget.setTabText(2,"Various")
        self.mutmodM = SetMutationModelMsat(self)
        self.mutmodS = SetMutationModelSequences(self)
        self.ui.tabWidget.addTab(self.mutmodM,"MM Microsats")
        self.ui.tabWidget.addTab(self.mutmodS,"MM Sequences")

        self.mutmodM.ui.frame_6.hide()
        self.mutmodM.ui.setMutMsLabel.setText("Default values for mutation model of Microsatellites")
        self.mutmodS.ui.frame_6.hide()
        self.mutmodS.ui.setMutSeqLabel.setText("Default values for mutation model of Sequences")

        self.ui.connectButton.hide()

        self.ui.maxLogLvlCombo.setCurrentIndex(2)

        self.styles = []
        for i in QStyleFactory.keys():
            self.styles.append(str(i))
            self.ui.styleCombo.addItem(str(i))

        ind = self.ui.styleCombo.findText("Cleanlooks")
        if ind != -1:
            self.ui.styleCombo.setCurrentIndex(ind)
        ind = self.ui.styleCombo.findText("white")
        if ind != -1:
            self.ui.bgColorCombo.setCurrentIndex(ind)

        # initialisation du combo pour le nombre max de thread
        try:
            nb_core = multiprocessing.cpu_count()
        except Exception,e:
            nb_core = 1
            log(3,"Impossible to count core number on this computer")
        for i in range(nb_core):
            self.ui.maxThreadCombo.addItem("%s"%(i+1))
        self.ui.maxThreadCombo.setCurrentIndex(self.ui.maxThreadCombo.count()-1)


    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        self.setWindowTitle("Settings")

        self.ui.gridLayout.setAlignment(Qt.AlignTop)
        self.ui.verticalLayout_57.setAlignment(Qt.AlignTop)
        self.ui.verticalLayout_2.setAlignment(Qt.AlignTop)
        #self.ui.verticalLayout_54.setAlignment(Qt.AlignTop)
        #self.ui.verticalLayout_29.setAlignment(Qt.AlignTop)
        #self.ui.verticalLayout_26.setAlignment(Qt.AlignTop)

        QObject.connect(self.ui.savePreferencesButton,SIGNAL("clicked()"),self.savePreferences)
        QObject.connect(self.ui.cancelPreferencesButton,SIGNAL("clicked()"),self.cancel)
        QObject.connect(self.ui.execBrowseButton,SIGNAL("clicked()"),self.browseExec)
        QObject.connect(self.ui.styleCombo,SIGNAL("currentIndexChanged(QString)"),self.changeStyle)
        QObject.connect(self.ui.maxLogLvlCombo,SIGNAL("currentIndexChanged(int)"),self.changeLogLevel)
        QObject.connect(self.ui.bgColorCombo,SIGNAL("currentIndexChanged(QString)"),self.changeBackgroundColor)

        QObject.connect(self.ui.serverCheck,SIGNAL("toggled(bool)"),self.toggleServer)
        QObject.connect(self.ui.useDefaultExeCheck,SIGNAL("toggled(bool)"),self.toggleExeSelection)
        QObject.connect(self.ui.wtCheck,SIGNAL("toggled(bool)"),self.toggleWtSelection)
        QObject.connect(self.ui.trayIconCheck,SIGNAL("toggled(bool)"),self.toggleTrayIconCheck)

        self.ui.addrEdit.setDisabled(True)
        self.ui.portEdit.setDisabled(True)

        self.toggleExeSelection(self.ui.useDefaultExeCheck.isChecked())
        self.toggleWtSelection(self.ui.wtCheck.isChecked())


        #QObject.connect(self.ui.saveMMMButton,SIGNAL("clicked()"),self.saveMMM)
        #QObject.connect(self.ui.saveMMSButton,SIGNAL("clicked()"),self.saveMMS)

    def cancel(self):
        """ pour annuler, on recharge depuis la derniere configuration sauvée
        """
        self.loadPreferences()
        self.close()

    def changeLogLevel(self,index):
        utilsPack.LOG_LEVEL = index + 1

    def toggleServer(self,state):
        self.ui.addrEdit.setDisabled(not state)
        self.ui.portEdit.setDisabled(not state)

    def toggleExeSelection(self,state):
        self.ui.execBrowseButton.setDisabled(state)
        self.ui.execPathEdit.setDisabled(state)

    def toggleWtSelection(self,state):
        self.parent.toggleWt(state)

    def toggleTrayIconCheck(self,state):
        self.parent.toggleTrayIcon(state)

    def browseExec(self):
        qfd = QFileDialog()
        path = str(qfd.getOpenFileName(self,"Where is your executable file ?"))
        self.ui.execPathEdit.setText(path)

    def getMaxThreadNumber(self):
        return int(self.ui.maxThreadCombo.currentText())

    def getExecutablePath(self):
        exPath = ""
        if self.ui.useDefaultExeCheck.isChecked():
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


    def savePreferences(self):
        """ sauve les préférences si elles sont valides
        """
        if self.allValid():
            self.saveMMM()
            self.saveMMS()
            self.saveConnexion()
            self.saveHM()
            self.saveVarious()
            self.saveRecent()
            self.writeConfigFile()
            self.close()

    #def writeConfigFile(self):
    #    with open(self.configFile, 'wb') as configfile:
    #        self.config.write(configfile)
    def writeConfigFile(self):
        self.config.write()

    def allValid(self):
        """ vérifie la validité des préférences
        """
        return (self.mutmodM.allValid() and self.mutmodS.allValid() and self.histModelValid() and self.connexionValid())

    def loadPreferences(self):
        """ charge les préférences de l'utilisateur si elles existent
        """
        if os.path.exists(os.path.expanduser("~/.diyabc/")):
            self.loadMMM()
            self.loadMMS()
            self.loadConnexion()
            self.loadHM()
            self.loadVarious()
            self.loadRecent()
        #else:
        #    # c'est sans doute la première fois qu'on lance diyabc
        #    # sous linux, on appelle gconf pour voir les icones dans les menus et boutons
        #    if "linux" in sys.platform:
        #        cmd_args_list = ["gconftool-2", "--type", "boolean", "--set", "/desktop/gnome/interface/buttons_have_icons", "true"]
        #        p = subprocess.Popen(cmd_args_list) 
        #        cmd_args_list = ["gconftool-2", "--type", "boolean", "--set", "/desktop/gnome/interface/menus_have_icons", "true"]
        #        p = subprocess.Popen(cmd_args_list) 

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

    def loadRecent(self):
            recent_list = []
            if self.config.has_key("recent"):
                for num,rec in self.config["recent"].items():
                    if len(rec) > 0 and rec.strip() != "":
                        recent_list.append(rec.strip())
                        log(3, "Loading recent : %s"%rec.strip())
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

    def saveVarious(self):
        """ sauvegarde de la partie various des préférences
        """
        style = str(self.ui.styleCombo.currentText())
        bgColor = str(self.ui.bgColorCombo.currentText())
        pic_format = str(self.ui.formatCombo.currentText())
        ex_path = str(self.ui.execPathEdit.text())
        pls = str(self.ui.particleLoopSizeEdit.text())
        show_tray = str(self.ui.trayIconCheck.isChecked())
        ex_default = str(self.ui.useDefaultExeCheck.isChecked())
        wt = str(self.ui.wtCheck.isChecked())
        max_thread = str(self.ui.maxThreadCombo.currentText())
        max_log_lvl = self.ui.maxLogLvlCombo.currentIndex()

        if not self.config.has_key("various"):
            self.config["various"] = {}
        self.config["various"]["style"] = style
        self.config["various"]["format"] = pic_format
        self.config["various"]["execPath"] = ex_path
        self.config["various"]["particleLoopSize"] = pls
        self.config["various"]["bgColor"] = bgColor
        self.config["various"]["showTrayIcon"] = show_tray
        self.config["various"]["useDefaultExecutable"] = ex_default
        self.config["various"]["activateWhatsThis"] = wt
        self.config["various"]["maxThreadNumber"] = max_thread
        self.config["various"]["maxLogLevel"] = max_log_lvl + 1

    def loadVarious(self):
        """ chargement de la partie various des préférences
        """
        cfg = self.config
        if cfg.has_key("various"):
            if cfg["various"].has_key("style"):
                style = cfg["various"]["style"]
                ind = self.ui.styleCombo.findText(style.strip())
                if ind != -1:
                    self.ui.styleCombo.setCurrentIndex(ind)
            if cfg["various"].has_key("format"):
                pformat = cfg["various"]["format"]
                ind = self.ui.formatCombo.findText(pformat.strip())
                if ind != -1:
                    self.ui.formatCombo.setCurrentIndex(ind)
            if cfg["various"].has_key("particleLoopSize"):
                pls = cfg["various"]["particleLoopSize"]
                self.ui.particleLoopSizeEdit.setText(pls.strip())
            if cfg["various"].has_key("execPath"):
                exf = cfg["various"]["execPath"]
                self.ui.execPathEdit.setText(exf.strip())
            if cfg["various"].has_key("bgColor"):
                bg = cfg["various"]["bgColor"]
                ind = self.ui.bgColorCombo.findText(bg.strip())
                if ind != -1:
                    self.ui.bgColorCombo.setCurrentIndex(ind)
            if cfg["various"].has_key("activateWhatsThis"):
                state = cfg["various"]["activateWhatsThis"]
                checked = (state == "True")
                self.ui.wtCheck.setChecked(checked)
                self.toggleWtSelection(checked)
            if cfg["various"].has_key("showTrayIcon"):
                state = cfg["various"]["showTrayIcon"]
                checked = (state == "True")
                self.ui.trayIconCheck.setChecked(checked)
                self.toggleTrayIconCheck(checked)
            if cfg["various"].has_key("useDefaultExecutable"):
                state = cfg["various"]["useDefaultExecutable"]
                checked = (state == "True")
                self.ui.useDefaultExeCheck.setChecked(checked)
                self.toggleExeSelection(checked)
            if cfg["various"].has_key("maxThreadNumber"):
                maxt = cfg["various"]["maxThreadNumber"]
                ind = self.ui.maxThreadCombo.findText(maxt.strip())
                if ind != -1:
                    self.ui.maxThreadCombo.setCurrentIndex(ind)
            if cfg["various"].has_key("maxLogLevel"):
                max_log_lvl = cfg["various"]["maxLogLevel"]
                self.ui.maxLogLvlCombo.setCurrentIndex(int(max_log_lvl) - 1)
            if cfg["various"].has_key("toolBarOrientation"):
                c = cfg["various"]["toolBarOrientation"].strip()
                if c == "1":
                    self.parent.setToolBarPosition(Qt.LeftToolBarArea)
                elif c == "2":
                    self.parent.setToolBarPosition(Qt.RightToolBarArea)
                elif c =="3":
                    self.parent.setToolBarPosition(Qt.TopToolBarArea)
                elif c == "4":
                    self.parent.setToolBarPosition(Qt.BottomToolBarArea)
        else:
            log(3,"No various conf found")

    def saveHM(self):
        """ sauvegarde de la partie historical model des préférences
        """
        if not self.config.has_key("hist_model_default_values"):
            self.config["hist_model_default_values"] = {}

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

        toSave = {}

        for key in ["nmin","nmax","nmean","nstdev","tmin","tmax","tmean","tstdev","amin","amax","amean","astdev"]:
            editname = key[0].upper() + key[1:]
            exec('toSave["%s"] = float(self.ui.%sEdit.text())'%(key,editname))
        toSave["nlaw"] = nlaw
        toSave["tlaw"] = tlaw
        toSave["alaw"] = alaw
        #toSave = {
        #        "nmin" : float(self.ui.NminEdit.text()),
        #        "nmax" : float(self.ui.NmaxEdit.text()),
        #        "nmean" : float(self.ui.NmeanEdit.text()),
        #        "nstdev" :float(self.ui.NstdevEdit.text()),
        #        "tmin" :float(self.ui.TminEdit.text()),
        #        "tmax" :float(self.ui.TmaxEdit.text()),
        #        "tmean" :float(self.ui.TmeanEdit.text()),
        #        "tstdev" :float(self.ui.TstdevEdit.text()),
        #        "amin" :float(self.ui.AminEdit.text()),
        #        "amax" :float(self.ui.AmaxEdit.text()),
        #        "amean" :float(self.ui.AmeanEdit.text()),
        #        "astdev" :float(self.ui.AstdevEdit.text()),
        #        "nlaw" : nlaw,
        #        "tlaw" : tlaw,
        #        "alaw" : alaw
        #        }
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
                    exec('self.ui.%sEdit.setText(dico_val["%s"])'%(editname,key))
                #self.ui.NminEdit.setText(dico_val["nmin"])
                #self.ui.NmaxEdit.setText(dico_val["nmax"])
                #self.ui.NmeanEdit.setText(dico_val["nmean"])
                #self.ui.NstdevEdit.setText(dico_val["nst"])
                #self.ui.TminEdit.setText(dico_val["tmin"])
                #self.ui.TmaxEdit.setText(dico_val["tmax"])
                #self.ui.TmeanEdit.setText(dico_val["tmean"])
                #self.ui.TstdevEdit.setText(dico_val["tst"])
                #self.ui.AminEdit.setText(dico_val["amin"])
                #self.ui.AmaxEdit.setText(dico_val["amax"])
                #self.ui.AmeanEdit.setText(dico_val["amean"])
                #self.ui.AstdevEdit.setText(dico_val["ast"])
                for pair in [("nlaw","UN"),("nlaw","LN"),("nlaw","NO"),("nlaw","LU"),
                        ("alaw","UN"),("alaw","LN"),("alaw","NO"),("alaw","LU"),
                        ("tlaw","UN"),("tlaw","LN"),("tlaw","NO"),("tlaw","LU")]:
                    exec('if dico_val["%s"] == "%s": self.ui.%sRadio.setChecked(True)'%(pair[0],pair[1],pair[0][0].upper()+pair[1]))
                #if dico_val["nlaw"] == "UN":
                #    self.ui.NUNRadio.setChecked(True)
                #if dico_val["nlaw"] == "LN":
                #    self.ui.NLNRadio.setChecked(True)
                #if dico_val["nlaw"] == "NO":
                #    self.ui.NNORadio.setChecked(True)
                #if dico_val["nlaw"] == "LU":
                #    self.ui.NLURadio.setChecked(True)
                #if dico_val["alaw"] == "UN":
                #    self.ui.AUNRadio.setChecked(True)
                #if dico_val["alaw"] == "LU":
                #    self.ui.ALURadio.setChecked(True)
                #if dico_val["alaw"] == "NO":
                #    self.ui.ANORadio.setChecked(True)
                #if dico_val["alaw"] == "LN":
                #    self.ui.ALNRadio.setChecked(True)
                #if dico_val["tlaw"] == "UN":
                #    self.ui.TUNRadio.setChecked(True)
                #if dico_val["tlaw"] == "LN":
                #    self.ui.TLNRadio.setChecked(True)
                #if dico_val["tlaw"] == "LU":
                #    self.ui.TLURadio.setChecked(True)
                #if dico_val["tlaw"] == "NO":
                #    self.ui.TNORadio.setChecked(True)
            except Exception,e:
                log(3,"Malformed hist_model_default_values section in configuration")
        else:
            log(3,"No hist conf found")
        
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
                    #exec('lines.append("{0} %s[%s,%s,%s,%s]"%(cfg.get("mutation_m_default_values","{0}_law"),cfg.get("mutation_m_default_values","{0}_zero"),cfg.get("mutation_m_default_values","{0}_one"),cfg.get("mutation_m_default_values","{0}_two"),cfg.get("mutation_m_default_values","{0}_three")))'.format(param))
                    exec('lines.append("{0} %s[%s,%s,%s,%s]"%(cfg["mutation_m_default_values"]["{0}_law"],cfg["mutation_m_default_values"]["{0}_zero"],cfg["mutation_m_default_values"]["{0}_one"],cfg["mutation_m_default_values"]["{0}_two"],cfg["mutation_m_default_values"]["{0}_three"]))'.format(param))
                self.mutmodM.setMutationConf(lines)
            except Exception,e:
                log(3,"Malformed mutation_m_default_values in configuration")
                
            #try:
            #    lines.append("MEANMU %s[%s,%s,%s,%s]"%(cfg.get("mutation_m_default_values","MEANMU_law"),cfg.get("mutation_m_default_values","MEANMU_zero"),cfg.get("mutation_m_default_values","MEANMU_one"),cfg.get("mutation_m_default_values","MEANMU_two"),cfg.get("mutation_m_default_values","MEANMU_three")))
            #    lines.append("GAMMU %s[%s,%s,%s,%s]"%(cfg.get("mutation_m_default_values","GAMMU_law"),cfg.get("mutation_m_default_values","GAMMU_zero"),cfg.get("mutation_m_default_values","GAMMU_one"),cfg.get("mutation_m_default_values","GAMMU_two"),cfg.get("mutation_m_default_values","GAMMU_three")))
            #    lines.append("MEANP %s[%s,%s,%s,%s]"%(cfg.get("mutation_m_default_values","MEANP_law"),cfg.get("mutation_m_default_values","MEANP_zero"),cfg.get("mutation_m_default_values","MEANP_one"),cfg.get("mutation_m_default_values","MEANP_two"),cfg.get("mutation_m_default_values","MEANP_three")))
            #    lines.append("GAMP %s[%s,%s,%s,%s]"%(cfg.get("mutation_m_default_values","GAMP_law"),cfg.get("mutation_m_default_values","GAMP_zero"),cfg.get("mutation_m_default_values","GAMP_one"),cfg.get("mutation_m_default_values","GAMP_two"),cfg.get("mutation_m_default_values","GAMP_three")))
            #    lines.append("MEANSNI %s[%s,%s,%s,%s]"%(cfg.get("mutation_m_default_values","MEANSNI_law"),cfg.get("mutation_m_default_values","MEANSNI_zero"),cfg.get("mutation_m_default_values","MEANSNI_one"),cfg.get("mutation_m_default_values","MEANSNI_two"),cfg.get("mutation_m_default_values","MEANSNI_three")))
            #    lines.append("GAMSNI %s[%s,%s,%s,%s]"%(cfg.get("mutation_m_default_values","GAMSNI_law"),cfg.get("mutation_m_default_values","GAMSNI_zero"),cfg.get("mutation_m_default_values","GAMSNI_one"),cfg.get("mutation_m_default_values","GAMSNI_two"),cfg.get("mutation_m_default_values","GAMSNI_three")))
            #    self.mutmodM.setMutationConf(lines)
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
                    #exec('lines.append("{0} %s[%s,%s,%s,%s]"%(cfg.get("mutation_s_default_values","{0}_law"),cfg.get("mutation_s_default_values","{0}_zero"),cfg.get("mutation_s_default_values","{0}_one"),cfg.get("mutation_s_default_values","{0}_two"),cfg.get("mutation_s_default_values","{0}_three")))'.format(param))
                    exec('lines.append("{0} %s[%s,%s,%s,%s]"%(cfg["mutation_s_default_values"]["{0}_law"],cfg["mutation_s_default_values"]["{0}_zero"],cfg["mutation_s_default_values"]["{0}_one"],cfg["mutation_s_default_values"]["{0}_two"],cfg["mutation_s_default_values"]["{0}_three"]))'.format(param))

                #lines.append("MEANMU %s[%s,%s,%s,%s]"%(cfg.get("mutation_s_default_values","MEANMU_law"),cfg.get("mutation_s_default_values","MEANMU_zero"),cfg.get("mutation_s_default_values","MEANMU_one"),cfg.get("mutation_s_default_values","MEANMU_two"),cfg.get("mutation_s_default_values","MEANMU_three")))
                #lines.append("GAMMU %s[%s,%s,%s,%s]"%(cfg.get("mutation_s_default_values","GAMMU_law"),cfg.get("mutation_s_default_values","GAMMU_zero"),cfg.get("mutation_s_default_values","GAMMU_one"),cfg.get("mutation_s_default_values","GAMMU_two"),cfg.get("mutation_s_default_values","GAMMU_three")))
                #lines.append("MEANK1 %s[%s,%s,%s,%s]"%(cfg.get("mutation_s_default_values","MEANK1_law"),cfg.get("mutation_s_default_values","MEANK1_zero"),cfg.get("mutation_s_default_values","MEANK1_one"),cfg.get("mutation_s_default_values","MEANK1_two"),cfg.get("mutation_s_default_values","MEANK1_three")))
                #lines.append("GAMK1 %s[%s,%s,%s,%s]"%(cfg.get("mutation_s_default_values","GAMK1_law"),cfg.get("mutation_s_default_values","GAMK1_zero"),cfg.get("mutation_s_default_values","GAMK1_one"),cfg.get("mutation_s_default_values","GAMK1_two"),cfg.get("mutation_s_default_values","GAMK1_three")))
                #lines.append("MEANK2 %s[%s,%s,%s,%s]"%(cfg.get("mutation_s_default_values","MEANK2_law"),cfg.get("mutation_s_default_values","MEANK2_zero"),cfg.get("mutation_s_default_values","MEANK2_one"),cfg.get("mutation_s_default_values","MEANK2_two"),cfg.get("mutation_s_default_values","MEANK2_three")))
                #lines.append("GAMK2 %s[%s,%s,%s,%s]"%(cfg.get("mutation_s_default_values","GAMK2_law"),cfg.get("mutation_s_default_values","GAMK2_zero"),cfg.get("mutation_s_default_values","GAMK2_one"),cfg.get("mutation_s_default_values","GAMK2_two"),cfg.get("mutation_s_default_values","GAMK2_three")))
                lines.append("MODEL %s %s %s"%(cfg["mutation_s_default_values"]["MODEL_model"],cfg["mutation_s_default_values"]["MODEL_inv_sites_pc"],cfg["mutation_s_default_values"]["MODEL_gamma_shape"]))
                self.mutmodS.setMutationConf(lines)
            except Exception,e:
                log(3,"Malformed mutation_s_default_values in configuration")
        else:
            log(3,"No MMS conf found")

    def saveConnexion(self):
        """ sauvegarde de la partie connexion des préférences
        """
        if not self.config.has_key("connexion"):
            self.config["connexion"] = {}

        self.config["connexion"]["host"] = str(self.ui.addrEdit.text())
        self.config["connexion"]["port"] = str(self.ui.portEdit.text())
        self.config["connexion"]["use_server"] = str(self.ui.serverCheck.isChecked())

    def loadConnexion(self):
        """ chargement de la partie connexion des préférences
        """
        cfg = self.config
        if cfg.has_key("connexion"):
                #print cfg["connexion"]
            try:
                self.ui.addrEdit.setText(cfg["connexion"]["host"])
                self.ui.portEdit.setText(cfg["connexion"]["port"])
                yesno = (cfg["connexion"]["use_server"] == 'True')
                self.ui.serverCheck.setChecked(yesno)
            except Exception,e:
                log(3,"Malformed connexion section in configuration\n%s"%e)
        else:
            log(3,"No connexion conf found")

    def connexionValid(self):
        """ vérifie la validité de la partie connexion
        """
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
        """ vérifie la validité de la partie hist model
        """
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

