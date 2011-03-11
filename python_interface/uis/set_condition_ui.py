# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'uis/setCondition.ui'
#
# Created: Thu Jan 13 18:22:04 2011
#      by: PyQt4 UI code generator 4.6
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.setWindowModality(QtCore.Qt.WindowModal)
        MainWindow.resize(341, 172)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.gridLayout = QtGui.QGridLayout(self.centralwidget)
        self.gridLayout.setObjectName("gridLayout")
        self.frame = QtGui.QFrame(self.centralwidget)
        self.frame.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame.setFrameShadow(QtGui.QFrame.Raised)
        self.frame.setObjectName("frame")
        self.verticalLayout = QtGui.QVBoxLayout(self.frame)
        self.verticalLayout.setObjectName("verticalLayout")
        self.paramLabel = QtGui.QLabel(self.frame)
        self.paramLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.paramLabel.setObjectName("paramLabel")
        self.verticalLayout.addWidget(self.paramLabel)
        self.gridLayout.addWidget(self.frame, 0, 0, 1, 1)
        self.frame_3 = QtGui.QFrame(self.centralwidget)
        self.frame_3.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame_3.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_3.setObjectName("frame_3")
        self.verticalLayout_2 = QtGui.QVBoxLayout(self.frame_3)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.targetComboBox = QtGui.QComboBox(self.frame_3)
        self.targetComboBox.setObjectName("targetComboBox")
        self.verticalLayout_2.addWidget(self.targetComboBox)
        self.gridLayout.addWidget(self.frame_3, 0, 2, 1, 1)
        self.validateButton = QtGui.QPushButton(self.centralwidget)
        self.validateButton.setObjectName("validateButton")
        self.gridLayout.addWidget(self.validateButton, 1, 0, 1, 1)
        self.cancelButton = QtGui.QPushButton(self.centralwidget)
        self.cancelButton.setObjectName("cancelButton")
        self.gridLayout.addWidget(self.cancelButton, 1, 2, 1, 1)
        self.frame_2 = QtGui.QFrame(self.centralwidget)
        self.frame_2.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame_2.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_2.setObjectName("frame_2")
        self.gridLayout_2 = QtGui.QGridLayout(self.frame_2)
        self.gridLayout_2.setObjectName("gridLayout_2")
        self.supRadio = QtGui.QRadioButton(self.frame_2)
        self.supRadio.setChecked(True)
        self.supRadio.setObjectName("supRadio")
        self.gridLayout_2.addWidget(self.supRadio, 0, 0, 1, 2)
        self.supERadio = QtGui.QRadioButton(self.frame_2)
        self.supERadio.setObjectName("supERadio")
        self.gridLayout_2.addWidget(self.supERadio, 0, 2, 1, 1)
        self.infERadio = QtGui.QRadioButton(self.frame_2)
        self.infERadio.setObjectName("infERadio")
        self.gridLayout_2.addWidget(self.infERadio, 1, 2, 1, 1)
        self.infRadio = QtGui.QRadioButton(self.frame_2)
        self.infRadio.setObjectName("infRadio")
        self.gridLayout_2.addWidget(self.infRadio, 1, 0, 1, 1)
        self.gridLayout.addWidget(self.frame_2, 0, 1, 1, 1)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 341, 25))
        self.menubar.setObjectName("menubar")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QtGui.QApplication.translate("MainWindow", "MainWindow", None, QtGui.QApplication.UnicodeUTF8))
        self.paramLabel.setText(QtGui.QApplication.translate("MainWindow", "N", None, QtGui.QApplication.UnicodeUTF8))
        self.validateButton.setText(QtGui.QApplication.translate("MainWindow", "VALIDATE", None, QtGui.QApplication.UnicodeUTF8))
        self.cancelButton.setText(QtGui.QApplication.translate("MainWindow", "CANCEL", None, QtGui.QApplication.UnicodeUTF8))
        self.supRadio.setText(QtGui.QApplication.translate("MainWindow", ">", None, QtGui.QApplication.UnicodeUTF8))
        self.supERadio.setText(QtGui.QApplication.translate("MainWindow", ">=", None, QtGui.QApplication.UnicodeUTF8))
        self.infERadio.setText(QtGui.QApplication.translate("MainWindow", "<=", None, QtGui.QApplication.UnicodeUTF8))
        self.infRadio.setText(QtGui.QApplication.translate("MainWindow", "<", None, QtGui.QApplication.UnicodeUTF8))

