# -*- coding: utf-8 -*-

import os
import shutil
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtSvg import *
from drawScenario_ui import Ui_Frame
from visualizescenario import *

class DrawScenario(QFrame):
    """ Classe pour créer une fenêtre à l'intérieur de laquelle on dessine les scénarios (valides car déjà vérifiés)
    on passe au constructeur une liste de dictionnaires contenant les informations sur chaque scénario
    """
    def __init__(self,sc_info_list=None,parent=None):
        super(DrawScenario,self).__init__(parent)
        self.parent=parent
        self.createWidgets()
        self.sc_info_list = sc_info_list
        self.pixList = []    
        self.svgList = []



    def createWidgets(self):
        self.ui = Ui_Frame()
        self.ui.setupUi(self)

        QObject.connect(self.ui.closeButton,SIGNAL("clicked()"),self.exit)
        QObject.connect(self.ui.saveButton,SIGNAL("clicked()"),self.save)

    def exit(self):
        ## reactivation des onglets
        #self.parent.parent.setTabEnabled(self.parent.parent.indexOf(self.parent),True)
        #self.parent.parent.removeTab(self.parent.parent.indexOf(self))
        #self.parent.parent.setCurrentIndex(self.parent.parent.indexOf(self.parent))
        self.parent.parent.ui.refTableStack.removeWidget(self)
        self.parent.parent.ui.refTableStack.setCurrentWidget(self.parent)

    def drawAll(self):
        """ Pour tous les scenarios (sous forme d'un dico d'infos extraites de la verif)
        on lance le dessin dans un pixmap
        """
        for sc_info in self.sc_info_list:
            if sc_info["tree"] != None:
                self.addDraw(sc_info["tree"].segments,sc_info["checker"],sc_info["tree"])
            else:
                self.addDrawError(sc_info["checker"])

    def addDrawError(self,scc):

        xmax = 500
        ymax = 450
        pix = QPixmap(xmax,ymax)
        self.pixList.append(pix)
        pix.fill(Qt.white)

        painter = QPainter(pix)
        pen = QPen(Qt.black,2)
        painter.setPen(pen)
        # dessin de la cartouche
        painter.drawLine(2,2,xmax-2,2)
        painter.drawLine(xmax-2,2,xmax-2,ymax-2)
        painter.drawLine(xmax-2,ymax-2,2,ymax-2)
        painter.drawLine(2,ymax-2,2,2)
        painter.setPen(QPen(Qt.black,6))
        painter.setPen(QPen(Qt.black,20))
        font = QFont()
        font.setItalic(False)
        font.setPixelSize(16)
        painter.setFont(font)
        painter.drawText( 10,20, "Scenario %i seems OK. But the current version of DIYABC"%(scc.number))
        painter.drawText( 10,40, "is unable to provide a valide graphic representation")

        label = QLabel()
        label.setPixmap(pix)
        self.ui.horizontalLayout_2.addWidget(label)

    def addDraw(self,segments,scc,t):
        """ dessine un scenario et l'ajoute à la fenetre
        """
        
        xmax = 500
        ymax = 450
        pix = QPixmap(xmax,ymax)
        pix.fill(Qt.white)

        painter = QPainter(pix)
        
        self.paintScenario(painter,segments,scc,t,xmax,ymax)

        label = QLabel()
        label.setPixmap(pix)
        self.ui.horizontalLayout_2.addWidget(label)
        self.pixList.append(pix)

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

    def closeEvent(self, event):
        pass
