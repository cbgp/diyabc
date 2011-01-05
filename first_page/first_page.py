import sys
import time
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from first_page_ui import Ui_MainWindow
import os

class MainWindowTest(QMainWindow):
    def __init__(self,parent=None):
        super(MainWindowTest,self).__init__(parent)
        self.createWidgets()

    def createWidgets(self):
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        file_menu = self.ui.menubar.addMenu("File")
        file_menu.addAction("Open",self.file_open,QKeySequence(Qt.CTRL + Qt.Key_O))
        action = file_menu.addAction("Quit",self.close,QKeySequence(Qt.CTRL + Qt.Key_Q))
	
	#mettre plusieurs raccourcis claviers pour le meme menu
	#action.setShortcuts([QKeySequence(Qt.CTRL + Qt.Key_Q),QKeySequence(Qt.Key_Escape)])
        QObject.connect(self.ui.newAnButton,SIGNAL("clicked()"),self.addRow)

        for i in range(self.ui.tableWidget.columnCount()):
            for j in range(self.ui.tableWidget.rowCount()):
                it = QTableWidgetItem("%i,%i"%(j,i))
                self.ui.tableWidget.setItem(j,i,it)

    def addRow(self):
        print self.ui.tableWidget.rowCount()
        self.ui.tableWidget.insertRow(self.ui.tableWidget.rowCount())
        print self.ui.tableWidget.rowCount()
        for i in range(self.ui.tableWidget.columnCount()):
            self.ui.tableWidget.setItem(self.ui.tableWidget.rowCount(),i,QTableWidgetItem("new"))
            print str(self.ui.tableWidget.rowCount()-1)+","+str(i)
            #self.ui.tableWidget.item(self.ui.tableWidget.rowCount()-1,i).setText("new")
        #self.ui.tableWidget.insertRow(2)

    def file_open(self):
        pass

        #QObject.connect(self.ui.lcdNumber,SIGNAL("sliderMoved(int)"),self.maj_prog)
    def closeEvent(self, event):
        reply = QtGui.QMessageBox.question(self, 'Message',
            "Are you sure to quit?", QtGui.QMessageBox.Yes | 
            QtGui.QMessageBox.No, QtGui.QMessageBox.Yes)

        if reply == QtGui.QMessageBox.Yes:
            event.accept()
        else:
            event.ignore()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    myapp = MainWindowTest()
    myapp.show()
    sys.exit(app.exec_())


