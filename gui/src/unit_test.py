# -*- coding: utf-8 -*-

import sys, codecs
import os.path
import os
sys.path.append((u"/".join(os.getcwd().split('/')[:-1])).encode(getFsEncoding(logLevel=False)))
from project import *

#import time
import unittest
import shutil
from diyabc import Diyabc
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtTest import QTest
from datetime import datetime
from utils.cbgpUtils import log,TeeLogger,getFsEncoding
import output
# pour eviter les popups de notification
output.debug = True

class testDiyabc(unittest.TestCase):
    """
    A test class for DIYABC interface
    """

    def setUp(self):
        """
        set up data used in the tests.
        setUp is called before each test function execution.
        """

        self.testDir = "/tmp/"
        self.testProjectDir = "/home/dehneg/TESTDIY/TOYTEST1/"
        self.testProjectDirHist = "/home/dehneg/TESTDIY/TOYTEST1_noref/"
        self.saveTestProjectDir = "/tmp/ploopsave"

    def initialize(self):
        app = QApplication(sys.argv)

        dd = datetime.now()
        logfile = os.path.expanduser("~/.diyabc/logs/test_%02d_%02d_%s-%02dh_%02dm-%s.log"%(dd.day,dd.month,dd.year,dd.hour,dd.minute,os.getpid()))
        diyabc = Diyabc(app,logfile=logfile)
        diyabc.show()
        # pour le dragNdrop des dossier projet
        diyabc.setAcceptDrops(True)
        # pour les logs dans un fichier et sur le terminal
        # chaque TeeLogger remplace une sortie et écrit dans
        # le fichier qu'on lui donne
        myOut = TeeLogger(logfile,True,diyabc.showLog)
        myErr = TeeLogger(logfile,False,diyabc.showLog)
        sys.stdout = myOut
        sys.stderr = myErr
        log(1,"\033[5;36m DIYABC launched \033[00m")
        return diyabc

    def testHistoricalModel(self):
        diyabc = self.initialize()

        if os.path.exists(self.saveTestProjectDir):
            shutil.rmtree(self.saveTestProjectDir)
        shutil.copytree(self.testProjectDirHist,self.saveTestProjectDir)

        project = self.tOpenExistingProject(diyabc,self.testProjectDirHist)

        QTest.mouseClick(project.setHistoricalButton,Qt.LeftButton)

        # pr tous les params, pour ttes les lois,pr les 4 champs, pr tous les types de valeur (fausse,min, max, random)
        # loop sur validate puis verif validité du histmodel
        valuesList = ["aa","0.0","0.01","5","-15","150","500","9999","9999.9"]
        radioNameList = ["uniformParamRadio","logUniformRadio","normalRadio","logNormalRadio"]
        valNameList = ["minValueParamEdit","maxValueParamEdit","meanValueParamEdit","stValueParamEdit"]
        for parambox in project.hist_model_win.paramList:
            parambox.findChild(QLineEdit,"minValueParamEdit").setText("0.1")
            parambox.findChild(QLineEdit,"maxValueParamEdit").setText("10.0")
            parambox.findChild(QLineEdit,"meanValueParamEdit").setText("5.1")
            parambox.findChild(QLineEdit,"stValueParamEdit").setText("0.1")
            parambox.findChild(QRadioButton,"uniformParamRadio").setChecked(True)
            for radioName in radioNameList:
                radioToClick = parambox.findChild(QRadioButton,radioName)
                QTest.mouseClick(radioToClick,Qt.LeftButton)
                for valName in valNameList:
                    fieldToEdit = parambox.findChild(QLineEdit,valName)
                    old_val = str(fieldToEdit.text())
                    for valToSet in valuesList:
                        fieldToEdit.setText(valToSet)
                        QTest.mouseClick(project.hist_model_win.okButton,Qt.LeftButton)
                        print "parameter %s, radiochecked %s, field %s, value %s"%(parambox.findChild(QLabel,"paramNameLabel").text(),radioName,valName,valToSet)
                        print "The project considers the historical model valid : %s"%project.hist_state_valid
                        # on vérifie que si le param est mal rempli, le model hist est dit invalide
                        self.assertEqual(self.checkParam(parambox),project.hist_state_valid)
                        QTest.mouseClick(project.setHistoricalButton,Qt.LeftButton)
                        fieldToEdit.setText(old_val)
            parambox.findChild(QLineEdit,"minValueParamEdit").setText("0.1")
            parambox.findChild(QLineEdit,"maxValueParamEdit").setText("10.0")
            parambox.findChild(QLineEdit,"meanValueParamEdit").setText("5.1")
            parambox.findChild(QLineEdit,"stValueParamEdit").setText("0.1")
            parambox.findChild(QRadioButton,"uniformParamRadio").setChecked(True)



        # restitution de la sauvegarde du projet
        shutil.rmtree(self.testProjectDirHist)
        shutil.copytree(self.saveTestProjectDir,self.testProjectDirHist)

        diyabc.close()

    def checkParam(self,pb):
        valNameList = ["minValueParamEdit","maxValueParamEdit","meanValueParamEdit","stValueParamEdit"]
        try:
            for i in valNameList:
                exec("%s = float(str(pb.findChild(QLineEdit,'%s').text()))"%(i,i))
            print "normal %s < %s < %s, unif: %s, logunif: %s"%(minValueParamEdit,meanValueParamEdit,maxValueParamEdit,pb.findChild(QRadioButton,"uniformParamRadio").isChecked(),pb.findChild(QRadioButton,"logUniformRadio").isChecked())

            if not (minValueParamEdit <= maxValueParamEdit):
                print "prob min <= max"
                return False
            if minValueParamEdit < 0 or maxValueParamEdit < 0:
                print "min or max < 0"
                return False
            if (pb.findChild(QRadioButton,"logNormalRadio").isChecked() or pb.findChild(QRadioButton,"normalRadio").isChecked()):
                if not (minValueParamEdit <= meanValueParamEdit <= maxValueParamEdit):
                    print "prob < <"
                    return False
                if not meanValueParamEdit > 0:
                    print "prob mean"
                    return False
                if not (stValueParamEdit > 0):
                    print "prob st"
                    return False
            return True
        except Exception as e:
            print("pas normal %s"%e)
            return False

    def testAllClicks(self):
        diyabc = self.initialize()

        # copie de sauvegarde du projet dans /tmp
        if os.path.exists(self.saveTestProjectDir):
            shutil.rmtree(self.saveTestProjectDir)
        shutil.copytree(self.testProjectDir,self.saveTestProjectDir)

        project = self.tOpenExistingProject(diyabc,self.testProjectDir)

        self.dicoPossibleClicks = {}

        self.dicoPossibleClicks["root"] = ["setHistoricalButton","setGeneticButton", "tabRefTable","tabAnalyses"]
        self.dicoPossibleClicks["setHistoricalButton"] = ["histExitButton","histClearButton","histOkButton","histAddScButton","histChkScButton","histDefPrButton"]
        #self.dicoPossibleClicks["setHistoricalButton"] = []
        self.dicoPossibleClicks["histExitButton"] = self.dicoPossibleClicks["root"]
        self.dicoPossibleClicks["histClearButton"] = self.dicoPossibleClicks["setHistoricalButton"]
        self.dicoPossibleClicks["histOkButton"] = self.dicoPossibleClicks["root"]
        self.dicoPossibleClicks["histAddScButton"] = self.dicoPossibleClicks["setHistoricalButton"]
        self.dicoPossibleClicks["histChkScButton"] = []
        self.dicoPossibleClicks["histDefPrButton"] = self.dicoPossibleClicks["setHistoricalButton"]
        self.dicoPossibleClicks["setGeneticButton"] = ["genClearButton","genOkButton","genExitButton","genAddGroupButton"]
        self.dicoPossibleClicks["tabRefTable"] = []
        self.dicoPossibleClicks["tabAnalyses"] = ["newAnButton"]
        self.dicoPossibleClicks["newAnButton"] = []
        self.dicoPossibleClicks["genClearButton"] = self.dicoPossibleClicks["setGeneticButton"]
        self.dicoPossibleClicks["genOkButton"] = self.dicoPossibleClicks["root"]
        self.dicoPossibleClicks["genExitButton"] = self.dicoPossibleClicks["root"]
        self.dicoPossibleClicks["genAddGroupButton"] = self.dicoPossibleClicks["setGeneticButton"]

        self.butCount = {}
        l = self.buildClickLists("root",0,2)
        print l

        nbproj = len(diyabc.project_list)
        diyabc.closeCurrentProject(False)
        self.assertEqual(len(diyabc.project_list) == nbproj-1,True)

        # on supprime les chaines dont la taille est trop faible
        to_remove = []
        for chain in l:
            if len(chain) < 3:
                to_remove.append(chain)
        for c in to_remove:
            l.remove(c)
        print "épurée : %s"%l

        # on a la liste des noms de boutons à cliquer
        # pour chaque combinaison possible de clics, on charge un projet et on effectue la suite de clic
        for chain in l:
            print "j'effectue la chaine : %s\n"%chain
            # on charge un projet
            project = self.tOpenExistingProject(diyabc,self.testProjectDir)
            # on associe les noms aux boutons de cette instance de projet
            self.dico_but_from_names = {"setHistoricalButton" : project.ui.setHistoricalButton,
                    "setGeneticButton" :project.ui.setGeneticButton,
                    "tabRefTable" : project.ui.tabRefTable,
                    "tabAnalyses" : project.ui.tabAnalyses,
                    "histExitButton" : project.hist_model_win.ui.exitButton,
                    "histClearButton" : project.hist_model_win.ui.clearButton,
                    "histOkButton" : project.hist_model_win.ui.okButton,
                    "histAddScButton" : project.hist_model_win.ui.addScButton,
                    "histChkScButton" : project.hist_model_win.ui.chkScButton,
                    "histDefPrButton" : project.hist_model_win.ui.defPrButton,
                    "genClearButton" : project.gen_data_win.ui.clearButton,
                    "genOkButton" : project.gen_data_win.ui.okButton,
                    "genExitButton" : project.gen_data_win.ui.exitButton,
                    "genAddGroupButton" : project.gen_data_win.ui.addGroupButton,
                    "newAnButton" : project.ui.newAnButton
                    }
            # on effectue la suite de clics
            for bname in chain:
                self.clickOn(self.dico_but_from_names[bname],project)

            print "j'ai fini la chaine : %s\n"%chain
            nbproj = len(diyabc.project_list)
            diyabc.closeCurrentProject(False)
            self.assertEqual(len(diyabc.project_list) == nbproj-1,True)

            # restitution de la sauvegarde du projet
            shutil.rmtree(self.testProjectDir)
            shutil.copytree(self.saveTestProjectDir,self.testProjectDir)

        diyabc.close()

    def clickOn(self,button,project):
        if button == self.dico_but_from_names["tabAnalyses"]:
            project.setCurrentIndex(1)
        elif button == self.dico_but_from_names["tabRefTable"]:
            project.setCurrentIndex(0)
        else:
            QTest.mouseClick(button,Qt.LeftButton)
        QCoreApplication.processEvents()


    def buildClickLists(self,but,level,nbMaxSameButton):
        """ construit récursivement la liste de clics possible à partir du bouton donné
        passe au maximum nbMaxSameButton fois sur le même bouton
        """
        if self.dicoPossibleClicks[but] == []:
            #print "[%s] fin sur %s\n"%(level,self.dico_names[but])
            return [[but]]
        else:
            if but in self.butCount.keys():
                self.butCount[but] += 1
                #print "[%s] %s eme fois sur %s"%(level,self.butCount[but],self.dico_names[but])
            else:
                #print "[%s] premiere fois sur %s"%(level,self.dico_names[but])
                self.butCount[but] = 1
            if self.butCount[but] <= nbMaxSameButton:
                lr = []
                for c in self.dicoPossibleClicks[but]:
                    #print "[%s] exploration de %s"%(level,self.dico_names[c])
                    lexp = list(self.buildClickLists(c,level+1,nbMaxSameButton))
                    for sublist in lexp:
                        #print "[%s] j'ajoute une liste (%s) a mon resultat\n"%(level,sublist)
                        new_l = []
                        if but != "root":
                            new_l.append(but)
                        new_l.extend(list(sublist))
                        lr.append(new_l)
                        #print "[%s] lr : %s"%(level,lr)
            else:
                #print "[%s] trop de fois sur %s"%(level,self.dico_names[but])
                lr = [[]]
            return lr

        #QTest.mouseClick(c,Qt.LeftButton)


    #def testGeneral(self):

    #    app = QApplication(sys.argv)

    #    dd = datetime.now()
    #    logfile = os.path.expanduser("~/.diyabc/logs/test_%02d_%02d_%s-%02dh_%02dm-%s.log"%(dd.day,dd.month,dd.year,dd.hour,dd.minute,os.getpid()))
    #    diyabc = Diyabc(app,logfile=logfile)
    #    diyabc.show()
    #    # pour le dragNdrop des dossier projet
    #    diyabc.setAcceptDrops(True)
    #    # pour les logs dans un fichier et sur le terminal
    #    # chaque TeeLogger remplace une sortie et écrit dans
    #    # le fichier qu'on lui donne
    #    myOut = output.TeeLogger(logfile,"a",diyabc,True)
    #    myErr = output.TeeLogger(logfile,"a",diyabc,False)
    #    sys.stdout = myOut
    #    sys.stderr = myErr
    #    log(1,"\033[5;36m DIYABC launched \033[00m")

    #    # test sur l'ouverture d'un projet existant
    #    self.tOpenExistingProject(diyabc,"/home/julien/vcs/git/diyabc.git/python_interface/datafiles/test/ploop_2011_1_18-3/")
    #    # test sur l'ouverture d'un projet inexistant
    #    self.tOpenNonExistingProject(diyabc,"/home/julien/vcs/git/diyabc.git/python_interface/datafiles/test/ploop_2011_1_18-3xx/")
    #    # tests sur la creation de projets ayant un nom non valide
    #    for c in ['_','-',"'",'"','.']:
    #        self.tNewIllegalProject(diyabc,"/home/julien/vcs/git/diyabc.git/python_interface/datafiles/admix1.txt","/home/julien/vcs/git/diyabc.git/python_interface/datafiles/test/unittest/ppor%sgerg"%c)


    #    project = self.tNewProject(diyabc,"/home/julien/vcs/git/diyabc.git/python_interface/datafiles/admix1.txt","/home/julien/vcs/git/diyabc.git/python_interface/datafiles/test/unittest/testnew")
    #    self.assertEqual(project.hist_model_win.ui.addScButton.isVisible(),False)
    #    QTest.mouseClick(project.ui.setHistoricalButton,Qt.LeftButton)
    #    self.assertEqual(project.hist_model_win.isVisible(),True)
    #    # remplissage des paramètres du modèle historique
    #    sctxtlist = []
    #    sc1txt = "N G F\n\
    #    0 sample 1\n\
    #    0 sample 2\n\
    #    0 sample 3 \n\
    #    t split 3 1 2 r33\n\
    #    t2 merge 1 2"
    #    sctxtlist.append(sc1txt)

    #    paramDicoValues = {
    #            "N":["UN",1,2,3,4],
    #            "G":["UN",1,2,3,4],
    #            "F":["UN",1,2,3,4],
    #            "t":["LU",1,2,3,4],
    #            "t2":["NO",1,2,3,4],
    #            "r33":["LN",1,2,3,4],
    #            }

    #    self.fillHistModel(sctxtlist,paramDicoValues,project.hist_model_win)
    #    #project.hist_model_win.addCondition("N>G")
    #    #project.hist_model_win.addCondition("N>F")

    #    QTest.mouseClick(project.hist_model_win.ui.defPrButton,Qt.LeftButton)
    #    QTest.mouseClick(project.hist_model_win.ui.okButton,Qt.LeftButton)
    #    self.assertEqual(project.hist_state_valid,True)
    #    self.assertEqual(len(project.hist_model_win.condList) == 2,False)

    #    self.assertEqual(project.gen_data_win.isVisible(),False)
    #    QTest.mouseClick(project.ui.setGeneticButton,Qt.LeftButton)
    #    self.assertEqual(project.gen_data_win.isVisible(),True)

    #    # test sur le clonage
    #    nbproj = len(diyabc.project_list)
    #    diyabc.cloneCurrentProject("cloneppproject","/home/julien/vcs/git/diyabc.git/python_interface/datafiles/test/unittest")
    #    self.assertEqual(len(diyabc.project_list) == nbproj+1,True)
    #    nbproj = len(diyabc.project_list)
    #    diyabc.cloneCurrentProject("clone2ppproject","/home/julien/vcs/git/diyabc.git/python_interface/datafiles/test/unittest/nonExistingDir/")
    #    self.assertEqual(len(diyabc.project_list) == nbproj,True)

    #    # suppression de projet
    #    current_project_dir = diyabc.ui.tabWidget.currentWidget().dir
    #    self.assertEqual(os.path.exists(current_project_dir),True)
    #    diyabc.deleteCurrentProject()
    #    self.assertEqual(os.path.exists(current_project_dir),False)

    #    diyabc.close()

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

    def tOpenExistingProject(self,diyabc,pdir):
        """ test sur l'ouverture d'un projet existant
        """
        nbproj = len(diyabc.project_list)
        diyabc.openProject(pdir)
        self.assertEqual(len(diyabc.project_list) == nbproj+1,True)
        return diyabc.project_list[-1]

    def tOpenNonExistingProject(self,diyabc,pdir):
        """ test sur l'ouverture d'un projet inexistant
        """
        nbproj = len(diyabc.project_list)
        diyabc.openProject(pdir)
        self.assertEqual(len(diyabc.project_list) == nbproj,True)

    def tNewIllegalProject(self,diyabc,datafile,projectdir):
        """ si on crée un projet avec un nouveau nom,
        on ne doit pas faire augmenter le nombre de projets
        """
        nbproj = len(diyabc.project_list)
        diyabc.newProject(projectdir)
        self.assertEqual(len(diyabc.project_list) , nbproj)

    def tNewProject(self,diyabc,datafile,projectdir):
        nbproj = len(diyabc.project_list)
        diyabc.newProject(projectdir)
        self.assertEqual(len(diyabc.project_list) == nbproj+1,True)
        project = diyabc.project_list[-1]
        self.assertEqual(project.name == projectdir.split('/')[-1],True)
        self.assertEqual(project.ui.browseDataFileButton.isVisible(),True)
        project.dataFileSelectionAndCopy(datafile)
        self.assertEqual(project.ui.browseDataFileButton.isVisible() , False)
        #project.dirSelection(projectdir)
        #self.assertEqual(project.ui.browseDirButton.isVisible() , False)
        return project


if __name__ == '__main__':
    unittest.main()

