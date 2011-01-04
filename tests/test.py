# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'groupbox.ui'
#
# Created: Tue Nov 30 15:45:27 2010
#      by: PyQt4 UI code generator 4.7.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_GroupBox(object):
    def setupUi(self, GroupBox):
        GroupBox.setObjectName("GroupBox")
        GroupBox.resize(398, 415)
        self.gridLayout_2 = QtGui.QGridLayout(GroupBox)
        self.gridLayout_2.setObjectName("gridLayout_2")
        self.gridLayout = QtGui.QGridLayout()
        self.gridLayout.setContentsMargins(-1, -1, 21, -1)
        self.gridLayout.setObjectName("gridLayout")
        self.horizontalSlider = QtGui.QSlider(GroupBox)
        self.horizontalSlider.setOrientation(QtCore.Qt.Horizontal)
        self.horizontalSlider.setObjectName("horizontalSlider")
        self.gridLayout.addWidget(self.horizontalSlider, 1, 0, 1, 1)
        self.lcdNumber = QtGui.QLCDNumber(GroupBox)
        self.lcdNumber.setObjectName("lcdNumber")
        self.gridLayout.addWidget(self.lcdNumber, 1, 1, 1, 1)
        self.pushButton = QtGui.QPushButton(GroupBox)
        self.pushButton.setMinimumSize(QtCore.QSize(129, 26))
        self.pushButton.setCursor(QtCore.Qt.CrossCursor)
        self.pushButton.setAutoDefault(False)
        self.pushButton.setDefault(False)
        self.pushButton.setFlat(False)
        self.pushButton.setObjectName("pushButton")
        self.gridLayout.addWidget(self.pushButton, 2, 0, 1, 1)
        self.pushButton_2 = QtGui.QPushButton(GroupBox)
        self.pushButton_2.setObjectName("pushButton_2")
        self.gridLayout.addWidget(self.pushButton_2, 2, 1, 1, 1)
        self.pushButton_3 = QtGui.QPushButton(GroupBox)
        self.pushButton_3.setObjectName("pushButton_3")
        self.gridLayout.addWidget(self.pushButton_3, 2, 2, 1, 1)
        self.lineEdit = QtGui.QLineEdit(GroupBox)
        self.lineEdit.setObjectName("lineEdit")
        self.gridLayout.addWidget(self.lineEdit, 0, 1, 1, 1)
        self.lineEdit_2 = QtGui.QLineEdit(GroupBox)
        self.lineEdit_2.setObjectName("lineEdit_2")
        self.gridLayout.addWidget(self.lineEdit_2, 0, 2, 1, 1)
        self.progressBar = QtGui.QProgressBar(GroupBox)
        self.progressBar.setProperty("value", 24)
        self.progressBar.setObjectName("progressBar")
        self.gridLayout.addWidget(self.progressBar, 0, 0, 1, 1)
        self.gridLayout_2.addLayout(self.gridLayout, 0, 0, 1, 2)
        self.toolButton = QtGui.QToolButton(GroupBox)
        self.toolButton.setObjectName("toolButton")
        self.gridLayout_2.addWidget(self.toolButton, 1, 0, 1, 1)
        self.toolBox = QtGui.QToolBox(GroupBox)
        self.toolBox.setObjectName("toolBox")
        self.page = QtGui.QWidget()
        self.page.setGeometry(QtCore.QRect(0, 0, 343, 226))
        self.page.setObjectName("page")
        self.toolBox.addItem(self.page, "")
        self.page_2 = QtGui.QWidget()
        self.page_2.setGeometry(QtCore.QRect(0, 0, 71, 71))
        self.page_2.setObjectName("page_2")
        self.toolBox.addItem(self.page_2, "")
        self.gridLayout_2.addWidget(self.toolBox, 1, 1, 1, 1)

        self.retranslateUi(GroupBox)
        self.toolBox.setCurrentIndex(0)
        QtCore.QObject.connect(self.horizontalSlider, QtCore.SIGNAL("sliderMoved(int)"), self.lcdNumber.display)
        QtCore.QObject.connect(self.pushButton_3, QtCore.SIGNAL("pressed()"), GroupBox.close)
        QtCore.QMetaObject.connectSlotsByName(GroupBox)

    def retranslateUi(self, GroupBox):
        GroupBox.setWindowTitle(QtGui.QApplication.translate("GroupBox", "GroupBox", None, QtGui.QApplication.UnicodeUTF8))
        GroupBox.setTitle(QtGui.QApplication.translate("GroupBox", "GroupBox", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton.setText(QtGui.QApplication.translate("GroupBox", "consult", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_2.setText(QtGui.QApplication.translate("GroupBox", "push", None, QtGui.QApplication.UnicodeUTF8))
        self.pushButton_3.setText(QtGui.QApplication.translate("GroupBox", "quit", None, QtGui.QApplication.UnicodeUTF8))
        self.toolButton.setText(QtGui.QApplication.translate("GroupBox", "...", None, QtGui.QApplication.UnicodeUTF8))
        self.toolBox.setItemText(self.toolBox.indexOf(self.page), QtGui.QApplication.translate("GroupBox", "Page 1", None, QtGui.QApplication.UnicodeUTF8))
        self.toolBox.setItemText(self.toolBox.indexOf(self.page_2), QtGui.QApplication.translate("GroupBox", "Page 2", None, QtGui.QApplication.UnicodeUTF8))

