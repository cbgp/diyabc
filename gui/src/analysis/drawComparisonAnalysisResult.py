# -*- coding: utf-8 -*-

import os
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtSvg import *
from PyQt4 import uic
from utils.visualizescenario import *
from viewTextFile import ViewTextFile
from utils.cbgpUtils import log
from utils.matplotlibCanvas import *
import variables
from variables import UIPATH

formDrawComparisonAnalysisResult,baseDrawComparisonAnalysisResult = uic.loadUiType("%s/drawScenarioFrame.ui"%UIPATH)

class DrawComparisonAnalysisResult(formDrawComparisonAnalysisResult,baseDrawComparisonAnalysisResult):
    """ Classe pour créer une frame à l'intérieur de laquelle on dessine les resultats d'une analyse
    comparison
    """
    def __init__(self,analysis,directory,parent=None):
        super(DrawComparisonAnalysisResult,self).__init__(parent)
        self.parent=parent
        self.directory=directory
        self.analysis = analysis
        self.createWidgets()
        self.dicoPlot = {}  
        self.tab_colors = ["#0000FF","#00FF00","#FF0000","#00FFFF","#FF00FF","#FFFF00","#000000","#808080","#008080","#800080","#808000","#000080","#008000","#800000","#A4A0A0","#A0A4A0","#A0A0A4","#A00000","#00A000","#00A0A0"]
        
        self.drawAll()

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        QObject.connect(self.ui.printButton,SIGNAL("clicked()"),self.printGraphs)
        QObject.connect(self.ui.closeButton,SIGNAL("clicked()"),self.exit)
        #QObject.connect(self.ui.savePicturesButton,SIGNAL("clicked()"),self.save)
        QObject.connect(self.ui.viewLocateButton,SIGNAL("clicked()"),self.viewCompDirectLogReg)
        self.ui.savePicturesButton.hide()
        #self.ui.printButton.hide()

        self.ui.PCAFrame.hide()
        self.ui.ACProgress.hide()
        self.ui.viewLocateButton.setText("view numerical results")
        self.ui.PCAGraphFrame.hide()
        self.ui.analysisNameLabel.setText("Analysis : %s"%self.analysis.name)

        self.ui.parameterChoiceFrame.hide()

    def exit(self):
        self.parent.ui.analysisStack.removeWidget(self)
        self.parent.ui.analysisStack.setCurrentIndex(0)

    def viewCompDirectLogReg(self):
        """ clic sur le bouton view numerical results
        """
        if os.path.exists("%s/analysis/%s/compdirlog.txt"%(self.parent.dir,self.directory)):
            f = open("%s/analysis/%s/compdirlog.txt"%(self.parent.dir,self.directory))
            textToDisplay = f.read()
            f.close()
            self.parent.drawAnalysisFrame = ViewTextFile(textToDisplay,self.returnToMe,self.parent)
            self.parent.drawAnalysisFrame.choiceFrame.hide()
            self.parent.ui.analysisStack.addWidget(self.parent.drawAnalysisFrame)
            self.parent.ui.analysisStack.setCurrentWidget(self.parent.drawAnalysisFrame)

    def returnToMe(self):
        self.parent.returnTo(self)

    def getCoord(self,filepath):
        """ extrait les coordonnées du fichier passé en paramètre
        """
        f = codecs.open(filepath,"r","utf-8")
        lines = f.readlines()
        f.close()
        l = 0
        #pat = re.compile(r'\s+')
        # on remplace les suites d'espace par un seul espace
        #first_line_tab = pat.sub(' ',lines[0].strip()).split(' ')
        first_line_tab = lines[0].split()
        # on vire le 'n'
        first_line_tab = first_line_tab[1:]

        for i in range(first_line_tab.count('scenario')):
            first_line_tab.remove('scenario')

        dico_coord = {}
        # initialisation du dico des coordonnées
        for i in range(len(first_line_tab)):
            dico_coord[first_line_tab[i]] = []
        dico_coord['n'] = []

        l=1
        while l < (len(lines)):
            if lines[l].strip() != "":
                #line_tab = pat.sub(' ',lines[l].strip()).split(' ')
                line_tab = lines[l].split()
                # on ajoute une abcisse
                dico_coord['n'].append(float(line_tab[0]))
                line_tab = line_tab[1:]
                # on supprime les intervalles de confiance
                l_to_del = []
                for i in range(len(line_tab)):
                    if (i%2) == 1:
                        l_to_del.append(i)
                l_to_del.reverse()
                for i in l_to_del:
                    line_tab.pop(i)
                # pour chaque scenario, on ajoute une ordonnée
                for i in range(len(first_line_tab)):
                    dico_coord[first_line_tab[i]].append(float(line_tab[i]))
            l += 1
        return (first_line_tab,dico_coord)

    def drawAll(self):
        """ dessine les graphes de tous les paramètres
        """
        if os.path.exists("%s/analysis/%s/compdirect.txt"%(self.parent.dir,self.directory)):
            (first_line_tab,dico_coord) = self.getCoord("%s/analysis/%s/compdirect.txt"%(self.parent.dir,self.directory))
            self.addDraw(first_line_tab,dico_coord,True)
            if os.path.exists("%s/analysis/%s/complogreg.txt"%(self.parent.dir,self.directory)):
                (first_line_tab,dico_coord) = self.getCoord("%s/analysis/%s/complogreg.txt"%(self.parent.dir,self.directory))
                self.addDraw(first_line_tab,dico_coord,False)
            else:
                log(3, "complogreg.txt not found")
        else:
            log(3, "compdirect.txt not found")


    def addDraw(self,first_line_tab,dico_coord,direct):
        """ dessine et affiche un graphe
        """        
        draw_widget = QtGui.QWidget(self)
        l = QtGui.QVBoxLayout(draw_widget)
        plotc = MyMplCanvas(draw_widget, width=12, height=4, dpi=100)
        l.addWidget(plotc)
        #plotc.fig.subplots_adjust(right=0.7,top=0.9,bottom=0.15)
        navtoolbar = NavigationToolbar(plotc, self)
        l.addWidget(navtoolbar)

        plotc.axes.grid(True)

        if direct:
            plotc.axes.set_title("Direct")
        else:
            plotc.axes.set_title("Logistic regression")


        labs = dico_coord['n']
            
        for i in range(len(first_line_tab)):    
            legend_txt = "Scenario %s"%first_line_tab[i]
            plotc.axes.plot(labs,dico_coord[first_line_tab[i]],label=legend_txt,c=self.tab_colors[i])

        plotc.axes.legend(bbox_to_anchor=(0.74, -0.14),ncol=2,prop={'size':9})
        plotc.axes.axes.title.set_fontsize(10)
        for tick in plotc.axes.axes.xaxis.get_major_ticks():
                tick.label1.set_fontsize(7)
        for tick in plotc.axes.axes.yaxis.get_major_ticks():
                tick.label1.set_fontsize(7)
        plotc.fig.subplots_adjust(right=0.92,top=0.9,bottom=0.27)
        plotc.fig.patch.set_facecolor('white')

        fr = QFrame(self)
        fr.setFrameShape(QFrame.StyledPanel)
        fr.setFrameShadow(QFrame.Raised)
        fr.setObjectName("frame")
        fr.setMinimumSize(QSize(400, 0))
        fr.setMaximumSize(QSize(400, 300))
        vert = QVBoxLayout(fr)
        vert.addWidget(draw_widget)

        self.ui.horizontalLayout_2.addWidget(fr)
        if direct:
            self.dicoPlot['direct'] = plotc
        else:
            self.dicoPlot['logistic'] = plotc

    def printGraphs(self):
        ifrom = 0
        ito = 1
        nbPlot = (ito - ifrom)+1
        largeur = 2
        # resultat de la div entière plus le reste (modulo)
        longueur = (nbPlot/largeur)+(nbPlot%largeur)
        ind = ifrom
        li=0

        # on prend un des graphes pour savoir ses dimensions
        #size = self.dicoPlot[self.dicoPlot.keys()[0]].rect().size()

        im_result = QPrinter()
        im_result.setOutputFileName("%s/analysis/%s/%s.pdf"%(self.parent.dir,self.directory,self.analysis.name))
        painter = QPainter()

        dial = QPrintDialog(im_result,self)
        if dial.exec_():
            im_result.setOrientation(QPrinter.Portrait)
            im_result.setPageMargins(20,20,20,20,QPrinter.Millimeter)
            im_result.setResolution(60)

            painter.begin(im_result)

            keys = self.dicoPlot.keys()
            # on fait des lignes tant qu'on a des pix
            while (ind <= ito):
                col = 0
                # une ligne
                while (ind <= ito) and (col < largeur):
                    plot = self.dicoPlot[keys[ind]]

                    bbox = plot.fig.bbox
                    canvas = plot.fig.canvas
                    w, h = int(bbox.width), int(bbox.height)
                    reg = canvas.copy_from_bbox(bbox)
                    if QtCore.QSysInfo.ByteOrder == QtCore.QSysInfo.LittleEndian:
                        stringBuffer = canvas.renderer._renderer.tostring_bgra()
                    else:
                        stringBuffer = canvas.renderer._renderer.tostring_argb()
                    qImage = QtGui.QImage(stringBuffer, w, h, QtGui.QImage.Format_ARGB32)

                    painter.drawImage(QRect(QPoint(0*w,(li*h)+li*30),QPoint(1*w,((li+1)*h)+li*30)),qImage)
                    col+=1
                    ind+=1
                    li+=1
                li+=1

            painter.end()
