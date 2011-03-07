# -*- coding: utf-8 -*-

from project import *

import time
import unittest
import sys
from diyabc import Diyabc
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui

class testDiyabc(unittest.TestCase):
    """
    A test class for DIYABC interface
    """

    def setUp(self):
        """
        set up data used in the tests.
        setUp is called before each test function execution.
        """
        self.app = QApplication(sys.argv)
        self.diyabc = Diyabc(self.app)
        self.diyabc.show()

        self.testDir = "/home/julien/vcs/git/diyabc/python_interface/datafiles/test"

    def testQuedale(self):
        self.assertEqual(self.diyabc.app, self.app)

    def testGeneral(self):
        QTest.mouseClick(self.diyabc.ui.skipButton,Qt.LeftButton)
        self.assertEqual(self.diyabc.ui.skipButton.isVisible(), False)
        # test sur l'ouverture d'un projet existant
        self.diyabc.openProject("/home/julien/vcs/git/diyabc/python_interface/datafiles/test/ploop_2011_1_18-3/")
        self.assertEqual(len(self.diyabc.project_list) == 1,True)
        # test sur l'ouverture d'un projet inexistant
        self.diyabc.openProject("/home/julien/vcs/git/diyabc/python_interface/datafiles/test/ploop_2011_1_18-3xx/")
        self.assertEqual(len(self.diyabc.project_list) == 1,True)
        # test sur la creation de projet
        self.diyabc.newProject("ppprojet")
        self.assertEqual(len(self.diyabc.project_list) == 2,True)
        project = self.diyabc.project_list[1]
        self.assertEqual(project.name == "ppprojet",True)
        self.assertEqual(project.ui.browseDataFileButton.isVisible(),True)
        project.dataFileSelection("/home/julien/vcs/git/diyabc/python_interface/datafiles/admix1.txt")
        self.assertEqual(project.ui.browseDataFileButton.isVisible() , False)
        project.dirSelection("%s/unittest"%self.testDir)
        self.assertEqual(project.hist_model_win.ui.addScButton.isVisible(),False)
        QTest.mouseClick(project.ui.setHistoricalButton,Qt.LeftButton)
        self.assertEqual(project.hist_model_win.ui.addScButton.isVisible(),True)
        project.hist_model_win.scList[0].findChild(QPlainTextEdit,"scplainTextEdit").setPlainText("N G F\n\
        0 sample 1\n\
        0 sample 2\n\
        0 sample 3 \n\
        t split 3 1 2 r33\n\
        t2 merge 1 2")
        QTest.mouseClick(project.hist_model_win.ui.defPrButton,Qt.LeftButton)
        QTest.mouseClick(project.hist_model_win.ui.okButton,Qt.LeftButton)
        self.assertEqual(project.hist_state_valid,True)


        self.diyabc.close()

    def fillHistModel(self,scTxtList,paramDicoValues,hist_model):
        # suppression des scenarios existants
        for i,scbox in enumerate(hist_model.scList):
            rmscbut = scbox.findChild(QPushButton,"rmScButton")
            QTest.mouseClick(rmscbut,Qt.LeftButton)
        # remplissage
        for txt in scTxtList:
            QTest.mouseClick(hist_model.ui.addScButton,Qt.LeftButton)
            hist_model.scList[-1].findChild(QPlainTextEdit,"scplainTextEdit").setPlainText(txt)
        QTest.mouseClick(project.hist_model_win.ui.defPrButton,Qt.LeftButton)
        # valeurs des paramètres
        for paramBox in hist_model.paramList:
            pname = str(paramBox.findChild(QLabel,"paramNameLabel"))
            values = paramDicoValues[pname]
            if values[0] == "UN":
                paramBox.findChild(QRadioButton,"uniformParamRadio").setChecked(True)
            elif values[0] == "LN":
                paramBox.findChild(QRadioButton,"logNormalRadio").setChecked(True)
            elif values[0] == "NO":
                paramBox.findChild(QRadioButton,"normalRadio").setChecked(True)
            elif values[0] == "LU":
                paramBox.findChild(QRadioButton,"logUniformRadio").setChecked(True)
            
            paramBox.findChild(QLineEdit,"minValueParamEdit").setText(values[1])
            paramBox.findChild(QLineEdit,"maxValueParamEdit").setText(values[2])
            paramBox.findChild(QLineEdit,"meanValueParamEdit").setText(values[3])
            paramBox.findChild(QLineEdit,"stValueParamEdit").setText(values[4])



if __name__ == '__main__':
    unittest.main()

