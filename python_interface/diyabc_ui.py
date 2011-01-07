# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'uis/diyabc.ui'
#
# Created: Fri Jan  7 16:48:37 2011
#      by: PyQt4 UI code generator 4.6
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(900, 700)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(MainWindow.sizePolicy().hasHeightForWidth())
        MainWindow.setSizePolicy(sizePolicy)
        MainWindow.setMinimumSize(QtCore.QSize(800, 700))
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.gridLayout_2 = QtGui.QGridLayout(self.centralwidget)
        self.gridLayout_2.setObjectName("gridLayout_2")
        self.gridLayout = QtGui.QGridLayout()
        self.gridLayout.setObjectName("gridLayout")
        self.tabWidget = QtGui.QTabWidget(self.centralwidget)
        self.tabWidget.setTabPosition(QtGui.QTabWidget.West)
        self.tabWidget.setTabShape(QtGui.QTabWidget.Triangular)
        self.tabWidget.setDocumentMode(False)
        self.tabWidget.setTabsClosable(True)
        self.tabWidget.setMovable(True)
        self.tabWidget.setObjectName("tabWidget")
        self.tab_6 = QtGui.QWidget()
        self.tab_6.setObjectName("tab_6")
        self.verticalLayout = QtGui.QVBoxLayout(self.tab_6)
        self.verticalLayout.setObjectName("verticalLayout")
        self.tabWidget.addTab(self.tab_6, "")
        self.gridLayout.addWidget(self.tabWidget, 0, 0, 1, 1)
        self.gridLayout_2.addLayout(self.gridLayout, 2, 0, 1, 1)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 900, 25))
        self.menubar.setObjectName("menubar")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)
        self.toolBar = QtGui.QToolBar(MainWindow)
        self.toolBar.setObjectName("toolBar")
        MainWindow.addToolBar(QtCore.Qt.TopToolBarArea, self.toolBar)
        self.actionWeb = QtGui.QAction(MainWindow)
        self.actionWeb.setCheckable(True)
        self.actionWeb.setObjectName("actionWeb")
        self.action = QtGui.QAction(MainWindow)
        self.action.setCheckable(True)
        self.action.setObjectName("action")
        self.actionLoop = QtGui.QAction(MainWindow)
        self.actionLoop.setCheckable(True)
        self.actionLoop.setObjectName("actionLoop")
        self.actionOngsuiv = QtGui.QAction(MainWindow)
        self.actionOngsuiv.setObjectName("actionOngsuiv")
        self.actionOngprec = QtGui.QAction(MainWindow)
        self.actionOngprec.setObjectName("actionOngprec")

        self.retranslateUi(MainWindow)
        self.tabWidget.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QtGui.QApplication.translate("MainWindow", "DIYABC", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_6), QtGui.QApplication.translate("MainWindow", "Projet 2", None, QtGui.QApplication.UnicodeUTF8))
        self.toolBar.setWindowTitle(QtGui.QApplication.translate("MainWindow", "toolBar", None, QtGui.QApplication.UnicodeUTF8))
        self.actionWeb.setText(QtGui.QApplication.translate("MainWindow", "activer web", None, QtGui.QApplication.UnicodeUTF8))
        self.actionWeb.setToolTip(QtGui.QApplication.translate("MainWindow", "obvious", None, QtGui.QApplication.UnicodeUTF8))
        self.action.setText(QtGui.QApplication.translate("MainWindow", "plap", None, QtGui.QApplication.UnicodeUTF8))
        self.action.setToolTip(QtGui.QApplication.translate("MainWindow", "hohoho", None, QtGui.QApplication.UnicodeUTF8))
        self.actionLoop.setText(QtGui.QApplication.translate("MainWindow", "loop", None, QtGui.QApplication.UnicodeUTF8))
        self.actionLoop.setToolTip(QtGui.QApplication.translate("MainWindow", "c\'est la vie!", None, QtGui.QApplication.UnicodeUTF8))
        self.actionLoop.setShortcut(QtGui.QApplication.translate("MainWindow", "Ctrl+Alt+L", None, QtGui.QApplication.UnicodeUTF8))
        self.actionOngsuiv.setText(QtGui.QApplication.translate("MainWindow", "ongsuiv", None, QtGui.QApplication.UnicodeUTF8))
        self.actionOngsuiv.setShortcut(QtGui.QApplication.translate("MainWindow", "Ctrl+PgDown", None, QtGui.QApplication.UnicodeUTF8))
        self.actionOngprec.setText(QtGui.QApplication.translate("MainWindow", "ongprec", None, QtGui.QApplication.UnicodeUTF8))
        self.actionOngprec.setShortcut(QtGui.QApplication.translate("MainWindow", "Ctrl+PgUp", None, QtGui.QApplication.UnicodeUTF8))

