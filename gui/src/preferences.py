# -*- coding: utf-8 -*-

## @package python_interface.preferences
# @author Julien Veyssier
#
# @brief Fenêtre pour gérer les préférences personnelles

import os,sys,platform,multiprocessing
import os.path
import ConfigParser
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
from mutationModel.setMutationModelMsat import SetMutationModelMsat
from mutationModel.setMutationModelSequences import SetMutationModelSequences
import output
import utils.cbgpUtils as utilsPack
from utils.cbgpUtils import log
from utils.autoPreferences import AutoPreferences,visible,invisible
from variables import DATAPATH

## @class Preferences
# @brief Fenêtre pour gérer les préférences personnelles
class Preferences(AutoPreferences):
    """ Classe dérivée d'AutoPreferences qui va :
    - déclarer la structure des préférences en appelant la méthode 'digest' de la classe mère
    - redéfinir savePreferences et loadPreferences pour effectuer les sauvegardes/chargement 
      non prévus dans la classe mère (projets récents, modèle historique)
    - définir les actions à effectuer lors de changement de valeurs des préférences (style, showTrayIcon ...)
    - implémenter les fonctions d'accès aux valeurs des préférences (getMaxThreadNumber, getExecutablePath, getClusterExecutablepath ...) 
    """

    DEFAULT_FIRST_PART_SCRIPT_CLUSTER = """#!/bin/bash
numSimulatedDataSet=
numSimulatedDataSetByJob=
coresPerJob=
maxConcurrentJobs=
seed=
diyabcPath=
projectName=
dataFileName=
referenceTableHeaderName=
"""    
    
    DEFAULT_SECOND_PART_SCRIPT_CLUSTER = """
set -o nounset
set -o errexit

#====== functions ========

function progress(){
    sum=0
    jobsTot=$1
    # for each log file, if it exists, do the sum of done values
    for i in $(seq 1 $jobsTot); do
        if [ -e reftable_$i.log ]; then
            let sum=$sum+`head -n 2 reftable_$i.log | tail -n 1`
        fi
    done
    pct=$(( $sum * 100 / $numSimulatedDataSet ))
    echo `nbOk $jobsTot` "/ $jobsTot finished, $pct %, sum $sum/$numSimulatedDataSet"
}

function nbOk(){
    nb=0
    # for each log file, check if the computation is terminated
    for jobNum in $(seq 1 $1); do
        if [ -e reftable_$jobNum.log ]; then
            numDone=`head -n 2 reftable_$jobNum.log | tail -n 1`
	        if [ $jobNum -ne $numJobs ]
                    then 
                    if [ $numDone -ge $numSimulatedDataSetByJob ]
                        then
                        let nb=$nb+1
                    fi
                else
                    # case of last computation
                    if [ $numDone -ge $numSimulatedDataSetLastJob ]
                        then
                        let nb=$nb+1
                    fi
            fi
        fi
    done
    echo $nb
}

function testJobsAllReadylaunched(){
    # verify if the jobs are already submitted
    lockFilesList=(./*lock)
    reftableFilesList=(./*reftable*)
    if [ ${#lockFilesList[@]} -ne "0" ]  ||  [ ${#reftableFilesList[@]} -ne "0" ]
        then
        return 1
    else 
        return 0
    fi
}

function numRNGFiles(){
    # verify if the RNG's are allready generated
    RNGfilesList=(./*RNG_state*bin)
    return ${#RNGfilesList[@]} 
}

function testRNGallReadyGenerated(){
    # verify if the RNG's are allready generated
    RNGfilesList=(./*RNG_state*bin)
    if [ ${#RNGfilesList[@]} -ne "0" ]
        then
        return 1
    else 
        return 0
    fi
}

function generateRNGs(){
    # Generation of random numbers
    cmd="\"$diyabcPath\" -p ./ -n \"t:$coresPerJob;c:$maxConcurrentJobs;s:$seed\""
    echo "Generation of random numbers :" 
    echo $cmd
    "$diyabcPath" -p ./ -n "t:$coresPerJob;c:$maxConcurrentJobs;s:$seed"
}


function submitJobs(){
    # submit jobs to the cluster scheduller
    for jobNum in $(seq 1 $numJobs); do 

        numSimulatedDataSetForThisJob=$numSimulatedDataSetByJob
        if [ "$jobNum" -eq "$numJobs" ]
            then
            numSimulatedDataSetForThisJob=$numSimulatedDataSetLastJob
        fi
        
        ############################## EDIT #########################################
        ######## Modify this line to submit jobs to your cluster scheduler ###############
        # node.sh arguments
        ##1 DIYABCPATH
        ##2 NBTOGEN
        ##3 USERDIR
        ##4 MYNUMBER
        ##5 DATAFILE
        cmd="qsub -N n${jobNum}_$projectName -q long.q -cwd node.sh \"$diyabcPath\" $numSimulatedDataSetForThisJob \"`pwd`\" $jobNum  \"$PWD/$dataFileName\""
    ############################## END EDIT #####################################
        echo $cmd
        qsub -N n${jobNum}_$projectName -q long.q -cwd node.sh "$diyabcPath" $numSimulatedDataSetForThisJob "$PWD" $jobNum  "$PWD/$dataFileName"
        if [ $? -eq "0" ] 
            then 
            touch runningJobs.lock
        fi 

    done
}


#===== script core =======

# job parameters
numSimulatedDataSetLastJob=$(( $numSimulatedDataSet % $numSimulatedDataSetByJob ))
if [ "$numSimulatedDataSetLastJob" -eq "0" ] 
    then 
    numJobs=$(( $numSimulatedDataSet / $numSimulatedDataSetByJob ))
    numSimulatedDataSetLastJob=$numSimulatedDataSetByJob
else 
    numJobs=$(( $numSimulatedDataSet / $numSimulatedDataSetByJob + 1 ))
fi

if [ "$seed" -lt "0" ] 
    then
    seed=$RANDOM
fi


if [ "$maxConcurrentJobs" -gt "$numJobs" ]
    then
    maxConcurrentJobs=$numJobs
fi

projectName=${$projectName//[-._ àâäçéèêëîïôöûü]/}

# test if the jobs had allready been submitted
if [ `ls -1 | grep lock  | wc -l` -eq "0" ] 
    then 
    # test if the RNG had allready been generated
    if  [ `ls -1 | grep RNG_state  | wc -l` -eq "0" ] 
        then 
        generateRNGs
    else 
        echo -e "/!\/!\      Becarefull       /!\/!\ "
        echo -e "/!\/!\  `ls -1 | grep RNG_state  | wc -l` old RNG files found. Let's use them instead of generate $maxConcurrentJobs new ones /!\ /!\ " 
        echo -e "/!\/!\  be sure the RNG's have enough cores /!\ /!\ "                
    fi
    submitJobs
fi


while ! [ "`nbOk $numJobs`" -ge "$numJobs" ]; do
    echo `progress $numJobs`
    sleep 30
done
echo `progress $numJobs`



echo "Concatenating reftables :"
cmd="./general -p \"$PWD/\" -q  &> concat.out"
echo $cmd
./general -p "$PWD/" -q  &> concat.out

if [ $? -eq "0" ] 
    then 
    echo "*************************************************************"
    echo "All the result files should have been concatenated into reftable.bin"
    echo "See concat.out output file "
    echo "*************************************************************"
    rm runningJobs.lock
fi 

#END
exit 0
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

        dicoValTxtLogLvl = {
                 "1":"1 : Human actions",
                 "2":"2 : High level actions (file read, checks)",
                 "3":"3 : Small actions",
                 "4":"4 : Details"
        }

        formats = ["pdf","svg","jpg","png"]
        dicoValTxtFormat = {}
        for i in formats:
            dicoValTxtFormat[i] = i

        self.styles = []
        for i in QStyleFactory.keys():
            self.styles.append(str(i))
        default_style = None
        if "Cleanlooks" in self.styles:
            default_style = "Cleanlooks"
        
        dicoValTxtStyle = {}
        for i in self.styles:
            dicoValTxtStyle[i] = i

        colors = ["default","white"]
        colors.extend(self.tabColor.keys())
        dicoValTxtColor = {}
        for i in colors:
            dicoValTxtColor[i] = i

        default_font_family = str(self.parent.app.font().family())
        if "linux" in sys.platform:
            default_Psize = "10"
            default_font_family = "DejaVu Sans"
        elif "win" in sys.platform and "darwin" not in sys.platform:
            default_Psize = "8"
        elif "darwin" in sys.platform:
            default_Psize = "12"

        # hashtable contenant les informations des champs
        dico_fields = {
                "cluster" : [
                    {"proptype":"check",
                         "propname":"useCluster",
                         "labelText":"Use a cluster (don't check if you don't know what it is)",
                         "defaultState":False},
                    {"proptype":"lineEdit",
                         "propname":"numSimulatedDataSetByJob",
                         "labelText":"Number of records to generate for each job (granularity)",
                         "default_value":"50000"},
                    {"proptype":"lineEdit",
                         "propname":"coresPerJob",
                         "labelText":"number of cores used by each job",
                         "default_value":"1"},
                    {"proptype":"lineEdit",
                         "propname":"maxConcurrentJobs",
                         "labelText":"Maximum number of jobs running a the same time in the cluster (number of RNG's to generate)",
                         "default_value":"200"},
                    {"proptype":"lineEdit",
                         "propname":"seed",
                         "labelText":"seed to generate all RNG files (if set to negative then a random seed will be used)",
                         "default_value":"-1"},
                    {"proptype":"pathEdit",
                         "propname":"diyabcPath",
                         "labelText":"Path to the diyabc binary on the cluster or on your computer",
                         "default_value":""},
                    {"proptype":"textEdit",
                         "propname":"scriptMasterFirst",
                         "labelText":"NOT EDITABLE !\\nFirst part of the script\\n to run on your cluster master node\\n\\n(this first part will be merged\\nwith the second part in order\\nto obtain the launch.sh script)",
                         "default_value":"""\\n""".join(Preferences.DEFAULT_FIRST_PART_SCRIPT_CLUSTER.splitlines()),
                          "visibility":False,
                         "readOnly":True},
                    {"proptype":"textEdit",
                         "propname":"scriptMasterLast",
                         "labelText":"Second part of the script\\nto run on master node\\n\\nModify it in order to fit\\n your cluster scheduler\\n(ask your cluster administrator)",
                         "default_value":"""\\n""".join(Preferences.DEFAULT_SECOND_PART_SCRIPT_CLUSTER.splitlines()),
                         "visibility":True},
                ],
                "appearance" : [
                    {"proptype":"check",
                         "propname":"showTrayIcon",
                         "labelText":"Show tray icon",
                         "defaultState":False},
                    {"proptype":"combo",
                         "propname":"style",
                         "labelText":"Style",
                         "dicoValTxt":dicoValTxtStyle,
                         "l_ordered_val":self.styles,
                         "default_value":default_style},
                    {"proptype":"lineEdit",
                         "propname":"fontFamily",
                         "labelText":"Application font family\\n(needs restart)",
                         "default_value":default_font_family,
                         "visibility":invisible},
                    {"proptype":"lineEdit",
                         "propname":"fontSize",
                         "labelText":"Application point font size\\n(needs restart)",
                         "default_value":default_Psize,
                         "visibility":invisible},
                    {"proptype":"lineEdit",
                         "propname":"fontStrikeOut",
                         "labelText":"rien",
                         "default_value":"False",
                         "visibility":invisible},
                    {"proptype":"lineEdit",
                         "propname":"fontUnderline",
                         "labelText":"rien",
                         "default_value":"False",
                         "visibility":invisible},
                    {"proptype":"lineEdit",
                         "propname":"fontBold",
                         "labelText":"rien",
                         "default_value":"False",
                         "visibility":invisible},
                    {"proptype":"lineEdit",
                         "propname":"fontItalic",
                         "labelText":"rien",
                         "default_value":"False",
                         "visibility":invisible},
                    {"proptype":"combo",
                         "propname":"backgroundColor",
                         "labelText":"Window background color",
                         "dicoValTxt":dicoValTxtColor,
                         "l_ordered_val":colors,
                         "default_value":"default"}
                ],
                "various" : [
                    {"proptype":"check",
                         "propname":"activateWhatsThis",
                         "labelText":"Activate ""what's this"" help functionnality",
                         "defaultState":True},
                    {"proptype":"check",
                         "propname":"debugWhatsThis",
                         "labelText":"Show object name in what's this\\n(needs restart)",
                         "defaultState":False},
                    {"proptype":"check",
                         "propname":"useDefaultExecutable",
                         "labelText":"Use default executable check",
                         "defaultState":True},
                    {"proptype":"pathEdit",
                         "propname":"execPath",
                         "labelText":"Path to the executable file",
                         "default_value":""},
                    {"proptype":"lineEdit",
                         "propname":"particleLoopSize",
                         "labelText":"Particle loop size",
                         "default_value":"100"},
                    {"proptype":"combo",
                         "propname":"maxThread",
                         "labelText":"Maximum thread number",
                         "dicoValTxt":dicoValTxtMaxThread,
                         "l_ordered_val":[str(i) for i in range(1,
                         nb_core+1)],
                         "default_value":str(nb_core)},
                    {"proptype":"combo",
                         "propname":"maxLogLvl",
                         "labelText":"Maximum log level",
                         "dicoValTxt":dicoValTxtLogLvl,
                         "l_ordered_val":["1","2","3","4"],
                         "default_value":"3"},
                    {"proptype":"lineEdit",
                         "propname":"nbMaxRecent",
                         "labelText":"Maximum number of memorized recent projects",
                         "default_value":"20"},
                    {"proptype":"combo",
                         "propname":"picturesFormat",
                         "labelText":"Graphics and pictures save format \\n(scenario trees, PCA graphics)",
                         "dicoValTxt":dicoValTxtFormat,
                         "l_ordered_val":formats,
                         "default_value":"pdf"}
                ]
        }
        self.digest(dico_fields)

        QObject.connect(self.ui.styleCombo,SIGNAL("currentIndexChanged(QString)"),self.changeStyle)
        self.changeStyle(self.ui.styleCombo.currentText())
        QObject.connect(self.ui.maxLogLvlCombo,SIGNAL("currentIndexChanged(int)"),self.changeLogLevel)
        QObject.connect(self.ui.backgroundColorCombo,SIGNAL("currentIndexChanged(QString)"),self.changeBackgroundColor)
        self.changeLogLevel(self.ui.maxLogLvlCombo.currentIndex())

        QObject.connect(self.ui.useClusterCheck,SIGNAL("toggled(bool)"),self.toggleCluster)
        QObject.connect(self.ui.numSimulatedDataSetByJobEdit,SIGNAL("textChanged(QString)"),self.checkClusterValues)
        QObject.connect(self.ui.coresPerJobEdit,SIGNAL("textChanged(QString)"),self.checkClusterValues)
        QObject.connect(self.ui.maxConcurrentJobsEdit,SIGNAL("textChanged(QString)"),self.checkClusterValues)
        QObject.connect(self.ui.seedEdit,SIGNAL("textChanged(QString)"),self.checkClusterValues)
        
        QObject.connect(self.ui.useDefaultExecutableCheck,SIGNAL("toggled(bool)"),self.toggleExeSelection)
        QObject.connect(self.ui.activateWhatsThisCheck,SIGNAL("toggled(bool)"),self.toggleWtSelection)
        QObject.connect(self.ui.showTrayIconCheck,SIGNAL("toggled(bool)"),self.toggleTrayIconCheck)

        self.qfd = QFontDialog(self.parent.app.font())
        fontButton = QPushButton("Change font options (needs restart)")
        self.verticalLayoutScroll_appearance.addWidget(fontButton)
        QObject.connect(fontButton,SIGNAL("clicked()"),self.changeFontOptions)
        self.updateFont()

        self.toggleCluster(self.ui.useClusterCheck.isChecked())
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

    def checkClusterValues(self, Qstr):
        numSimulatedDatasetByJob = str(self.ui.numSimulatedDataSetByJobEdit.text()).strip()
        if numSimulatedDatasetByJob != '' :
            try :
                numSimulatedDatasetByJob = int(numSimulatedDatasetByJob)
            except ValueError:
                output.notify(self,"Value Error : numSimulatedDatasetByJob","Number of simulated data set by job must be a positive integer")
            
        coresPerJob =  str(self.ui.coresPerJobEdit.text()).strip()
        if coresPerJob != '' :
            try :
                coresPerJob = int(coresPerJob)
            except ValueError:
                output.notify(self,"Value Error : coresPerJob","Number of cores per job must be a positive integer")
        
        maxConcurrentJobs = str(self.ui.maxConcurrentJobsEdit.text()).strip()
        if maxConcurrentJobs != '' :
            try :
                maxConcurrentJobs= int(maxConcurrentJobs)
            except ValueError:
                output.notify(self,"Value Error : maxConcurrentJobs","Maximum number of concurrent job running on a cluster must be a positive integer")        
        
        seed = str(self.ui.seedEdit.text()).strip()
        if seed not in ['','None'] :
            try :
                seed = int(seed)
            except ValueError:
                output.notify(self,"Value Error : seed","seed must be a positive integer or 'None' or blank")   
        
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
            self.saveCluster()
            super(Preferences,self).savePreferences()
            self.saveCluster()            
            self.close()

    def loadPreferences(self):
        """ charge les préférences de l'utilisateur si elles existent
        """
        self.loadMMM()
        self.loadMMS()
        self.loadHM()
        self.loadRecent()
        self.loadToolBarPosition()
        self.loadCluster()
        super(Preferences,self).loadPreferences()
        
        self.updateFont()

    def changeLogLevel(self,index):
        utilsPack.LOG_LEVEL = index + 1

    def changeFontSize(self,size):
        font = self.parent.app.font()
        font.setPointSize(int(size))
        self.parent.app.setFont(font)

    def changeFontFamily(self,family):
        font = self.parent.app.font()
        font.setFamily(family)
        self.parent.app.setFont(font)

    def changeFontOptions(self):
        self.qfd.setCurrentFont(self.parent.app.font())
        i =self.qfd.exec_()
        if i != 0:
            font = self.qfd.currentFont()

            self.fontFamilyEdit.setText(str(font.family()))
            self.fontSizeEdit.setText(str(font.pointSize()))
            self.fontStrikeOutEdit.setText(str(font.strikeOut()))
            self.fontUnderlineEdit.setText(str(font.underline()))
            self.fontBoldEdit.setText(str(font.bold()))
            self.fontItalicEdit.setText(str(font.italic()))

            self.updateFont(font)

    def updateFont(self,font=None):
        if font != None:
            self.parent.app.setFont(font)
        else:
            fofo = QFont()
            fofo.setFamily(self.fontFamilyEdit.text())
            fofo.setPointSize(int(str(self.fontSizeEdit.text())))
            fofo.setStrikeOut(str(self.fontStrikeOutEdit.text()) == "True")
            fofo.setUnderline(str(self.fontUnderlineEdit.text()) == "True")
            fofo.setBold(str(self.fontBoldEdit.text()) == "True")
            fofo.setItalic(str(self.fontItalicEdit.text()) == "True")
            self.parent.app.setFont(fofo)


    def toggleCluster(self,state):
        self.ui.numSimulatedDataSetByJobEdit.setDisabled(not state)
        self.ui.coresPerJobEdit.setDisabled(not state)
        self.ui.maxConcurrentJobsEdit.setDisabled(not state)
        self.ui.seedEdit.setDisabled(not state)
        self.ui.scriptMasterFirstTextEdit.setDisabled(not state)
        self.ui.scriptMasterLastTextEdit.setDisabled(not state)

    def toggleExeSelection(self,state):
        self.ui.execPathBrowseButton.setDisabled(state)
        self.ui.execPathPathEdit.setDisabled(state)

    def toggleWtSelection(self,state):
        self.parent.toggleWt(state)
        self.debugWhatsThisCheck.setDisabled(not state)
        self.debugWhatsThisCheck.setChecked(False)

    def toggleTrayIconCheck(self,state):
        self.parent.systrayHandler.toggleTrayIcon(state)
    
    def getClusterExecutablePath(self):
        exClusterPath = ""
        if self.ui.useDefaultClusterExecutableCheck.isChecked():
            # LINUX
            if "linux" in sys.platform:
                if "86" in platform.machine() and "64" not in platform.machine():
                    exClusterpath = "%s/bin/diyabc-comput-linux-i386"%DATAPATH
                else:
                    exClusterpath = "%s/bin/diyabc-comput-linux-x64"%DATAPATH
                # si on a le paquet deb installé et qu'aucun binaire n'est dans le datapath
                if os.path.exists("/usr/bin/diyabc") and not os.path.exists("%s/bin/diyabc-comput-linux-x64"%DATAPATH)\
                        and not os.path.exists("%s/bin/diyabc-comput-linux-i386"%DATAPATH):
                    exClusterpath = "/usr/bin/diyabc"
            # WINDOWS
            elif "win" in sys.platform and "darwin" not in sys.platform:
                if os.environ.has_key("PROCESSOR_ARCHITECTURE") and "86" not in os.environ["PROCESSOR_ARCHITECTURE"]:
                    exClusterpath = ".\%s\\bin\diyabc-comput-win-x64"%DATAPATH.replace('/','\\')
                else:
                    exClusterpath = ".\%s\\bin\diyabc-comput-win-i386"%DATAPATH.replace('/','\\')
            # MACOS
            elif "darwin" in sys.platform:
                if "86" in platform.machine() and "64" not in platform.machine():
                    exClusterpath = "%s/bin/diyabc-comput-mac-i386"%DATAPATH
                else:
                    exClusterpath = "%s/bin/diyabc-comput-mac-x64"%DATAPATH
        else:
            if not os.path.exists(self.ui.execCluserPathPathEdit.text()):
                output.notify(self,"executable not found","The executable set in DIYABC settings cannot be found\n%s"%self.ui.diyabcPathPathEdit.text())
                return ""
            return str(self.ui.diyabcPathPathEdit.text())

        if not os.path.exists(exClusterpath):
            output.notify(self,"executable not found","The executable set in DIYABC settings cannot be found\n%s"%exClusterpath)
            return ""
        return exClusterpath   
    
    def getMaxThreadNumber(self):
        return int(self.ui.maxThreadCombo.currentText())

    def getExecutablePath(self):
        exPath = ""
        if self.ui.useDefaultExecutableCheck.isChecked():
            # LINUX
            if "linux" in sys.platform:
                if "86" in platform.machine() and "64" not in platform.machine():
                    exPath = "%s/bin/diyabc-comput-linux-i386"%DATAPATH
                else:
                    exPath = "%s/bin/diyabc-comput-linux-x64"%DATAPATH
                # si on a le paquet deb installé et qu'aucun binaire n'est dans le datapath
                if os.path.exists("/usr/bin/diyabc") and not os.path.exists("%s/bin/diyabc-comput-linux-x64"%DATAPATH)\
                        and not os.path.exists("%s/bin/diyabc-comput-linux-i386"%DATAPATH):
                    exPath = "/usr/bin/diyabc"
            # WINDOWS
            elif "win" in sys.platform and "darwin" not in sys.platform:
                if os.environ.has_key("PROCESSOR_ARCHITECTURE") and "86" not in os.environ["PROCESSOR_ARCHITECTURE"]:
                    exPath = ".\%s\\bin\diyabc-comput-win-x64"%DATAPATH.replace('/','\\')
                else:
                    exPath = ".\%s\\bin\diyabc-comput-win-i386"%DATAPATH.replace('/','\\')
            # MACOS
            elif "darwin" in sys.platform:
                if "86" in platform.machine() and "64" not in platform.machine():
                    exPath = "%s/bin/diyabc-comput-mac-i386"%DATAPATH
                else:
                    exPath = "%s/bin/diyabc-comput-mac-x64"%DATAPATH
        else:
            if not os.path.exists(self.ui.execPathPathEdit.text()):
                output.notify(self,"executable not found","The executable set in DIYABC settings cannot be found\n%s"%self.ui.execPathPathEdit.text())
                return ""
            return str(self.ui.execPathPathEdit.text())

        if not os.path.exists(exPath):
            output.notify(self,"executable not found","The executable set in DIYABC settings cannot be found\n%s"%exPath)
            return ""
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
        if self.config.has_key("recent") and self.config["recent"].has_key("projectPathList"):
            log(3, "Loading recent list")
            for rec in self.config["recent"]["projectPathList"].split('\n'):
                if len(rec) > 0 and rec.strip() != "":
                    recent_list.append(rec.strip())
                    log(4, "Loading recent : %s"%rec.strip())
            self.parent.setRecent(recent_list)

    def loadCluster(self):
        if self.config.has_key("cluster") and self.config["cluster"].has_key("scriptMasterFirst"):
            del self.config["cluster"]["scriptMasterFirst"]
        
    def getMaxRecentNumber(self):
        return int(str(self.ui.nbMaxRecentEdit.text()))

    def saveRecent(self):
        log(3,"Saving recent list")
        if not self.config.has_key("recent"):
            self.config["recent"] = {}
        recList = self.parent.getRecent()
        cfgRecentIndex = 0
        rec_cfg = ""
        for rec in recList:
            log(4,"Saving into recent list : %s"%rec)
            rec_cfg += "{0}\n".format(rec)
        # delete last '\n'
        rec_cfg = rec_cfg[:-1]
        self.config["recent"]["projectPathList"] = rec_cfg
    
    def saveCluster(self):
        if self.config.has_key("cluster") and self.config["cluster"].has_key("scriptMasterFirst"):
            del self.config["cluster"]["scriptMasterFirst"]
        
        
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
                for f in ["law","zero","one","two","three"]:
                    exec("self.config['mutation_m_default_values'][ti+'_{0}'] = {0}".format(f))

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
                    model = ll.split(' ')[1]
                    inv_sites_pc = ll.split(' ')[2]
                    gamma_shape = ll.split(' ')[3]
                    for f in ["model","inv_sites_pc","gamma_shape"]:
                        exec("self.config['mutation_s_default_values'][ti+'_{0}'] = {0}".format(f))
                else:
                    law = ll.split(' ')[1].split('[')[0]
                    zero = ll.split('[')[1].split(',')[0]
                    one = ll.split(',')[1]
                    two = ll.split(',')[2]
                    three = ll.split(',')[3].split(']')[0]
                    for f in ["law","zero","one","two","three"]:
                        exec("self.config['mutation_s_default_values'][ti+'_{0}'] = {0}".format(f))

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

        for type_letter in ['n','t','a']:
            for law_name in ["UN","LU","NO","LN"]:
                exec("if self.hist_model.{0}{1}Radio.isChecked(): {2}law = '{1}'".format(\
                    type_letter.upper(),law_name,type_letter))

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
