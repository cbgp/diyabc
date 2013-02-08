# -*- coding: utf-8 -*-

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.set_condition_ui import Ui_MainWindow
import output,os,shutil,tarfile,sys
import variables
from variables import VERSION,VERSION_DATE,UIPATH

formBugReport,baseBugReport= uic.loadUiType("%s/bugReport.ui"%UIPATH)

class BugReport(formBugReport,baseBugReport):
    """ Fenetre de definition d'un rapport de bug
    """
    def __init__(self,parent=None):
        super(BugReport,self).__init__(parent)
        self.parent=parent
        self.createWidgets()

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)
        QObject.connect(self.ui.cancelButton,SIGNAL("clicked()"),self.close)
        QObject.connect(self.ui.validateButton,SIGNAL("clicked()"),self.valid)
        self.ui.infoLabel.setText(str(self.ui.infoLabel.text()).replace("CONF","%s"%self.parent.configFile))
        self.ui.cancelButton.setShortcut(QKeySequence(Qt.Key_Escape))

    def valid(self):
        """ clic sur le bouton de validation qui entraine l'affichage de la condition dans setHistoricalModel
        """
        problems = ""
        if len(self.parent.project_list) == 0:
            problems += "- You must have a currently opened project\n"
        if self.ui.lastThingNoRadio.isChecked():
            problems += "- You must produce the bug just before opening this bug report window\n"
        if str(self.ui.descEdit.toPlainText()) == "":
            problems += "- Please write at least a small description\n"
        if not self.ui.originInterfRadio.isChecked() and not self.ui.originCompRadio.isChecked():
            problems += "- You must select a possible origin\n"

        if problems == "":
            self.generateBugReport()
        else:
            output.notify(self,"Invalid bug report",problems)
            return

    def generateBugReport(self):
        current_project = self.parent.ui.tabWidget.currentWidget()
        fileDial = QFileDialog(self,"Select location of the bug report","%s"%os.path.dirname(current_project.dir))
        fileDial.setAcceptMode(QFileDialog.AcceptSave)
        fileDial.setLabelText(QFileDialog.Accept,"Save bug report")
        fileDial.setLabelText(QFileDialog.FileName,"Bug report name")
        ok = (fileDial.exec_() == 1)
        if not ok:
            return
        result = fileDial.selectedFiles()
        if len(result) > 0:
            path = result[0]
        tarname = "%s"%path
        if not tarname.endswith(".tar.gz"):
            tarname += ".tar.gz"

        repIntoTar = tarname.split("/")[-1].replace(".tar.gz","")
        
        # creation du tar
        dest = "%s/bug_report_tmp/"%current_project.dir
        if os.path.exists(dest):
            shutil.rmtree(dest)
        os.mkdir(dest)
        
        tar = tarfile.open(tarname,"w:gz")

        # copie du header
        if self.ui.headerYesRadio.isChecked():
            tar.add(os.path.expanduser("%s/header.txt"%current_project.dir),'%s/header.txt'%repIntoTar)

        # copie du datafile
        if self.ui.dataYesRadio.isChecked():
            tar.add(current_project.dataFileSource,'%s/datafile.txt'%repIntoTar)

        # copie du logfile
        tar.add(os.path.expanduser("%s"%self.parent.logfile),'%s/logfile.txt'%repIntoTar)

        # config file
        tar.add(os.path.expanduser("~/.diyabc/config.cfg"),'%s/config.cfg'%repIntoTar)

        # creation de la desc
        cause = ""
        if self.ui.originInterfRadio.isChecked():
            cause = "interface"
        else:
            cause = "computations"
        osname = sys.platform
        version_info = VERSION + " " + VERSION_DATE
        desc = str(self.ui.descEdit.toPlainText())
        descf = open("%sdescription.txt"%dest,'w')
        descf.write("Version : %s\nOS : %s\nCause : %s\n\n"%(version_info,osname,cause))
        descf.write(desc)
        descf.close()
        tar.add(os.path.expanduser("%sdescription.txt"%dest),'%s/description.txt'%repIntoTar)

        # les .out
        listf = os.listdir("%s"%current_project.dir)
        for fname in listf:
            if fname.endswith(".out"):
                tar.add(os.path.expanduser("%s/%s"%(current_project.dir,fname)),'%s/%s'%(repIntoTar,fname))

        tar.close()

        shutil.rmtree(dest)

        output.notify(self,"Thank you","Thank you for creating a bug report, please send it to the development team")

        self.close()
        
