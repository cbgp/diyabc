import sys
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from test import Ui_GroupBox
import os

class plop(QGroupBox):
    """ comme  nt"""
    def __init__(self,parent=None):
        super(plop,self).__init__(parent)
        self.createWidgets()
        self.dico = {}

    def createWidgets(self):
        self.ui = Ui_GroupBox()
        self.ui.setupUi(self)

        #QObject.connect(self.ui.pushButton,SIGNAL("clicked()"),os.system('echo "'+str(self.ui.lineEdit.displayText())+'" >> /home/julien/tuto/plopppp'))
        QObject.connect(self.ui.pushButton,SIGNAL("clicked()"),self.consult)
        QObject.connect(self.ui.pushButton_2,SIGNAL("clicked()"),self.pouche)
        QObject.connect(self.ui.lineEdit,SIGNAL("returnPressed()"),self.consult)
        QObject.connect(self.ui.lcdNumber,SIGNAL("sliderMoved(int)"),self.maj_prog)

    def pouche(self):
        #self.ui.lineEdit_2.setText(self.ui.lineEdit.text())
        self.dico[self.ui.lineEdit.text()] = self.ui.lineEdit_2.text()
        if self.ui.progressBar.value() > 90:
            self.ui.progressBar.setValue(0)
        self.ui.progressBar.setValue(self.ui.progressBar.value() + 10)
        qfd = QFileDialog()
        qfd.setDirectory("/home/julien/Desktop/")
        name = qfd.getOpenFileName()
        self.ui.lineEdit_2.setText(name)
        name = qfd.getOpenFileName()
        self.ui.lineEdit.setText(name)
        plop = QMenu()



    def consult(self):
        #self.ui.lineEdit_2.setText(self.dico[self.ui.lineEdit.text()])
        a=os.popen("%s" % self.ui.lineEdit.text())
        b=a.read()
        self.ui.lineEdit_2.setText(b)
        a.close() 

    def maj_prog(self,n):
        self.ui.progressBar.setValue(n)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    myapp = plop()
    myapp.show()
    sys.exit(app.exec_())


