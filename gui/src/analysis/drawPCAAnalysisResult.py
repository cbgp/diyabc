# -*- coding: utf-8 -*-

import os,sys
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtSvg import *
from PyQt4 import uic
from utils.visualizescenario import *
from viewTextFile import ViewTextFile
import output
from utils.matplotlibCanvas import *
from utils.cbgpUtils import readlinesWindows
import variables
from variables import UIPATH

formDrawPCAAnalysisResult,baseDrawPCAAnalysisResult = uic.loadUiType("%s/drawScenarioFrame.ui"%UIPATH)

class DrawPCAAnalysisResult(formDrawPCAAnalysisResult,baseDrawPCAAnalysisResult):
    """ Classe pour créer une frame à l'intérieur de laquelle on dessine les resultats d'une analyse
    Pre-evaluate PCA ou model checking
    """
    def __init__(self,analysis,directory,parent=None):
        super(DrawPCAAnalysisResult,self).__init__(parent)
        self.parent=parent
        self.directory=directory
        self.dico_points = {}
        self.analysis = analysis
        if self.directory.split('_')[-1] == "modelChecking":
            self.acpFile = "%s/analysis/%s/mcACP.txt"%(self.parent.dir,self.directory)
        else:
            self.acpFile = "%s/analysis/%s/ACP.txt"%(self.parent.dir,self.directory)
        self.createWidgets()
        self.dicoPlot = {}  
        self.tab_colors = ["#0000FF","#00FF00","#FF0000","#00FFFF","#FF00FF","#FFFF00","#000000","#808080","#008080","#800080","#808000","#000080","#008000","#800000","#A4A0A0","#A0A4A0","#A0A0A4","#A00000","#00A000","#00A0A0"]
        
    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        QObject.connect(self.ui.closeButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.printButton,SIGNAL("clicked()"),self.printGraph)
        QObject.connect(self.ui.viewLocateButton,SIGNAL("clicked()"),self.viewLocate)
        self.ui.savePicturesButton.hide()
        #self.ui.printButton.hide()

        QObject.connect(self.ui.scCombo,SIGNAL("currentIndexChanged(int)"),self.drawGraph)
        QObject.connect(self.ui.compoHCombo,SIGNAL("currentIndexChanged(int)"),self.drawGraph)
        QObject.connect(self.ui.compoVCombo,SIGNAL("currentIndexChanged(int)"),self.drawGraph)
        QObject.connect(self.ui.nbpCombo,SIGNAL("currentIndexChanged(int)"),self.drawGraph)
        self.ui.ACProgress.setValue(0)
        self.tab_colors = ["#0000FF","#00FF00","#FF0000","#00FFFF","#FF00FF","#FFFF00","#000000","#808080","#008080","#800080","#808000","#000080","#008000","#800000","#A4A0A0","#A0A4A0","#A0A0A4","#A00000","#00A000","#00A0A0"]

        self.ui.scrollArea.hide()
        if not os.path.exists("%s/analysis/%s/mclocate.txt"%(self.parent.dir,self.directory))\
                and not os.path.exists("%s/analysis/%s/locate.txt"%(self.parent.dir,self.directory)):
            self.ui.viewLocateButton.hide()

        self.ui.analysisNameLabel.setText("Analysis : %s"%self.analysis.name)

        #self.ui.PCAGraphFrame.setMinimumSize(QSize(860, 430))
        #self.ui.PCAGraphFrame.setMaximumSize(QSize(860, 430))

        self.ui.parameterChoiceFrame.hide()

    def viewLocate(self):
        """ clic sur le bouton view locate
        """
        if os.path.exists("%s/analysis/%s/mclocate.txt"%(self.parent.dir,self.directory)):
            f = open("%s/analysis/%s/mclocate.txt"%(self.parent.dir,self.directory),'r')
        elif os.path.exists("%s/analysis/%s/locate.txt"%(self.parent.dir,self.directory)):
            f = open("%s/analysis/%s/locate.txt"%(self.parent.dir,self.directory),'r')
        data = f.read()
        f.close()
        self.parent.drawAnalysisFrame = ViewTextFile(data,self.returnToMe,self.parent)
        self.parent.drawAnalysisFrame.choiceFrame.hide()
        self.parent.ui.analysisStack.addWidget(self.parent.drawAnalysisFrame)
        self.parent.ui.analysisStack.setCurrentWidget(self.parent.drawAnalysisFrame)

    def returnToMe(self):
        self.parent.returnTo(self)

    def loadACP(self):
        """ charge le fichier ACP dans un dico
        et initialise les combo boxes
        """
        QApplication.setOverrideCursor( Qt.WaitCursor )
        self.ui.ACProgress.setValue(0)

        if not os.path.exists(self.acpFile):
            output.notify(self,"error","%s not found"%self.acpFile)
        else:
            if "win" in sys.platform and "darwin" not in sys.platform:
                lines = readlinesWindows(self.acpFile)
            else:
                f = codecs.open(self.acpFile,"r","utf-8")
                lines = f.readlines()
                f.close()
            self.dico_points = {}
            self.dico_points_posterior = {}

            nb_composantes = (len(lines[0].split())-2)
            nb_prior = int(lines[0].split()[0])
            nb_lignes = len(lines)
            self.dico_points[-1] = lines[0].split()[2:]
            self.dico_points[0] = lines[1].split()[1:]
            for i in range(len(self.dico_points[0])):
                self.dico_points[0][i] = float(self.dico_points[0][i])

            # pour chaque ligne
            i=2
            while i < nb_prior+2 and i < nb_lignes:
                l=lines[i]
                pc = (float(i)/float(nb_lignes)*100)+1
                if (int(pc) % 2) == 0:
                    self.ui.ACProgress.setValue(pc)
                    QCoreApplication.processEvents()
                tab = l.split()
                num_sc = int(tab[0])
                # si le sc n'est pas encore dans le dico
                if num_sc not in self.dico_points.keys():
                    self.dico_points[num_sc] = []
                    # on y ajoute ce qui va etre la liste des coords pour chaque composante
                    for j in range(nb_composantes):
                        self.dico_points[num_sc].append([])

                # on ajoute chaque coordonnée dans la composante correspondante
                c = 1
                while c < len(tab):
                    self.dico_points[num_sc][c-1].append( float(tab[c]) )
                    c+=1
                i+=1

            while i < nb_lignes:
                l=lines[i]
                pc = (float(i)/float(nb_lignes)*100)+1
                if (int(pc) % 2) == 0:
                    self.ui.ACProgress.setValue(pc)
                    QCoreApplication.processEvents()
                tab = l.split()
                num_sc = int(tab[0])
                # si le sc n'est pas encore dans le dico
                if num_sc not in self.dico_points_posterior.keys():
                    self.dico_points_posterior[num_sc] = []
                    # on y ajoute ce qui va etre la liste des coords pour chaque composante
                    for j in range(nb_composantes):
                        self.dico_points_posterior[num_sc].append([])

                # on ajoute chaque coordonnée dans la composante correspondante
                c = 1
                while c < len(tab):
                    self.dico_points_posterior[num_sc][c-1].append( float(tab[c]) )
                    c+=1
                i+=1

            # initialisation des combo
            self.ui.compoHCombo.clear()
            self.ui.compoVCombo.clear()
            for i in range(nb_composantes):
                self.ui.compoHCombo.addItem("%s"%(i+1))
                self.ui.compoVCombo.addItem("%s"%(i+1))
            if nb_composantes > 1:
                self.ui.compoHCombo.setCurrentIndex(0)
                self.ui.compoVCombo.setCurrentIndex(1)
            self.ui.scCombo.clear()
            self.ui.scCombo.addItem("all")
            for sc in self.dico_points.keys():
                if sc != 0 and sc != -1:
                    self.ui.scCombo.addItem("%s"%sc)
            if self.analysis.category == "modelChecking":
                ind = self.ui.scCombo.findText(str(self.analysis.candidateScList[0]))
                if ind != -1:
                    self.ui.scCombo.setCurrentIndex(ind)
                    self.ui.scCombo.setDisabled(True)
        QApplication.restoreOverrideCursor()

    def drawGraphToPlot(self,plot,num_sc,compo_h,compo_v,nbp):
        """ dessine les points pour un scenario, deux components, sur plot et met à jour legend
        le tout limité à nbp points. retourne le curve du posterior ou None s'il n'y en a pas
        """
        col = self.tab_colors[(num_sc%20)]

        legend_txt = "Scenario %s prior"%num_sc
        mypoints = plot.axes.scatter(self.dico_points[num_sc][compo_h][:nbp], self.dico_points[num_sc][compo_v][:nbp],marker='o',label=legend_txt,c=col,s=20.0)
        # si on a des coordonnées pour le posterior, on les dessine
        # avec la meme couleur mais plus gros
        if num_sc in self.dico_points_posterior.keys():
            legend_txt = "Scenario %s posterior"%num_sc
            mypoints = plot.axes.scatter(self.dico_points_posterior[num_sc][compo_h][:nbp], self.dico_points_posterior[num_sc][compo_v][:nbp],marker='o',label=legend_txt,c=col,s=75)
            return mypoints
        return None

    def drawObservedToPlot(self,plot,compo_h,compo_v):
        """ dessine le point observé sur plot pour les deux components donnés
        """
        col="yellow"
        legend_txt = "Observed data set"
        mypoints = plot.axes.scatter([self.dico_points[0][compo_h]],[self.dico_points[0][compo_v]],marker='o',label=legend_txt,c=col,s=150)
        return mypoints

    def drawGraph(self):
        """ dessine le graphe entier en fonction des valeurs selectionnées
        """
        # on attend que les combo soient initialisés pour dessiner
        if self.ui.scCombo.currentText() != '':
            compo_h = int(self.ui.compoHCombo.currentText())-1
            compo_v = int(self.ui.compoVCombo.currentText())-1
            nbp = int(self.ui.nbpCombo.currentText())+1

            sc_txt = ""
            if self.ui.scCombo.currentText() != "all":
                sc_txt = "_sc_%s"%self.ui.scCombo.currentText()
            graph_file_name = "PCA_%s_%s_%s%s"%(self.ui.compoHCombo.currentText(),self.ui.compoVCombo.currentText(),self.ui.nbpCombo.currentText(),sc_txt)

            # le conteneur auquel on va ajouter des curves
            main_draw_widget = QtGui.QWidget(self)
            l = QtGui.QVBoxLayout(main_draw_widget)
            plotc = MyMplCanvas(main_draw_widget, width=12, height=4, dpi=100)
            l.addWidget(plotc)
            plotc.fig.subplots_adjust(right=0.76,top=0.9,bottom=0.15)
            navtoolbar = NavigationToolbar(plotc, self)
            l.addWidget(navtoolbar)

            plotc.axes.set_title("Components %s and %s (%s)"%(compo_h+1,compo_v+1,graph_file_name))
            plotc.axes.grid(True)
            plotc.fig.patch.set_facecolor('white')

            posteriorList = []
            if self.ui.scCombo.currentText() == "all":
                for i in self.dico_points.keys():
                    # on ne fait pas le observed pour l'instant
                    if i != 0 and i != -1:
                        posteriorList.append(self.drawGraphToPlot(plotc,i,compo_h,compo_v,nbp))
            else:
                num_sc = int(self.ui.scCombo.currentText())
                posteriorList.append(self.drawGraphToPlot(plotc,num_sc,compo_h,compo_v,nbp))

            # on fait le observed à la fin pour qu'il soit au dessus des autres
            # et donc visible
            obs = self.drawObservedToPlot(plotc,compo_h,compo_v)

            plotc.axes.legend(bbox_to_anchor=(1.38, 1.0),prop={'size':10},scatterpoints=1,borderpad=0.7)
            plotc.axes.set_ylabel("P.C.%s (%8.2f%%)"%(compo_v+1,float(self.dico_points[-1][compo_v])*100))
            plotc.axes.set_xlabel("P.C.%s (%8.2f%%)"%(compo_h+1,float(self.dico_points[-1][compo_h])*100))
            for tick in plotc.axes.axes.xaxis.get_major_ticks():
                    tick.label1.set_fontsize(9)
            for tick in plotc.axes.axes.yaxis.get_major_ticks():
                    tick.label1.set_fontsize(9)

            if self.ui.verticalLayout_3.itemAt(0) != None:
                self.ui.verticalLayout_3.itemAt(0).widget().hide()
            self.ui.verticalLayout_3.removeItem(self.ui.verticalLayout_3.itemAt(0))
            self.ui.verticalLayout_3.addWidget(main_draw_widget)

            self.plot = plotc

    def exit(self):
        del self.dico_points
        self.parent.ui.analysisStack.removeWidget(self)
        self.parent.ui.analysisStack.setCurrentIndex(0)

    def printGraph(self):
        plot = self.plot
        im_result = QPrinter()
        #im_result.setOutputFileName("%s/analysis/%s/%s.pdf"%(self.parent.dir,self.directory,self.analysis.name))
        dial = QPrintDialog(im_result,self)
        if dial.exec_():
            painter = QPainter()
            im_result.setOrientation(QPrinter.Landscape)
            im_result.setPageMargins(7,20,20,20,QPrinter.Millimeter)
            im_result.setResolution(80)
            painter.begin(im_result)
            pen = QPen(Qt.black,2)
            painter.setPen(pen)

            bbox = plot.fig.bbox
            canvas = plot.fig.canvas
            w, h = int(bbox.width), int(bbox.height)
            reg = canvas.copy_from_bbox(bbox)
            if QtCore.QSysInfo.ByteOrder == QtCore.QSysInfo.LittleEndian:
                stringBuffer = canvas.renderer._renderer.tostring_bgra()
            else:
                stringBuffer = canvas.renderer._renderer.tostring_argb()
            qImage = QtGui.QImage(stringBuffer, w, h, QtGui.QImage.Format_ARGB32)

            painter.drawImage(QRect(QPoint(0,0),QPoint(w,h)),qImage)

            painter.end()

