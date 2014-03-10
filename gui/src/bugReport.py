# -*- coding: utf-8 -*-

import sys, codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import uic
#from uis.set_condition_ui import Ui_MainWindow
import output,os,shutil,tarfile,sys
import variables
from variables import VERSION,VERSION_DATE,UIPATH
import urllib, urllib2

formBugReport,baseBugReport= uic.loadUiType((u"%s/bugReport.ui"%UIPATH).encode(sys.getfilesystemencoding()))

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
        if u"%s"%self.ui.descEdit.toPlainText() == "":
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
        fsCoding = sys.getfilesystemencoding()
        fileDial = QFileDialog(self,"Select location of the bug report","%s"%os.path.dirname(str(current_project.dir)))
        fileDial.setAcceptMode(QFileDialog.AcceptSave)
        fileDial.setLabelText(QFileDialog.Accept,"Save bug report")
        fileDial.setLabelText(QFileDialog.FileName,"Bug report name")
        ok = (fileDial.exec_() == 1)
        if not ok:
            return
        result = fileDial.selectedFiles()
        if len(result) > 0:
            path = result[0]
        tarname = u"%s"%path
        if not tarname.endswith(u".tar.gz"):
            tarname += u".tar.gz"

        repIntoTar = tarname.split("/")[-1].replace(u".tar.gz","")

        # creation du tar
        dest = u"%s/bug_report_tmp/"%current_project.dir
        if os.path.exists(dest.encode(fsCoding)):
            try :
                shutil.rmtree(dest.encode(fsCoding))
            except :
                QMessageBox.information(self,'Fatal Error', "Unable to remove previous bug report temporary directory: \n\n %s\n\n Please go to this folder, delete it and try again to create a bug report." % dest)
        os.mkdir(dest.encode(fsCoding))

        tar = tarfile.open(tarname.encode(fsCoding),"w:gz")

        # copie du header
        if self.ui.headerYesRadio.isChecked():
            if os.path.exists((os.path.expanduser(u"%s/header.txt"%current_project.dir)).encode(fsCoding)):
                tar.add(os.path.expanduser((u"%s/header.txt"%current_project.dir).encode(fsCoding)),u'%s/header.txt'%repIntoTar)

        # copie du datafile
        if self.ui.dataYesRadio.isChecked():
            if os.path.exists(current_project.dataFileSource.encode(fsCoding)):
                dataFileName = os.path.basename(current_project.dataFileSource)
                tar.add(current_project.dataFileSource.encode(fsCoding),u'%s/%s'%(repIntoTar,dataFileName))

        # copie du diyabcproject file
        if os.path.exists((u"%s/%s.diyabcproject"%(current_project.dir,current_project.name)).encode(fsCoding)):
            dataFileName = os.path.basename(u"%s/%s.diyabcproject"%(current_project.dir,current_project.name))
            tar.add(current_project.dataFileSource.encode(fsCoding),u'%s/%s'%(repIntoTar,dataFileName))


        # copie des logfiles
        if os.path.exists(os.path.expanduser((u"%s"%self.parent.logfile)).encode(fsCoding)):
            tar.add(os.path.expanduser((u"%s"%self.parent.logfile).encode(fsCoding)),u'%s/logfile.txt'%repIntoTar)
        if os.path.exists((u"%s/command.txt"%(current_project.dir)).encode(fsCoding)):
            tar.add(os.path.expanduser((u"%s/command.txt"%(current_project.dir)).encode(fsCoding)),u'%s/command.txt'%(repIntoTar))

        # config file
        if os.path.exists(os.path.expanduser((u"~/.diyabc/config.cfg")).encode(fsCoding)):
            tar.add(os.path.expanduser((u"~/.diyabc/config.cfg").encode(fsCoding)),u'%s/config.cfg'%repIntoTar)

        # creation de la desc
        cause = ""
        if self.ui.originInterfRadio.isChecked():
            cause = "interface"
        else:
            cause = "computations"
        osname = sys.platform
        version_info = VERSION + " " + VERSION_DATE
        name = "%s"%self.ui.nameLabel.text()
        email = "%s"%self.ui.emailLabel.text()
        desc = "%s"%self.ui.descEdit.toPlainText()
        descf = open("%sdescription.txt"%dest,'w')
        infos= "Name : %s\nEmail : %s\nVersion : %s\nOS : %s\nCause : %s\n\n"%(name,email,version_info,osname,cause)
        descf.write(infos.encode('utf-8'))
        descf.write(desc.encode('utf-8'))
        descf.close()
        tar.add(os.path.expanduser((u"%sdescription.txt"%dest).encode(fsCoding)),u'%s/description.txt'%repIntoTar)

        # les .out
        listf = os.listdir((u"%s"%current_project.dir).encode(fsCoding))
        for fname in listf:
            if fname.startswith("conf."):
                tar.add(os.path.expanduser((u"%s/%s"%(current_project.dir,fname)).encode(fsCoding)),u'%s/%s'%(repIntoTar,fname))
            if fname.endswith(u".out"):
                tar.add(os.path.expanduser((u"%s/%s"%(current_project.dir,fname)).encode(fsCoding)),u'%s/%s'%(repIntoTar,fname))

        tar.close()

        shutil.rmtree(dest.encode(fsCoding))

        output.notify(self,"Thank you","Thank you for creating a bug report : %s, please send it to the development team : diyabc@supagro.inra.fr"%tarname)

        ## envoi par http :
        #ft = open(tarname,'r')
        #ftc = ft.read()
        #ft.close()

        #url = 'http://privas.dyndns.info/~julien/ff/write.php'
        #values = {"file":ftc}

        #data = urllib.urlencode(values)
        #req = urllib2.Request(url, data)
        #try:
        #    response = urllib2.urlopen(req)
        #except Exception as e:
        #    output.notify(self,"Thank you","Thank you for creating a bug report, please send it to the development team because the sending process went wrong\n\n%s"%str(e).decode("utf-8"))
        #    self.close()
        #    return

        #the_page = response.read()
        #if the_page.strip() == "OK":
        #    output.notify(self,"Thank you","Thank you for creating a bug report, it was successfully sent to our development team !")

        self.close()

