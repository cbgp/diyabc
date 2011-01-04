import sys
import time
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from mainwindow import Ui_MainWindow
import os

class MainWindowTest(QMainWindow):
    def __init__(self,parent=None):
        super(MainWindowTest,self).__init__(parent)
        self.createWidgets()

    def createWidgets(self):
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.looping = False

        file_menu = self.ui.menubar.addMenu("File")
        file_menu.addAction("Open",self.file_open,QKeySequence(Qt.CTRL + Qt.Key_O))
        action = file_menu.addAction("Quit",self.close,QKeySequence(Qt.CTRL + Qt.Key_Q))
	
	#mettre plusieurs raccourcis claviers pour le meme menu
	#action.setShortcuts([QKeySequence(Qt.CTRL + Qt.Key_Q),QKeySequence(Qt.Key_Escape)])

        self.ui.webView.setUrl(QUrl("http://weboob.org"))

        #QObject.connect(self.ui.lcdNumber,SIGNAL("sliderMoved(int)"),self.maj_prog)
        QObject.connect(self.ui.actionWeb,SIGNAL("toggled(bool)"),self.gerer_web)
        QObject.connect(self.ui.actionLoop,SIGNAL("toggled(bool)"),self.gerer_loop)
        self.ui.tabWidget.setMovable(True)
        self.ui.tabWidget.setTabsClosable(True)
        QObject.connect(self.ui.actionOngsuiv,SIGNAL("triggered(bool)"),self.ongsuiv)
        QObject.connect(self.ui.actionOngprec,SIGNAL("triggered(bool)"),self.ongprec)
        QObject.connect(self.ui.lineEdit_2,SIGNAL("returnPressed()"),self.web2)
        QObject.connect(self.ui.pushButton_2,SIGNAL("clicked()"),self.stPrec)
        QObject.connect(self.ui.pushButton_3,SIGNAL("clicked()"),self.stSuiv)

        self.ui.webView.hide()

        splitter1 = QtGui.QSplitter(Qt.Horizontal)
        splitter1.addWidget(QPushButton())
        splitter1.addWidget(QLineEdit())
        self.ui.gridLayout.addWidget(splitter1)

        thr = mythread(self)
        thr.start()

    # se deplacer dans la stack
    def stPrec(self):
        self.ui.stackedWidget.setCurrentIndex((self.ui.stackedWidget.currentIndex() - 1)% self.ui.stackedWidget.count())

    def stSuiv(self):
        self.ui.stackedWidget.setCurrentIndex((self.ui.stackedWidget.currentIndex() + 1)% self.ui.stackedWidget.count())

    def ongsuiv(self):
        self.ui.tabWidget.setCurrentIndex((self.ui.tabWidget.currentIndex() + 1) % self.ui.tabWidget.count())

    def ongprec(self):
        self.ui.tabWidget.setCurrentIndex((self.ui.tabWidget.currentIndex() - 1) % self.ui.tabWidget.count())

    def web2(self):
        self.ui.webView_2.setUrl(QUrl(self.ui.lineEdit_2.text()))

    def gerer_web(self,on):
        if on:
            self.ui.webView.show()
        else:
            self.ui.webView.hide()

    def gerer_loop(self,on):
        if on:
            self.looping = True
        else:
            self.looping = False

    def file_open_pass(self):
        text, ok = QtGui.QInputDialog.getText(self, 'Input Dialog', 'Enter the password to open sesame:',mode=QLineEdit.Password)

        if ok:
            if str(text) == "kaizer":
                qfd = QFileDialog()
                qfd.setDirectory("~/")
                name = qfd.getOpenFileName()
                self.ui.lineEdit.setText(name)

    def file_open(self):
        print self.ui.webView.url()
        #qfd = QFileDialog()
        #qfd.setDirectory("~/")
        text, ok = QtGui.QInputDialog.getText(self, 'la parole aux users', 'choix du basedir',mode=QLineEdit.PasswordEchoOnEdit)

        if ok:
            name = QFileDialog.getOpenFileName(self,"bon choix",str(text))
        else:
            name = QFileDialog.getOpenFileName(self,"basedir = pwd")
        self.ui.lineEdit.setText(str(name))

    def closeEvent(self, event):
        reply = QtGui.QMessageBox.question(self, 'Message',
            "Are you sure to quit?", QtGui.QMessageBox.Yes | 
            QtGui.QMessageBox.No, QtGui.QMessageBox.Yes)

        if reply == QtGui.QMessageBox.Yes:
            event.accept()
        else:
            event.ignore()

               
class mythread(QThread):
    def __init__(self,parent=None):
        QThread.__init__(self,parent)
        self.test = ''
        self.parent=parent

    def run(self):
        while 1:
            time.sleep(2)
            if self.parent.looping:
                self.parent.ui.actionWeb.toggle()
                self.parent.ui.calendarWidget.setSelectedDate(self.parent.ui.calendarWidget.selectedDate().addDays(1))


if __name__ == "__main__":
    app = QApplication(sys.argv)
    myapp = MainWindowTest()
    myapp.show()
    sys.exit(app.exec_())


