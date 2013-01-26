# -*- coding: utf-8 -*-

import os
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.preferences_ui import Ui_MainWindow
from utils.cbgpUtils import log
import variables
from variables import UIPATH

formLogFile,baseLogFile = uic.loadUiType("%s/showLogFile.ui"%UIPATH)

class ShowLogFile(formLogFile,baseLogFile):
    """ Classe pour consulter les logs dans l'interface
    """
    def __init__(self,parent=None):
        super(ShowLogFile,self).__init__(parent)
        self.parent = parent
        #dd = datetime.now()
        #self.logfile = os.path.expanduser("~/.diyabc/logs/%02d_%02d_%s-%s.log"%(dd.day,dd.month,dd.year,os.getpid()))
        #self.logfile = self.parent.logfile
        self.createWidgets()
        self.updateLogFile()

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        self.ui.logText.setCenterOnScroll(True)
        self.ui.pathEdit.setText(self.parent.logfile)

        QObject.connect(self.ui.updateButton,SIGNAL("clicked()"),self.updateLogFile)
        QObject.connect(self.ui.closeButton,SIGNAL("clicked()"),self.close)
        QObject.connect(self.ui.saveButton,SIGNAL("clicked()"),self.saveFile)

    def saveFile(self):
        fname = str(QFileDialog.getSaveFileName(self,"Save log file","","(logfile) *.log"))
        if fname != "":
            if not fname.endswith(".log"):
                fname = "%s.log"%fname
            sf=open(fname,'w')

            lf=open(self.parent.logfile,'r')
            text=lf.read()
            lf.close()

            sf.write(text)
            sf.close()
            log(1,"Saving logfile into %s"%fname)


    def updateLogFile(self):
        if os.path.exists(self.parent.logfile):
            f=open(self.parent.logfile,'r')
            text=f.read()
            f.close()
            self.ui.logText.setPlainText(text)
            self.ui.logText.setLineWrapMode(0)



