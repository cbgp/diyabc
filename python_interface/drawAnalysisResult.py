# -*- coding: utf-8 -*-

import os
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtSvg import *
from uis.drawScenario_ui import Ui_Frame
from utils.visualizescenario import *
from PyQt4.Qwt5 import *
from PyQt4.Qwt5.qplt import *

class DrawAnalysisResult(QFrame):
    """ Classe pour créer une fenêtre à l'intérieur de laquelle on dessine les resultats d'une analyse
    """
    def __init__(self,directory,parent=None):
        super(DrawAnalysisResult,self).__init__(parent)
        self.parent=parent
        self.directory=directory
        self.createWidgets()
        self.pixList = []    
        self.svgList = []
        self.tab_colors = ["#0000FF","#00FF00","#FF0000","#00FFFF","#FF00FF","#FFFF00","#000000","#808080","#008080","#800080","#808000","#000080","#008000","#800000","#A4A0A0","#A0A4A0","#A0A0A4","#A00000","#00A000","#00A0A0"]
        
        self.drawAll()

    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        QObject.connect(self.ui.closeButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.saveButton,SIGNAL("clicked()"),self.save)

    def exit(self):
        self.parent.ui.analysisStack.removeWidget(self)
        self.parent.ui.analysisStack.setCurrentIndex(0)

    def drawAll(self):
        if os.path.exists("%s/analysis/%s/paramstatdens.txt"%(self.parent.dir,self.directory)):
            f = open("%s/analysis/%s/paramstatdens.txt"%(self.parent.dir,self.directory),"r")
            lines = f.readlines()
            f.close()
            l = 0
            while l < (len(lines) - 1):
                name = lines[l].strip()
                values = lines[l+1]
                absv = lines[l+3]
                ordpr = lines[l+4]
                ordpo = lines[l+5]
                self.addDraw(name,values,absv,ordpr,ordpo)
                l += 6

        else:
            print "paramstatdens.txt not found"


    def addDraw(self,name,values,absv,ordpr,ordpo):
        
        p = QwtPlot()
        p.setCanvasBackground(Qt.white)
        p.setTitle(name)
        legend = QwtLegend()

        labs = []
        for num in absv.strip().split('  '):
            labs.append(float(num))
        lpr = []
        for num in ordpr.strip().split('  '):
            lpr.append(float(num))
        lpo = []
        for num in ordpo.strip().split('  '):
            lpo.append(float(num))
            
        legend_txt = "prior"
        pr = QwtPlotCurve(legend_txt)
        pr.setStyle(QwtPlotCurve.Lines)
        pr.setPen(QPen(QColor(self.tab_colors[1]),3))
        pr.setSymbol(QwtSymbol(Qwt.QwtSymbol.NoSymbol,
              QBrush(QColor(self.tab_colors[1])),
                QPen(QColor(self.tab_colors[1])),
                  QSize(7, 7)))
        pr.setData(labs,lpr)
        pr.attach(p)
        pr.updateLegend(legend)

        legend_txt = "posterior"
        post = QwtPlotCurve(legend_txt)
        post.setStyle(QwtPlotCurve.Lines)
        post.setPen(QPen(QColor(self.tab_colors[2]),3))
        post.setSymbol(QwtSymbol(Qwt.QwtSymbol.NoSymbol,
              QBrush(QColor(self.tab_colors[2])),
                QPen(QColor(self.tab_colors[2])),
                  QSize(7, 7)))
        post.setData(labs,lpo)
        post.attach(p)
        post.updateLegend(legend)

        for it in legend.legendItems():
            f = it.font()
            f.setPointSize(14)
            it.setFont(f)
        #litem = legend.find(obs)
        #litem.symbol().setSize(QSize(17,17))
        #litem.setIdentifierWidth(17)
        legend.setFrameShape(QFrame.Box)
        legend.setFrameShadow(QFrame.Raised)

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

        self.ui.horizontalLayout_2.addWidget(p)
        self.ui.horizontalLayout_3.addWidget(QLabel("plop"))
        self.pixList.append(p)

    def DrawSvg(self,segments,scc,t,savename):
        """ dessine un scenario dans un fichier svg
        """
        
        xmax = 500
        ymax = 450
        svgpic = QSvgGenerator()
        svgpic.setFileName(savename)
        self.svgList.append(svgpic)

        painter = QPainter()
        painter.begin(svgpic)

        self.paintScenario(painter,segments,scc,t,xmax,ymax)
        
        painter.end()

    def paintScenario(self,painter,segments,scc,t,xmax,ymax,font_inc=0):
        """ dessine le scenario sur le painter
        """
        tab_colors = ["#0000FF","#00FF00","#FF0000","#00FFFF","#FF00FF","#FFFF00","#000000","#808080","#008080","#800080","#808000","#000080","#008000","#800000","#A4A0A0","#A0A4A0","#A0A0A4","#A00000","#00A000","#00A0A0"]
        NN = scc.history.ne0s

        pen = QPen(Qt.black,2)
        painter.setPen(pen)

        # dessin de la cartouche
        painter.drawLine(2,2,xmax-2,2)
        painter.drawLine(xmax-2,2,xmax-2,ymax-2)
        painter.drawLine(xmax-2,ymax-2,2,ymax-2)
        painter.drawLine(2,ymax-2,2,2)
        painter.setPen(QPen(Qt.black,6))
        # TODO verifier la validité de NN qui doit avoir une taille de 1 quand N N N
        for i in range(len(NN)):
            pen = QPen(Qt.yellow,6)
            pen.setCapStyle(Qt.RoundCap)
            pen.setColor(QColor(tab_colors[(i%20)]))
            painter.setPen(pen)
            painter.drawLine(20,40+20*i,30,40+20*i)
            pen.setColor(QColor("#000000"))
            font = QFont()
            font.setPixelSize(10+font_inc)
            painter.setFont(font)
            painter.setPen(pen)
            painter.drawText( 40,45+20*i, NN[i].name)
        painter.setPen(QPen(Qt.black,20))
        font = QFont()
        font.setItalic(False)
        font.setPixelSize(16+font_inc)
        painter.setFont(font)
        painter.drawText( 10,20+(font_inc/2), "Scenario %i"%(scc.number))

        # echelle de temps
        pen = QPen(Qt.black,1)
        painter.setPen(pen)
        painter.drawLine(xmax-50,50,xmax-50,ymax-50)
        pen = QPen(Qt.black,4)
        painter.setPen(pen)
        for i in range(len(scc.history.events)):
            y = scc.history.events[i].y
            painter.drawLine(xmax-55,y,xmax-45,y)
            font = QFont()
            font.setItalic(False)
            font.setPixelSize(10+font_inc)
            painter.setFont(font)
            painter.drawText(xmax-30,y+5,scc.history.events[i].stime)
        font = QFont()
        font.setItalic(False)
        font.setPixelSize(10)
        painter.setFont(font)
        painter.drawText(xmax-170,14,"(Warning ! Time is not to scale.)");

        # echantillons
        pen = QPen(Qt.black,1)
        painter.setPen(pen)
        n=0
        for i in range(0,len(t.node)):
            if (t.node[i].category == "sa") or (t.node[i].category == "sa2"):
                for j in range(0,len(t.node)):
                    if ((t.node[i].category == "sa") or (t.node[i].category == "sa2")) and (t.node[i].category != t.node[j].category):
                        if t.node[i].y > t.node[j].y:
                            t.node[i].category = "sa";
                            t.node[j].category = "sa2";
                        else:
                            t.node[j].category = "sa";
                            t.node[i].category = "sa2";
                    for i in range(0,len(t.node)):
                        if (t.node[i].category == "sa") or (t.node[i].category == "sa2"):
                            x = t.node[i].x
                            y = t.node[i].y
                            n+=1
                            pen = QPen(Qt.black,1)
                            painter.setPen(pen)
                            painter.drawRoundRect(x-15,y+10,35,20)
                            font = QFont()
                            font.setItalic(True)
                            font.setPixelSize(10+font_inc)
                            painter.setFont(font)
                            painter.drawText(x-10,y+25,'Sa %i'%t.node[i].pop)
                            font = QFont()
                            font.setItalic(False)
                            font.setPixelSize(12+font_inc)
                            painter.setFont(font)
                            if t.node[i].category == "sa":
                                painter.drawText(x-19,y+50,"Pop %i"%t.node[i].pop)


        # dessin des segments
        for s in segments:
            #print "seg",s
            if s.ydeb == s.yfin and s.xdeb != s.xfin:
                xmed = (s.xdeb+s.xfin)/2
                ymed = s.ydeb
                jj = 0
                while(jj < len(NN)) and (NN[jj].name != s.sNedeb):
                    jj+=1
                pen = QPen(Qt.yellow,10)
                pen.setCapStyle(Qt.RoundCap)
                pen.setColor(QColor(tab_colors[(jj%20)]))
                painter.setPen(pen)
                painter.drawLine(s.xdeb,ymed,xmed,ymed)
                painter.setPen(QPen(Qt.black,8))
                x0 = xmed-(2*len(s.sadm))
                font = QFont()
                font.setItalic(False)
                font.setPixelSize(10)
                painter.setFont(font)
                painter.drawText( x0, ymed+16, s.sadm)

                jj = 0
                while(jj < len(NN)) and (NN[jj].name != s.sNefin):
                    jj+=1
                pen = QPen(Qt.yellow,10)
                pen.setCapStyle(Qt.RoundCap)
                pen.setColor(QColor(tab_colors[(jj%20)]))
                painter.setPen(pen)
                painter.drawLine(s.xfin,ymed,xmed,ymed)
            else:
                jj = 0
                while(jj < len(NN)) and (NN[jj].name != s.sNefin):
                    jj+=1
                pen = QPen(Qt.black,10)
                pen.setCapStyle(Qt.RoundCap)
                pen.setColor(QColor(tab_colors[(jj%20)]))
                painter.setPen(pen)
                painter.drawLine(s.xdeb,s.ydeb,s.xfin,s.yfin)

    def save(self):
        """ clic sur le bouton save
        """
        # nettoyage radical : suppression du dossier
        proj_dir = self.parent.parent.dir
        pic_dir = self.parent.parent.parent.scenario_pix_dir_name
        if os.path.exists("%s/%s"%(proj_dir,pic_dir)):
            shutil.rmtree("%s/%s"%(proj_dir,pic_dir))
        # puis on le recrée, vide évidemment
        os.mkdir("%s/%s"%(proj_dir,pic_dir))

        answer = QMessageBox.question(self,"Saving option","Would you like to save all images in one file or in separated files ?",\
                "All in one","Separated")
        if answer == 0:
            self.saveDrawsToOne()
        elif answer == 1:
            self.saveEachDraws()

    def saveEachDraws(self):
        """ Sauve chaque scenario dans un fichier
        """
        proj_dir = self.parent.parent.dir
        pic_dir = self.parent.parent.parent.scenario_pix_dir_name
        pic_basename = self.parent.parent.parent.scenario_pix_basename
        pic_whole_path = "%s/%s/%s_%s"%(proj_dir,pic_dir,self.parent.parent.name,pic_basename)

        ## si le rep contenant les images n'existe pas, on le crée
        #if not os.path.exists("%s/%s"%(proj_dir,pic_dir)):
        #    os.mkdir("%s/%s"%(proj_dir,pic_dir))
        #else:
        #    # effacer les anciennes images
        #    for i in range(100):
        #        if os.path.exists("%s_%i.jpg"%(pic_whole_path,i)):
        #            os.remove("%s_%i.jpg"%(pic_whole_path,i))

        pic_format = str(self.parent.parent.parent.preferences_win.ui.formatCombo.currentText())
        if pic_format == "jpg":
            for ind,pix in enumerate(self.pixList):
                im = pix.toImage()
                im.save("%s_%i.jpg"%(pic_whole_path,ind+1))
        else:
            for ind,sc_info in enumerate(self.sc_info_list):
                if sc_info["tree"] != None:
                    savename = "%s_%i.svg"%(pic_whole_path,ind+1)
                    self.DrawSvg(sc_info["tree"].segments,sc_info["checker"],sc_info["tree"],savename)

    def saveDrawsToOne(self):
        """ Sauve tous les scenarios dans une seule image et un seul svg
        """
        proj_dir = self.parent.parent.dir
        pic_dir = self.parent.parent.parent.scenario_pix_dir_name
        pic_basename = self.parent.parent.parent.scenario_pix_basename
        pic_whole_path = "%s/%s/%s_%s"%(proj_dir,pic_dir,self.parent.parent.name,pic_basename)

        pic_format = str(self.parent.parent.parent.preferences_win.ui.formatCombo.currentText())

        nbpix = len(self.pixList)
        largeur = 2
        # resultat de la div entière plus le reste (modulo)
        longueur = (len(self.pixList)/largeur)+(len(self.pixList)%largeur)
        ind = 0
        li=0

        if pic_format == "jpg":
            self.im_result = QImage(largeur*500,longueur*450,QImage.Format_RGB32)
            self.im_result.fill(Qt.black)
            painter = QPainter(self.im_result)
            painter.fillRect(0, 0, largeur*500, longueur*450, Qt.white)
        else:
            self.pic_result = QSvgGenerator()
            #self.pic_result.setSize(QSize(largeur*500, longueur*450));
            #self.pic_result.setViewBox(QRect(0, 0, largeur*500, longueur*450));
            self.pic_result.setFileName("%s_all.svg"%pic_whole_path)
            painter_pic = QPainter()
            painter_pic.begin(self.pic_result)

        # on fait des lignes tant qu'on a des pix
        while (ind < nbpix):
            col = 0
            if pic_format == "svg":
                if ind > 0:
                    painter_pic.translate(-2*500,450)
            # une ligne
            while (ind < nbpix) and (col < largeur):
                # ajout
                #self.im_result.fill(self.pixList[ind],QPoint(col*500,li*450))
                #painter_pic.drawImage(QPoint(col*500,li*450),self.pixList[ind].toImage())
                if pic_format == "jpg":
                    painter.drawImage(QPoint(col*500,li*450),self.pixList[ind].toImage())
                else:
                    sc_info = self.sc_info_list[ind]
                    if sc_info["tree"] != None:
                        self.paintScenario(painter_pic,sc_info["tree"].segments,sc_info["checker"],sc_info["tree"],500,450)
                    # on va a droite
                    painter_pic.translate(500,0)
                #print "li:",li," col:",col
                #print "xof:",col*500," yof:",li*450
                #print "zzz"
                col+=1
                ind+=1
            li+=1

        if pic_format == "jpg":
            self.im_result.save("%s_all.jpg"%pic_whole_path)
        else:
            painter_pic.end()
