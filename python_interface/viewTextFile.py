import sys
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtSvg import *
from PyQt4 import uic
from utils.cbgpUtils import log

formTF,baseTF = uic.loadUiType("uis/viewTextFile.ui")

class ViewTextFile(formTF,baseTF):
    def __init__(self,text,okFunction,parent=None):
        super(ViewTextFile,self).__init__(parent)
        self.parent = parent
        self.text = text
        self.okFunction = okFunction
        self.ui = self
        self.ui.setupUi(self)
        self.displayText()

        QObject.connect(self.ui.okButton,SIGNAL("clicked()"),self.okFunction)
        QObject.connect(self.ui.saveButton,SIGNAL("clicked()"),self.save)
        QObject.connect(self.ui.printButton,SIGNAL("clicked()"),self.printMe)

    def displayText(self):
        self.ui.dataPlain.setPlainText(self.text)
        self.ui.dataPlain.setLineWrapMode(0)
        font = "FreeMono"
        if sys.platform.startswith('win'):
            font = "Courier New"
        elif "darwin" in sys.platform:
            font = "Andale Mono"
        self.ui.dataPlain.setFont(QFont(font,10))

    def printMe(self):
        im_result = QPrinter()
        dial = QPrintDialog(im_result,self)
        if dial.exec_():
            painter = QPainter()
            #im_result.setOrientation(QPrinter.Landscape)
            im_result.setPageMargins(20,20,20,20,QPrinter.Millimeter)
            # TODO changer taille
            im_result.setResolution(80)
            self.ui.dataPlain.setLineWrapMode(1)
            self.ui.dataPlain.print_(im_result)
            self.ui.dataPlain.setLineWrapMode(0)
            #painter.begin(im_result)
            #pen = QPen(Qt.black,2)
            #painter.setPen(pen)
            #painter.drawText(20,20,self.text)
            #painter.end()

    def save(self):
        pass

