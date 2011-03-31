# -*- coding: utf-8 -*-

import os,re
import shutil
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtSvg import *
from uis.drawScenario_ui import Ui_Frame
from utils.visualizescenario import *
from PyQt4.Qwt5 import *
from PyQt4.Qwt5.qplt import *

class DrawComparisonAnalysisResult(QFrame):
    """ Classe pour créer une frame à l'intérieur de laquelle on dessine les resultats d'une analyse
    comparison
    """
    def __init__(self,directory,parent=None):
        super(DrawComparisonAnalysisResult,self).__init__(parent)
        self.parent=parent
        self.directory=directory
        self.createWidgets()
        self.dicoPlot = {}  
        self.tab_colors = ["#0000FF","#00FF00","#FF0000","#00FFFF","#FF00FF","#FFFF00","#000000","#808080","#008080","#800080","#808000","#000080","#008000","#800000","#A4A0A0","#A0A4A0","#A0A0A4","#A00000","#00A000","#00A0A0"]
        
        self.drawAll()

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        QObject.connect(self.ui.closeButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.saveButton,SIGNAL("clicked()"),self.save)

        self.ui.PCAFrame.hide()
        self.ui.ACProgress.hide()

    def exit(self):
        self.parent.ui.analysisStack.removeWidget(self)
        self.parent.ui.analysisStack.setCurrentIndex(0)

    def getCoord(self,filepath):
        """ extrait les coordonnées du fichier passé en paramètre
        """
        f = codecs.open(filepath,"r","utf-8")
        lines = f.readlines()
        f.close()
        l = 0
        pat = re.compile(r'\s+')
        # on remplace les suites d'espace par un seul espace
        first_line_tab = pat.sub(' ',lines[0].strip()).split(' ')
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
                line_tab = pat.sub(' ',lines[l].strip()).split(' ')
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
                print "complogreg.txt not found"
        else:
            print "compdirect.txt not found"


    def addDraw(self,first_line_tab,dico_coord,direct):
        
        p = QwtPlot()
        p.setCanvasBackground(Qt.white)
        if direct:
            p.setTitle("Direct")
        else:
            p.setTitle("Logistic regression")
        legend = QwtLegend()

        labs = dico_coord['n']
            
        for i in range(len(first_line_tab)):    
            legend_txt = "Scenario %s"%first_line_tab[i]
            pr = QwtPlotCurve(legend_txt)
            pr.setStyle(QwtPlotCurve.Lines)
            pr.setPen(QPen(QColor(self.tab_colors[i]),2))
            pr.setSymbol(QwtSymbol(Qwt.QwtSymbol.NoSymbol,
                  QBrush(QColor(self.tab_colors[1])),
                    QPen(QColor(self.tab_colors[1])),
                      QSize(7, 7)))
            pr.setData(labs,dico_coord[first_line_tab[i]])
            pr.attach(p)
            pr.updateLegend(legend)

        for it in legend.legendItems():
            f = it.font()
            f.setPointSize(11)
            it.setFont(f)
        #litem = legend.find(obs)
        #litem.symbol().setSize(QSize(17,17))
        #litem.setIdentifierWidth(17)
        legend.setFrameShape(QFrame.Box)
        legend.setFrameShadow(QFrame.Raised)

        p.replot()
        grid = QwtPlotGrid()
        grid.setPen(QPen(Qt.black,0.5))
        grid.attach(p)
        p.insertLegend(legend,QwtPlot.BottomLegend)

        fr = QFrame(self)
        fr.setFrameShape(QFrame.StyledPanel)
        fr.setFrameShadow(QFrame.Raised)
        fr.setObjectName("frame")
        fr.setMinimumSize(QSize(400, 0))
        fr.setMaximumSize(QSize(400, 300))
        vert = QVBoxLayout(fr)
        vert.addWidget(p)

        self.ui.horizontalLayout_2.addWidget(fr)
        if direct:
            self.dicoPlot['direct'] = p
        else:
            self.dicoPlot['logistic'] = p

    def save(self):
        """ clic sur le bouton save
        """
        # nettoyage radical : suppression du dossier
        proj_dir = self.parent.dir
        pic_dir = "%s/analysis/%s/pictures"%(proj_dir,self.directory)
        if os.path.exists(pic_dir):
            shutil.rmtree(pic_dir)
        # puis on le recrée, vide évidemment
        os.mkdir(pic_dir)

        answer = QMessageBox.question(self,"Saving option","Would you like to save all images in one file or in separated files ?",\
                "All in one","Separated")
        if answer == 0:
            self.saveDrawsToOne()
        elif answer == 1:
            self.saveEachDraws()

    def saveEachDraws(self):
        """ Sauve chaque scenario dans un fichier
        """
        proj_dir = self.parent.dir
        pic_dir = "%s/analysis/%s/pictures"%(proj_dir,self.directory)
        pic_basename = "comparison"
        pic_whole_path = "%s/%s_"%(pic_dir,pic_basename)

        pic_format = str(self.parent.parent.preferences_win.ui.formatCombo.currentText())
        if pic_format == "jpg" or pic_format == "png":
            for name in self.dicoPlot.keys():
                p = self.dicoPlot[name]
                savename = "%s%s.%s"%(pic_whole_path,name,pic_format)
                pix = QPixmap(p.rect().size().width(),p.rect().size().height())
                pix.fill(Qt.white)
                painter = QPainter(pix)
                pen = QPen(Qt.black,2)
                painter.setPen(pen)
                p.print_(painter, p.rect())
                painter.end()
                im = pix.toImage()
                im.save(savename)
        else:
            for name in self.dicoPlot.keys():
                p = self.dicoPlot[name]
                savename = "%s%s.svg"%(pic_whole_path,name)
                svg = QSvgGenerator()
                svg.setFileName(savename)
                svg.setSize(p.rect().size())

                painter = QPainter(svg)
                p.print_(painter, p.rect())
                painter.end()

    def saveDrawsToOne(self):
        """ Sauve tous les scenarios dans une seule image et un seul svg
        """
        proj_dir = self.parent.dir
        pic_dir = "%s/analysis/%s/pictures"%(proj_dir,self.directory)
        pic_basename = "posterior"
        pic_whole_path = "%s/%s_"%(pic_dir,pic_basename)

        pic_format = str(self.parent.parent.preferences_win.ui.formatCombo.currentText())

        nbPlot = len(self.dicoPlot.keys())
        largeur = 2
        # resultat de la div entière plus le reste (modulo)
        longueur = (nbPlot/largeur)+(nbPlot%largeur)
        ind = 0
        li=0

        # on prend un des graphes pour savoir ses dimensions
        size = self.dicoPlot[self.dicoPlot.keys()[0]].rect().size()

        if pic_format == "jpg" or pic_format == "png":
            self.im_result = QImage(largeur*(size.width()),longueur*(size.height()),QImage.Format_RGB32)
            self.im_result.fill(Qt.black)
            painter = QPainter(self.im_result)
            painter.fillRect(0, 0, largeur*(size.width()), longueur*(size.height()), Qt.white)
        else:
            self.pic_result = QSvgGenerator()
            #self.pic_result.setSize(QSize(largeur*500, longueur*450));
            #self.pic_result.setViewBox(QRect(0, 0, largeur*500, longueur*450));
            self.pic_result.setFileName("%sall.svg"%pic_whole_path)
            painter_svg = QPainter()
            painter_svg.begin(self.pic_result)

        keys = self.dicoPlot.keys()
        # on fait des lignes tant qu'on a des pix
        while (ind < nbPlot):
            col = 0
            # une ligne
            while (ind < nbPlot) and (col < largeur):
                plot = self.dicoPlot[keys[ind]]
                if pic_format == "jpg" or pic_format == "png":
                    plot.print_(painter, QRect(QPoint(col*size.width(),li*size.height()),QSize(size)))
                else:
                    plot.print_(painter_svg, QRect(QPoint(col*size.width(),li*size.height()),QSize(size)))
                col+=1
                ind+=1
            li+=1

        if pic_format == "jpg" or pic_format == "png":
            self.im_result.save("%sall.%s"%(pic_whole_path,pic_format))
        else:
            painter_svg.end()
