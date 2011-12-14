# -*- coding: utf-8 -*-

import os,re
import shutil
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtSvg import *
from PyQt4 import uic
#from uis.drawScenario_ui import Ui_Frame
from utils.visualizescenario import *
from PyQt4.Qwt5 import *
from PyQt4.Qwt5.qplt import *
import output
#from uis.viewTextFile_ui import Ui_Frame as ui_viewTextFile

formDrawPCAAnalysisResult,baseDrawPCAAnalysisResult = uic.loadUiType("uis/drawScenarioFrame.ui")

class DrawPCAAnalysisResult(formDrawPCAAnalysisResult,baseDrawPCAAnalysisResult):
    """ Classe pour créer une frame à l'intérieur de laquelle on dessine les resultats d'une analyse
    Pre-evaluate PCA ou model checking
    """
    def __init__(self,analysis,directory,parent=None):
        super(DrawPCAAnalysisResult,self).__init__(parent)
        self.parent=parent
        self.directory=directory
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

        QObject.connect(self.ui.printButton,SIGNAL("clicked()"),self.printGraph)
        QObject.connect(self.ui.closeButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.savePicturesButton,SIGNAL("clicked()"),self.saveGraph)
        QObject.connect(self.ui.viewLocateButton,SIGNAL("clicked()"),self.viewLocate)

        QObject.connect(self.ui.scCombo,SIGNAL("currentIndexChanged(int)"),self.drawGraph)
        QObject.connect(self.ui.compoHCombo,SIGNAL("currentIndexChanged(int)"),self.drawGraph)
        QObject.connect(self.ui.compoVCombo,SIGNAL("currentIndexChanged(int)"),self.drawGraph)
        QObject.connect(self.ui.nbpCombo,SIGNAL("currentIndexChanged(int)"),self.drawGraph)
        self.ui.ACProgress.setValue(0)
        self.tab_colors = ["#0000FF","#00FF00","#FF0000","#00FFFF","#FF00FF","#FFFF00","#000000","#808080","#008080","#800080","#808000","#000080","#008000","#800000","#A4A0A0","#A0A4A0","#A0A0A4","#A00000","#00A000","#00A0A0"]

        #QObject.connect(self.ui.loadButton,SIGNAL("clicked()"),self.loadACP)
        # replaced by :
        #self.loadACP()
        self.ui.scrollArea.hide()
        if not os.path.exists("%s/analysis/%s/mclocate.txt"%(self.parent.dir,self.directory))\
                and not os.path.exists("%s/analysis/%s/locate.txt"%(self.parent.dir,self.directory)):
            self.ui.viewLocateButton.hide()

        self.ui.analysisNameLabel.setText("Analysis : %s"%self.analysis.name)

        self.ui.PCAGraphFrame.setMinimumSize(QSize(860, 430))
        self.ui.PCAGraphFrame.setMaximumSize(QSize(860, 430))

    def viewLocate(self):
        """ clic sur le bouton view locate
        """
        if os.path.exists("%s/analysis/%s/mclocate.txt"%(self.parent.dir,self.directory)):
            f = open("%s/analysis/%s/mclocate.txt"%(self.parent.dir,self.directory),'r')
        elif os.path.exists("%s/analysis/%s/locate.txt"%(self.parent.dir,self.directory)):
            f = open("%s/analysis/%s/locate.txt"%(self.parent.dir,self.directory),'r')
        data = f.read()
        f.close()
        #self.parent.drawAnalysisFrame = QFrame(self)
        self.parent.drawAnalysisFrame = uic.loadUi("uis/viewTextFile.ui")
        self.parent.drawAnalysisFrame.parent = self.parent
        ui = self.parent.drawAnalysisFrame
        #ui = ui_viewTextFile()
        #ui.setupUi(self.parent.drawAnalysisFrame)
        ui.dataPlain.setPlainText(data)
        ui.dataPlain.setLineWrapMode(0)
        font = "FreeMono"
        if sys.platform.startswith('win'):
            font = "Courier New"
        elif "darwin" in sys.platform:
            font = "Andale Mono"
        ui.dataPlain.setFont(QFont(font,10))
        #QObject.connect(ui.okButton,SIGNAL("clicked()"),self.parent.returnToAnalysisList)
        QObject.connect(ui.okButton,SIGNAL("clicked()"),self.returnToMe)
        self.parent.ui.analysisStack.addWidget(self.parent.drawAnalysisFrame)
        self.parent.ui.analysisStack.setCurrentWidget(self.parent.drawAnalysisFrame)

    def returnToMe(self):
        self.parent.returnTo(self)

    def loadACP(self):
        """ charge le fichier ACP dans un dico
        et initialise les combo boxes
        """
        self.ui.ACProgress.setValue(0)

        if not os.path.exists(self.acpFile):
            output.notify(self,"error","%s not found"%self.acpFile)
        else:
            f = codecs.open(self.acpFile,"r","utf-8")
            lines = f.readlines()
            f.close()
            self.dico_points = {}
            self.dico_points_posterior = {}

            nb_composantes = (len(lines[0].split(" "))-2)
            nb_prior = int(lines[0].split(" ")[0])
            nb_lignes = len(lines)
            self.dico_points[-1] = lines[0].split(" ")[2:]
            self.dico_points[0] = lines[1].split(" ")[1:]
            for i in range(len(self.dico_points[0])):
                self.dico_points[0][i] = float(self.dico_points[0][i])

            # pour chaque ligne
            i=2
            #while i < len(lines):
            while i < nb_prior+2 and i < nb_lignes:
            #for i,l in enumerate(lines):
                l=lines[i]
                pc = (float(i)/float(nb_lignes)*100)+1
                if (int(pc) % 2) == 0:
                    self.ui.ACProgress.setValue(pc)
                    QCoreApplication.processEvents()
                tab = l.split(" ")
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
                    #print "compo %s"%c
                    #print "x: %s"%float(tab[c])
                    #print "y: %s"%float(tab[c+1])
                    self.dico_points[num_sc][c-1].append( float(tab[c]) )
                    c+=1
                i+=1

            while i < nb_lignes:
                l=lines[i]
                pc = (float(i)/float(nb_lignes)*100)+1
                if (int(pc) % 2) == 0:
                    self.ui.ACProgress.setValue(pc)
                    QCoreApplication.processEvents()
                tab = l.split(" ")
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
                    #print num_sc," ",c-1," ",tab[c]," ",self.dico_points_posterior.keys()
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

    def drawGraphToPlot(self,legend,plot,num_sc,compo_h,compo_v,nbp):
        """ dessine les points pour un scenario, deux components, sur plot et met à jour legend
        le tout limité à nbp points. retourne le curve du posterior ou None s'il n'y en a pas
        """
        legend_txt = "Scenario %s"%num_sc
        c = QwtPlotCurve(legend_txt)
        c.setStyle(QwtPlotCurve.Dots)
        c.setSymbol(QwtSymbol(Qwt.QwtSymbol.Ellipse,
              QBrush(QColor(self.tab_colors[(num_sc%20)])),
                QPen(Qt.black),
                  QSize(7, 7)))
        c.setData(self.dico_points[num_sc][compo_h][:nbp], self.dico_points[num_sc][compo_v][:nbp])
        c.attach(plot)
        c.updateLegend(legend)
        # si on a des coordonnées pour le posterior, on les dessine
        # avec la meme couleur mais plus gros
        if num_sc in self.dico_points_posterior.keys():
            legend_txt = "Scenario %s posterior"%num_sc
            c = QwtPlotCurve(legend_txt)
            c.setStyle(QwtPlotCurve.Dots)
            c.setSymbol(QwtSymbol(Qwt.QwtSymbol.Ellipse,
                  QBrush(QColor(self.tab_colors[(num_sc%20)])),
                    QPen(Qt.black),
                      QSize(12, 12)))
            c.setData(self.dico_points_posterior[num_sc][compo_h][:nbp], self.dico_points_posterior[num_sc][compo_v][:nbp])
            c.attach(plot)
            c.updateLegend(legend)
            return c
        return None

    def drawObservedToPlot(self,legend,plot,compo_h,compo_v):
        """ dessine le point observé sur plot pour les deux components donnés
        """
        rp = QwtPlotCurve("Observed data set")
        rp.setStyle(QwtPlotCurve.Dots)
        rp.setSymbol(QwtSymbol(Qwt.QwtSymbol.Ellipse,
             QBrush(Qt.yellow),
               QPen(Qt.black),
                 QSize(17, 17)))
        rp.setData([self.dico_points[0][compo_h]],[self.dico_points[0][compo_v]])
        rp.attach(plot)
        rp.updateLegend(legend)
        return rp

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
            p = QwtPlot()
            p.setCanvasBackground(Qt.white)
            p.setTitle("Components %s and %s (%s)"%(compo_h+1,compo_v+1,graph_file_name))
            legend = QwtLegend()
            #legend.setItemMode(QwtLegend.CheckableItem)


            posteriorList = []
            if self.ui.scCombo.currentText() == "all":
                for i in self.dico_points.keys():
                    # on ne fait pas le observed pour l'instant
                    if i != 0 and i != -1:
                        posteriorList.append(self.drawGraphToPlot(legend,p,i,compo_h,compo_v,nbp))
            else:
                num_sc = int(self.ui.scCombo.currentText())
                posteriorList.append(self.drawGraphToPlot(legend,p,num_sc,compo_h,compo_v,nbp))

            # on fait le observed à la fin pour qu'il soit au dessus des autres
            # et donc visible
            obs = self.drawObservedToPlot(legend,p,compo_h,compo_v)

            self.fancyfyGraph(legend,p,obs,posteriorList)
            p.insertLegend(legend,QwtPlot.RightLegend)
            pm = QwtPlotMagnifier(p.canvas())
            pp = QwtPlotPanner(p.canvas())
            #pz = QwtPlotZoomer(p.canvas())
            #ppi = QwtPlotPicker(p.canvas())
            p.setAxisTitle(0,"P.C.%s (%8.2f%%)"%(compo_v+1,float(self.dico_points[-1][compo_v])*100))
            p.setAxisTitle(2,"P.C.%s (%8.2f%%)"%(compo_h+1,float(self.dico_points[-1][compo_h])*100))
            p.replot()

            fr = QFrame(self)
            fr.setFrameShape(QFrame.StyledPanel)
            fr.setFrameShadow(QFrame.Raised)
            fr.setObjectName("frame")
            #fr.setMinimumSize(QSize(800, 0))
            #fr.setMaximumSize(QSize(800, 440))
            vert = QVBoxLayout(fr)
            vert.addWidget(p)

            #if self.ui.horizontalLayout_3.itemAt(0) != None:
            #    self.ui.horizontalLayout_3.itemAt(0).widget().hide()
            #self.ui.horizontalLayout_3.removeItem(self.ui.horizontalLayout_3.itemAt(0))
            #self.ui.horizontalLayout_3.addWidget(fr)
            if self.ui.verticalLayout_3.itemAt(0) != None:
                self.ui.verticalLayout_3.itemAt(0).widget().hide()
            self.ui.verticalLayout_3.removeItem(self.ui.verticalLayout_3.itemAt(0))
            self.ui.verticalLayout_3.addWidget(fr)

            self.plot = p


    def fancyfyGraph(self,legend,p,obs,posteriorList):
        """ met en forme la légende et calcule l'intervale des divisions des axes
        """
        for it in legend.legendItems():
            f = it.font()
            f.setPointSize(14)
            it.setFont(f)
        litem = legend.find(obs)
        if litem != None:
            litem.symbol().setSize(QSize(17,17))
            litem.setIdentifierWidth(17)
        legend.setFrameShape(QFrame.Box)
        legend.setFrameShadow(QFrame.Raised)
        for postcurve in posteriorList:
            litem = legend.find(postcurve)
            if litem != None:
                litem.symbol().setSize(QSize(12,12))
                litem.setIdentifierWidth(12)

        p.replot()
        sd = p.axisScaleDiv(0)
        interval = sd.interval()
        minv = interval.minValue()
        maxv = interval.maxValue()
        inc = 1.0
        if maxv-minv < 9:
            inc = 0.5
        if maxv-minv < 5:
            inc = 0.2
        if maxv-minv < 3:
            inc = 0.1
        ticks = []
        vv = minv
        while vv < maxv:
            ticks.append(round(vv,1))
            vv += inc
        #print sd.ticks(QwtScaleDiv.MajorTick)
        sd.setTicks(QwtScaleDiv.MajorTick,ticks)
        p.setAxisScaleDiv(0,sd)
        grid = QwtPlotGrid()
        grid.attach(p)

    def printGraph(self):
        p = self.plot
        im_result = QPrinter()
        dial = QPrintDialog(im_result,self)
        if dial.exec_():
            painter = QPainter()
            im_result.setOrientation(QPrinter.Landscape)
            im_result.setPageMargins(20,20,20,20,QPrinter.Millimeter)
            im_result.setResolution(80)
            painter.begin(im_result)
            pen = QPen(Qt.black,2)
            painter.setPen(pen)
            p.print_(painter, p.rect())
            painter.end()

    def saveGraph(self):
        """ sauvegarde le graphe dans le dossier PCA_pictures du projet
        """
        if self.ui.verticalLayout_3.itemAt(0) != None:
            proj_dir = self.parent.dir
            graph_dir = self.parent.parent.PCA_dir_name
            sc_txt = ""
            if self.ui.scCombo.currentText() != "all":
                sc_txt = "_sc_%s"%self.ui.scCombo.currentText()
            graph_dir_path = "%s/analysis/%s/%s"%(proj_dir,self.directory,graph_dir)
            if not os.path.exists(graph_dir_path):
                os.mkdir(graph_dir_path)

            graph_file_path = "%s/refTable_PCA_%s_%s_%s%s"%(graph_dir_path,self.ui.compoHCombo.currentText(),self.ui.compoVCombo.currentText(),self.ui.nbpCombo.currentText(),sc_txt)
            if os.path.exists(graph_file_path):
                os.remove(graph_file_path)

            #p = self.ui.horizontalLayout_3.itemAt(0).widget()
            p = self.plot

            pic_format = str(self.parent.parent.preferences_win.ui.picturesFormatCombo.currentText())

            if pic_format == "svg":
                svg = QSvgGenerator()
                svg.setFileName("%s.svg"%(graph_file_path))
                svg.setSize(p.rect().size())

                painter = QPainter(svg)
                p.print_(painter, p.rect())
                painter.end()
            elif pic_format == "jpg" or pic_format == "png":
                pix = QPixmap(p.rect().size().width(),p.rect().size().height())
                pix.fill(Qt.white)

                painter = QPainter(pix)
                pen = QPen(Qt.black,2)
                painter.setPen(pen)
                p.print_(painter, p.rect())
                painter.end()
                im = pix.toImage()
                im.save("%s.%s"%(graph_file_path,pic_format))
            elif pic_format == "pdf":
                im_result = QPrinter()
                im_result.setOrientation(QPrinter.Landscape)
                im_result.setOutputFormat(QPrinter.PdfFormat)
                im_result.setOutputFileName('%s.pdf'%graph_file_path)
                painter = QPainter()
                im_result.setResolution(80)
                painter.begin(im_result)
                pen = QPen(Qt.black,2)
                painter.setPen(pen)
                p.print_(painter, p.rect())
                painter.end()

    def exit(self):
        del self.dico_points
        self.parent.ui.analysisStack.removeWidget(self)
        self.parent.ui.analysisStack.setCurrentIndex(0)

