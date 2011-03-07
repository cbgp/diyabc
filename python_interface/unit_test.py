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
        self.tOpenExistingProject("/home/julien/vcs/git/diyabc/python_interface/datafiles/test/ploop_2011_1_18-3/")
        # test sur l'ouverture d'un projet inexistant
        self.tOpenNonExistingProject("/home/julien/vcs/git/diyabc/python_interface/datafiles/test/ploop_2011_1_18-3xx/")
        # tests sur la creation de projets ayant un nom non valide
        for c in ['_','-',"'",'"','.','/']:
            self.tNewIllegalProject("pppro%sject"%c,"/home/julien/vcs/git/diyabc/python_interface/datafiles/admix1.txt","/home/julien/vcs/git/diyabc/python_interface/datafiles/test/unittest/")


        project = self.tNewProject("ppproject","/home/julien/vcs/git/diyabc/python_interface/datafiles/admix1.txt","/home/julien/vcs/git/diyabc/python_interface/datafiles/test/unittest/")
        self.assertEqual(project.hist_model_win.ui.addScButton.isVisible(),False)
        QTest.mouseClick(project.ui.setHistoricalButton,Qt.LeftButton)
        self.assertEqual(project.hist_model_win.isVisible(),True)
        # remplissage des paramètres du modèle historique
        sctxtlist = []
        sc1txt = "N G F\n\
        0 sample 1\n\
        0 sample 2\n\
        0 sample 3 \n\
        t split 3 1 2 r33\n\
        t2 merge 1 2"
        sctxtlist.append(sc1txt)

        paramDicoValues = {
                "N":["UN",1,2,3,4],
                "G":["UN",1,2,3,4],
                "F":["UN",1,2,3,4],
                "t":["LU",1,2,3,4],
                "t2":["NO",1,2,3,4],
                "r33":["LN",1,2,3,4],
                }

        self.fillHistModel(sctxtlist,paramDicoValues,project.hist_model_win)
        #project.hist_model_win.addCondition("N>G")
        #project.hist_model_win.addCondition("N>F")

        QTest.mouseClick(project.hist_model_win.ui.defPrButton,Qt.LeftButton)
        QTest.mouseClick(project.hist_model_win.ui.okButton,Qt.LeftButton)
        self.assertEqual(project.hist_state_valid,True)
        self.assertEqual(len(project.hist_model_win.condList) == 2,False)

        self.assertEqual(project.gen_data_win.isVisible(),False)
        QTest.mouseClick(project.ui.setGeneticButton,Qt.LeftButton)
        self.assertEqual(project.gen_data_win.isVisible(),True)

        # test sur le clonage
        nbproj = len(self.diyabc.project_list)
        self.diyabc.cloneCurrentProject("cloneppproject","/home/julien/vcs/git/diyabc/python_interface/datafiles/test/unittest/")
        self.assertEqual(len(self.diyabc.project_list) == nbproj+1,True)
        nbproj = len(self.diyabc.project_list)
        self.diyabc.cloneCurrentProject("clone2ppproject","/home/julien/git/diyabc/python_interface/datafiles/test/unittest/nonExistingDir/")
        self.assertEqual(len(self.diyabc.project_list) == nbproj,True)

        # suppression de projet
        current_project_dir = self.diyabc.ui.tabWidget.currentWidget().dir
        self.assertEqual(os.path.exists(current_project_dir),True)
        self.diyabc.deleteCurrentProject()
        self.assertEqual(os.path.exists(current_project_dir),False)

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
        QTest.mouseClick(hist_model.ui.defPrButton,Qt.LeftButton)
        # valeurs des paramètres
        for paramBox in hist_model.paramList:
            pname = str(paramBox.findChild(QLabel,"paramNameLabel").text())
            values = paramDicoValues[pname]
            if values[0] == "UN":
                paramBox.findChild(QRadioButton,"uniformParamRadio").setChecked(True)
            elif values[0] == "LN":
                paramBox.findChild(QRadioButton,"logNormalRadio").setChecked(True)
            elif values[0] == "NO":
                paramBox.findChild(QRadioButton,"normalRadio").setChecked(True)
            elif values[0] == "LU":
                paramBox.findChild(QRadioButton,"logUniformRadio").setChecked(True)
            
            paramBox.findChild(QLineEdit,"minValueParamEdit").setText(str(values[1]))
            paramBox.findChild(QLineEdit,"maxValueParamEdit").setText(str(values[2]))
            paramBox.findChild(QLineEdit,"meanValueParamEdit").setText(str(values[3]))
            paramBox.findChild(QLineEdit,"stValueParamEdit").setText(str(values[4]))

    def tOpenExistingProject(self,pdir):
        """ test sur l'ouverture d'un projet existant
        """
        nbproj = len(self.diyabc.project_list)
        self.diyabc.openProject(pdir)
        self.assertEqual(len(self.diyabc.project_list) == nbproj+1,True)

    def tOpenNonExistingProject(self,pdir):
        """ test sur l'ouverture d'un projet inexistant
        """
        nbproj = len(self.diyabc.project_list)
        self.diyabc.openProject(pdir)
        self.assertEqual(len(self.diyabc.project_list) == nbproj,True)

    def tNewIllegalProject(self,name,datafile,projectdir):
        """ si on crée un projet avec un nouveau nom, 
        on ne doit pas faire augmenter le nombre de projets
        """
        nbproj = len(self.diyabc.project_list)        
        self.diyabc.newProject(name)
        self.assertEqual(len(self.diyabc.project_list) == nbproj,True)

    def tNewProject(self,name,datafile,projectdir):
        nbproj = len(self.diyabc.project_list)        
        self.diyabc.newProject(name)
        self.assertEqual(len(self.diyabc.project_list) == nbproj+1,True)
        project = self.diyabc.project_list[-1]
        self.assertEqual(project.name == name,True)
        self.assertEqual(project.ui.browseDataFileButton.isVisible(),True)
        project.dataFileSelection(datafile)
        self.assertEqual(project.ui.browseDataFileButton.isVisible() , False)
        project.dirSelection(projectdir)
        self.assertEqual(project.ui.browseDirButton.isVisible() , False)
        return project


if __name__ == '__main__':
    unittest.main()

