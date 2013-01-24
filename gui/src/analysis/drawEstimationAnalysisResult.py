# -*- coding: utf-8 -*-

import os
import shutil
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtSvg import *
from PyQt4 import uic
from utils.visualizescenario import *
from viewTextFile import ViewTextFile
from utils.cbgpUtils import log
from utils.matplotlib_example import *
import output

formDrawEstimationAnalysisResult,baseDrawEstimationAnalysisResult = uic.loadUiType("uis/drawScenarioFrame.ui")

class DrawEstimationAnalysisResult(formDrawEstimationAnalysisResult,baseDrawEstimationAnalysisResult):
    """ Classe pour créer une frame à l'intérieur de laquelle on dessine les resultats d'une analyse
    pour l'instant : estimation
    """
    def __init__(self,analysis,directory,parent=None):
        super(DrawEstimationAnalysisResult,self).__init__(parent)
        self.parent=parent
        self.directory=directory
        self.analysis = analysis
        self.paramChoice = "o"
        self.file_subname = "_original"
        self.createWidgets()
        self.dicoPlot = {}  
        self.dicoFrame = {}
        self.svgList = []
        self.tab_colors = ["#0000FF","#00FF00","#FF0000","#00FFFF","#FF00FF","#FFFF00","#000000","#808080","#008080","#800080","#808000","#000080","#008000","#800000","#A4A0A0","#A0A4A0","#A0A0A4","#A00000","#00A000","#00A0A0"]
        
        self.drawAll()

    def createWidgets(self):
        self.ui=self
        self.ui.setupUi(self)

        QObject.connect(self.ui.printButton,SIGNAL("clicked()"),self.printDraws)
        QObject.connect(self.ui.closeButton,SIGNAL("clicked()"),self.exit)
        #self.ui.savePicturesButton.hide()
        self.ui.savePicturesButton.setText("Export all to pdf")
        #self.ui.printButton.hide()
        QObject.connect(self.ui.savePicturesButton,SIGNAL("clicked()"),self.saveDrawsToOnePdf)
        QObject.connect(self.ui.viewLocateButton,SIGNAL("clicked()"),self.viewMmmq)
        
        QObject.connect(self.ui.oRadio,SIGNAL("clicked()"),self.changeParamChoice)
        QObject.connect(self.ui.cRadio,SIGNAL("clicked()"),self.changeParamChoice)
        QObject.connect(self.ui.sRadio,SIGNAL("clicked()"),self.changeParamChoice)

        self.ui.PCAFrame.hide()
        self.ui.ACProgress.hide()
        self.ui.viewLocateButton.setText("view numerical results")
        self.ui.PCAGraphFrame.hide()
        self.ui.analysisNameLabel.setText("Analysis : %s"%self.analysis.name)
        self.ui.PCAScroll.hide()

        if not os.path.exists("%s/analysis/%s/mmmq_composite.txt"%(self.parent.dir,self.directory))\
        and not os.path.exists("%s/analysis/%s/paramstatdens_composite.txt"%(self.parent.dir,self.directory)):
            self.ui.cRadio.hide()
        if not os.path.exists("%s/analysis/%s/mmmq_scaled.txt"%(self.parent.dir,self.directory))\
        and not os.path.exists("%s/analysis/%s/paramstatdens_scaled.txt"%(self.parent.dir,self.directory)):
            self.ui.sRadio.hide()

        self.ui.horizontalLayout_2.setContentsMargins(0,0,0,0)
        self.ui.horizontalLayout_3.setContentsMargins(0,0,0,0)

    def changeParamChoice(self):
        sd = self.sender()
        if sd == self.ui.oRadio:
            parenthText = "(Original)"
            self.paramChoice = "o"
            self.file_subname = "_original"
        elif sd == self.ui.cRadio:
            parenthText = "(Composite)"
            self.paramChoice = "c"
            self.file_subname = "_composite"
        elif sd == self.ui.sRadio:
            parenthText = "(Scaled)"
            self.paramChoice = "s"
            self.file_subname = "_scaled"

        self.ui.viewLocateButton.setText("View numerical results " + parenthText)
        self.drawAll()

    def exit(self):
        self.parent.ui.analysisStack.removeWidget(self)
        self.parent.ui.analysisStack.setCurrentIndex(0)

    def viewMmmq(self):
        """ clic sur le bouton view numerical
        """
        if os.path.exists("%s/analysis/%s/mmmq%s.txt"%(self.parent.dir,self.directory,self.file_subname)):
            f = open("%s/analysis/%s/mmmq%s.txt"%(self.parent.dir,self.directory,self.file_subname),'r')
            data = f.read()
            f.close()

            self.parent.drawAnalysisFrame = ViewTextFile(data,self.returnToMe,self.parent)
            self.parent.drawAnalysisFrame.choiceFrame.hide()

            self.parent.ui.analysisStack.addWidget(self.parent.drawAnalysisFrame)
            self.parent.ui.analysisStack.setCurrentWidget(self.parent.drawAnalysisFrame)
        else:
            log(3, "mmmq%s.txt not found for analysis %s"%(self.file_subname,self.analysis.name))

    def returnToMe(self):
        self.parent.returnTo(self)

    def drawAll(self):
        """ dessine les graphes de tous les paramètres
        """
        # nettoyage
        for fr in self.findChildren(QFrame,"frameDraw"):
            self.ui.horizontalLayout_2.removeWidget(fr)
            fr.hide()
            del fr
        for fr in self.findChildren(QFrame,"frameValues"):
            self.ui.horizontalLayout_3.removeWidget(fr)
            fr.hide()
            del fr
        self.dicoPlot = {}

        if os.path.exists("%s/analysis/%s/paramstatdens%s.txt"%(self.parent.dir,self.directory,self.file_subname)):
            f = codecs.open("%s/analysis/%s/paramstatdens%s.txt"%(self.parent.dir,self.directory,self.file_subname),"r","utf-8")
            lines = f.readlines()
            f.close()
            dico_info_draw = {}
            l = 0
            while l < (len(lines) - 1):
                name = lines[l].strip()
                values = lines[l+1]
                absv = lines[l+3]
                ordpr = lines[l+4]
                ordpo = lines[l+5]
                dico_info_draw[name] = [values,absv,ordpr,ordpo]
                l += 6
            keys = dico_info_draw.keys()
            keys.sort()
            for name in keys:
                self.addDraw(name,dico_info_draw[name][0],dico_info_draw[name][1],dico_info_draw[name][2],dico_info_draw[name][3])
        else:
            log(3, "paramstatdens%s.txt not found for analysis %s"%(self.file_subname,self.analysis.name))


    def addDraw(self,name,values,absv,ordpr,ordpo):
        """ dessine et affiche un graphe
        """        
        tabvalues = values.strip().split('  ')
        av = float(tabvalues[0])
        median = float(tabvalues[1])
        mode = float(tabvalues[2])
        q2_5 = float(tabvalues[3])
        q5 = float(tabvalues[4])
        q25 = float(tabvalues[5])
        q75 = float(tabvalues[6])
        q95 = float(tabvalues[7])
        q975 = float(tabvalues[8])

        draw_widget = QtGui.QWidget(self)
        l = QtGui.QVBoxLayout(draw_widget)
        l.setContentsMargins(0,0,0,0)
        plotc = MyMplCanvas(draw_widget, width=12, height=4, dpi=100)
        l.addWidget(plotc)
        #plotc.fig.subplots_adjust(right=0.7,top=0.9,bottom=0.15)
        navtoolbar = NavigationToolbar(plotc, self)
        l.addWidget(navtoolbar)

        plotc.axes.grid(True)
        plotc.axes.set_title("%s [%6.2e]"%(name,median))

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
        plotc.axes.plot(labs,lpr,label=legend_txt,c=self.tab_colors[2])

        legend_txt = "posterior"
        plotc.axes.plot(labs,lpo,label=legend_txt,c=self.tab_colors[1])

        plotc.axes.legend(bbox_to_anchor=(0.74, -0.14),ncol=2,prop={'size':9})
        plotc.fig.subplots_adjust(left=0.17,right=0.92,top=0.85,bottom=0.27)
        plotc.axes.axes.title.set_fontsize(10)
        plotc.fig.patch.set_facecolor('white')
        for tick in plotc.axes.axes.xaxis.get_major_ticks():
                tick.label1.set_fontsize(7)
        for tick in plotc.axes.axes.yaxis.get_major_ticks():
                tick.label1.set_fontsize(7)

        fr = QFrame(self)
        fr.setFrameShape(QFrame.StyledPanel)
        fr.setFrameShadow(QFrame.Raised)
        fr.setObjectName("frameDraw")
        fr.setMinimumSize(QSize(400, 0))
        fr.setMaximumSize(QSize(400, 300))
        vert = QVBoxLayout(fr)
        vert.addWidget(draw_widget)

        self.ui.horizontalLayout_2.addWidget(fr)
        self.dicoPlot[name] = plotc

        # frame des valeurs
        frame = QFrame(self.ui.scrollAreaWidgetContents)

        self.dicoFrame[name] = frame

        frame.setFrameShape(QFrame.StyledPanel)
        frame.setFrameShadow(QFrame.Raised)
        frame.setObjectName("frameValues")
        verticalLayout_3 = QVBoxLayout(frame)
        verticalLayout_3.setObjectName("verticalLayout_3")
        avLabel = QLabel("Average   :  %6.2e"%av,frame)
        avLabel.setObjectName("avLabel")
        avLabel.setAlignment(Qt.AlignCenter)
        avLabel.setFont(QFont("Courrier",10))
        verticalLayout_3.addWidget(avLabel)
        medLabel = QLabel("Median    :  %6.2e"%median,frame)
        medLabel.setObjectName("medLabel")
        medLabel.setAlignment(Qt.AlignCenter)
        medLabel.setFont(QFont("Courrier",10))
        verticalLayout_3.addWidget(medLabel)
        modeLabel = QLabel("Mode       :  %6.2e"%mode,frame)
        modeLabel.setObjectName("modeLabel")
        modeLabel.setAlignment(Qt.AlignCenter)
        modeLabel.setFont(QFont("Courrier",10))
        verticalLayout_3.addWidget(modeLabel)
        q2_5Label = QLabel("q(0.025)  :  %6.2e"%q2_5,frame)
        q2_5Label.setObjectName("q2_5Label")
        q2_5Label.setAlignment(Qt.AlignCenter)
        q2_5Label.setFont(QFont("Courrier",10))
        verticalLayout_3.addWidget(q2_5Label)
        q5Label = QLabel("q(0.050)  :  %6.2e"%q5,frame)
        q5Label.setObjectName("q5Label")
        q5Label.setAlignment(Qt.AlignCenter)
        q5Label.setFont(QFont("Courrier",10))
        verticalLayout_3.addWidget(q5Label)
        q25Label = QLabel("q(0.250)  :  %6.2e"%q25,frame)
        q25Label.setObjectName("q25Label")
        q25Label.setAlignment(Qt.AlignCenter)
        q25Label.setFont(QFont("Courrier",10))
        verticalLayout_3.addWidget(q25Label)
        q75Label = QLabel("q(0.750)  :  %6.2e"%q75,frame)
        q75Label.setObjectName("q75")
        q75Label.setAlignment(Qt.AlignCenter)
        q75Label.setFont(QFont("Courrier",10))
        verticalLayout_3.addWidget(q75Label)
        q95Label = QLabel("q(0.950)  :  %6.2e"%q95,frame)
        q95Label.setObjectName("q95Label")
        q95Label.setAlignment(Qt.AlignCenter)
        q95Label.setFont(QFont("Courrier",10))
        verticalLayout_3.addWidget(q95Label)
        q975Label = QLabel("q(0.975)  :  %6.2e"%q975,frame)
        q975Label.setObjectName("q975Label")
        q975Label.setAlignment(Qt.AlignCenter)
        q975Label.setFont(QFont("Courrier",10))
        verticalLayout_3.addWidget(q975Label)
        frame.setMinimumSize(QSize(400, 0))
        frame.setMaximumSize(QSize(400, 9000))
        self.ui.horizontalLayout_3.addWidget(frame)

    def printDraws(self):
        nbpix = len(self.dicoPlot.keys())
        largeur = 2
        # resultat de la div entière plus le reste (modulo)
        longueur = (nbpix/largeur)+(nbpix%largeur)
        ind = 0
        li=0
        delta = 120

        im_result = QPrinter()
        im_result.setOutputFileName("%s/analysis/%s/%s.pdf"%(self.parent.dir,self.directory,self.analysis.name))
        painter = QPainter()

        dial = QPrintDialog(im_result,self)
        if dial.exec_():
            im_result.setOrientation(QPrinter.Portrait)
            im_result.setPageMargins(10,10,10,10,QPrinter.Millimeter)
            im_result.setResolution(100)

            painter.begin(im_result)

            keys = self.dicoPlot.keys()
            keys.sort()
            # on prend un des graphes pour savoir ses dimensions
            #size = self.dicoPlot[self.dicoPlot.keys()[0]].rect().size()

            # on fait des lignes tant qu'on a des pix
            while (ind < nbpix):
                col = 0
                # une ligne
                while (ind < nbpix) and (col < largeur):
                    plot = self.dicoPlot[keys[ind]]

                    bbox = plot.fig.bbox
                    canvas = plot.fig.canvas
                    w, h = int(bbox.width), int(bbox.height)
                    #l, t = bbox.ll().x().get(), bbox.ur().y().get()
                    reg = canvas.copy_from_bbox(bbox)
                    if QtCore.QSysInfo.ByteOrder == QtCore.QSysInfo.LittleEndian:
                        stringBuffer = canvas.renderer._renderer.tostring_bgra()
                    else:
                        stringBuffer = canvas.renderer._renderer.tostring_argb()
                    qImage = QtGui.QImage(stringBuffer, w, h, QtGui.QImage.Format_ARGB32)

                    #plot.print_(painter, QRect(QPoint(col*size.width(),li*size.height()),QSize(size)))
                    painter.drawImage(QRect(QPoint(col*w,(li*h)+(delta*li)),QPoint((col+1)*w,((li+1)*h)+(delta*li))),qImage)
                    if (ind+1)%6 == 0:
                        im_result.newPage()
                    col+=1
                    ind+=1
                if (ind)%6 == 0:
                    li = 0
                else:
                    li+=1

            painter.end()

    def saveDrawsToOnePdf(self):
        """ Sauve tous les scenarios dans un seul pdf en découpant à 6 scenarios par page
        """
        proj_dir = self.parent.dir
        pic_dir = "%s/analysis/%s"%(proj_dir,self.directory)
        pic_basename = "posterior"
        pic_whole_path = "%s/%s"%(pic_dir,pic_basename)

        if self.ui.oRadio.isChecked():
            file_subname = "_original"
        elif self.ui.cRadio.isChecked():
            file_subname = "_composite"
        elif self.ui.sRadio.isChecked():
            file_subname = "_scaled"

        nbpix = len(self.dicoPlot.keys())
        largeur = 2
        # resultat de la div entière plus le reste (modulo)
        longueur = (nbpix/largeur)+(nbpix%largeur)
        ind = 0
        li=0
        delta = 120

        self.im_result = QPrinter()
        self.im_result.setOutputFormat(QPrinter.PdfFormat)
        save_path = '%s%s_all.pdf'%(pic_whole_path,file_subname)
        self.im_result.setOutputFileName(save_path)
        painter = QPainter()
        self.im_result.setResolution(100)
        painter.begin(self.im_result)

        keys = self.dicoPlot.keys()
        # on prend un des graphes pour savoir ses dimensions
        #size = self.dicoPlot[self.dicoPlot.keys()[0]].rect().size()
        # on fait des lignes tant qu'on a des pix
        while (ind < nbpix):
            col = 0
            # une ligne
            while (ind < nbpix) and (col < largeur):
                if ind>0 and (ind)%6 == 0:
                    self.im_result.newPage()
                plot = self.dicoPlot[keys[ind]]

                bbox = plot.fig.bbox
                canvas = plot.fig.canvas
                w, h = int(bbox.width), int(bbox.height)
                #l, t = bbox.ll().x().get(), bbox.ur().y().get()
                reg = canvas.copy_from_bbox(bbox)
                if QtCore.QSysInfo.ByteOrder == QtCore.QSysInfo.LittleEndian:
                    stringBuffer = canvas.renderer._renderer.tostring_bgra()
                else:
                    stringBuffer = canvas.renderer._renderer.tostring_argb()
                qImage = QtGui.QImage(stringBuffer, w, h, QtGui.QImage.Format_ARGB32)

                #plot.print_(painter, QRect(QPoint(col*size.width(),li*size.height()),QSize(size)))
                painter.drawImage(QRect(QPoint(col*w,(li*h)+(delta*li)),QPoint((col+1)*w,((li+1)*h)+(delta*li))),qImage)

                #plot.print_(painter, QRect(QPoint(col*size.width(),li*size.height()),QSize(size)))
                col+=1
                ind+=1
            if (ind)%6 == 0:
                li = 0
            else:
                li+=1

        painter.end()
        output.notify(self,"pdf saved", 'The pdf has been saved in\n%s'%save_path)
