# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'uis/comparison.ui'
#
# Created: Mon Mar 21 14:59:20 2011
#      by: PyQt4 UI code generator 4.8.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_Frame(object):
    def setupUi(self, Frame):
        Frame.setObjectName(_fromUtf8("Frame"))
        Frame.resize(825, 597)
        Frame.setFrameShape(QtGui.QFrame.StyledPanel)
        Frame.setFrameShadow(QtGui.QFrame.Raised)
        self.verticalLayout_2 = QtGui.QVBoxLayout(Frame)
        self.verticalLayout_2.setObjectName(_fromUtf8("verticalLayout_2"))
        self.frame_6 = QtGui.QFrame(Frame)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.frame_6.sizePolicy().hasHeightForWidth())
        self.frame_6.setSizePolicy(sizePolicy)
        self.frame_6.setMinimumSize(QtCore.QSize(0, 55))
        self.frame_6.setMaximumSize(QtCore.QSize(16777215, 55))
        self.frame_6.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame_6.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_6.setObjectName(_fromUtf8("frame_6"))
        self.horizontalLayout_9 = QtGui.QHBoxLayout(self.frame_6)
        self.horizontalLayout_9.setObjectName(_fromUtf8("horizontalLayout_9"))
        self.exitButton = QtGui.QPushButton(self.frame_6)
        font = QtGui.QFont()
        font.setPointSize(14)
        self.exitButton.setFont(font)
        self.exitButton.setObjectName(_fromUtf8("exitButton"))
        self.horizontalLayout_9.addWidget(self.exitButton)
        self.okButton = QtGui.QPushButton(self.frame_6)
        font = QtGui.QFont()
        font.setPointSize(14)
        self.okButton.setFont(font)
        self.okButton.setObjectName(_fromUtf8("okButton"))
        self.horizontalLayout_9.addWidget(self.okButton)
        self.verticalLayout_2.addWidget(self.frame_6)
        self.label = QtGui.QLabel(Frame)
        self.label.setMinimumSize(QtCore.QSize(0, 40))
        self.label.setMaximumSize(QtCore.QSize(16777215, 40))
        font = QtGui.QFont()
        font.setPointSize(15)
        self.label.setFont(font)
        self.label.setAlignment(QtCore.Qt.AlignCenter)
        self.label.setObjectName(_fromUtf8("label"))
        self.verticalLayout_2.addWidget(self.label)
        self.horizontalLayout_3 = QtGui.QHBoxLayout()
        self.horizontalLayout_3.setObjectName(_fromUtf8("horizontalLayout_3"))
        self.label_6 = QtGui.QLabel(Frame)
        self.label_6.setMinimumSize(QtCore.QSize(130, 0))
        self.label_6.setMaximumSize(QtCore.QSize(130, 16777215))
        self.label_6.setObjectName(_fromUtf8("label_6"))
        self.horizontalLayout_3.addWidget(self.label_6)
        self.projectNameEdit = QtGui.QLineEdit(Frame)
        self.projectNameEdit.setReadOnly(True)
        self.projectNameEdit.setObjectName(_fromUtf8("projectNameEdit"))
        self.horizontalLayout_3.addWidget(self.projectNameEdit)
        self.verticalLayout_2.addLayout(self.horizontalLayout_3)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.verticalLayout = QtGui.QVBoxLayout()
        self.verticalLayout.setContentsMargins(-1, -1, 10, -1)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.scenariosLabel = QtGui.QLabel(Frame)
        self.scenariosLabel.setMinimumSize(QtCore.QSize(0, 20))
        self.scenariosLabel.setMaximumSize(QtCore.QSize(16777215, 20))
        font = QtGui.QFont()
        font.setPointSize(10)
        self.scenariosLabel.setFont(font)
        self.scenariosLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.scenariosLabel.setObjectName(_fromUtf8("scenariosLabel"))
        self.horizontalLayout_2.addWidget(self.scenariosLabel)
        self.redefButton = QtGui.QPushButton(Frame)
        self.redefButton.setMinimumSize(QtCore.QSize(100, 0))
        self.redefButton.setMaximumSize(QtCore.QSize(100, 16777215))
        self.redefButton.setObjectName(_fromUtf8("redefButton"))
        self.horizontalLayout_2.addWidget(self.redefButton)
        self.verticalLayout.addLayout(self.horizontalLayout_2)
        self.label_2 = QtGui.QLabel(Frame)
        self.label_2.setAlignment(QtCore.Qt.AlignCenter)
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.verticalLayout.addWidget(self.label_2)
        self.label_3 = QtGui.QLabel(Frame)
        font = QtGui.QFont()
        font.setPointSize(7)
        self.label_3.setFont(font)
        self.label_3.setAlignment(QtCore.Qt.AlignCenter)
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.verticalLayout.addWidget(self.label_3)
        self.gridLayout_2 = QtGui.QGridLayout()
        self.gridLayout_2.setObjectName(_fromUtf8("gridLayout_2"))
        self.label_4 = QtGui.QLabel(Frame)
        self.label_4.setMinimumSize(QtCore.QSize(140, 0))
        self.label_4.setMaximumSize(QtCore.QSize(140, 16777215))
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.gridLayout_2.addWidget(self.label_4, 0, 0, 1, 1)
        self.deEdit = QtGui.QLineEdit(Frame)
        self.deEdit.setMinimumSize(QtCore.QSize(100, 0))
        self.deEdit.setMaximumSize(QtCore.QSize(100, 16777215))
        self.deEdit.setObjectName(_fromUtf8("deEdit"))
        self.gridLayout_2.addWidget(self.deEdit, 0, 1, 1, 2)
        self.label_5 = QtGui.QLabel(Frame)
        self.label_5.setMinimumSize(QtCore.QSize(140, 0))
        self.label_5.setMaximumSize(QtCore.QSize(140, 16777215))
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.gridLayout_2.addWidget(self.label_5, 1, 0, 1, 1)
        self.lrEdit = QtGui.QLineEdit(Frame)
        self.lrEdit.setMinimumSize(QtCore.QSize(100, 0))
        self.lrEdit.setMaximumSize(QtCore.QSize(100, 16777215))
        self.lrEdit.setObjectName(_fromUtf8("lrEdit"))
        self.gridLayout_2.addWidget(self.lrEdit, 1, 1, 1, 1)
        self.comboBox = QtGui.QComboBox(Frame)
        self.comboBox.setMinimumSize(QtCore.QSize(100, 0))
        self.comboBox.setMaximumSize(QtCore.QSize(100, 16777215))
        self.comboBox.setObjectName(_fromUtf8("comboBox"))
        self.gridLayout_2.addWidget(self.comboBox, 1, 2, 1, 1)
        self.verticalLayout.addLayout(self.gridLayout_2)
        self.horizontalLayout.addLayout(self.verticalLayout)
        self.line = QtGui.QFrame(Frame)
        self.line.setFrameShape(QtGui.QFrame.VLine)
        self.line.setFrameShadow(QtGui.QFrame.Sunken)
        self.line.setObjectName(_fromUtf8("line"))
        self.horizontalLayout.addWidget(self.line)
        self.gridLayout = QtGui.QGridLayout()
        self.gridLayout.setContentsMargins(10, -1, -1, -1)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.label_7 = QtGui.QLabel(Frame)
        self.label_7.setMinimumSize(QtCore.QSize(220, 0))
        self.label_7.setMaximumSize(QtCore.QSize(220, 16777215))
        self.label_7.setObjectName(_fromUtf8("label_7"))
        self.gridLayout.addWidget(self.label_7, 3, 0, 1, 3)
        self.label_8 = QtGui.QLabel(Frame)
        self.label_8.setMinimumSize(QtCore.QSize(220, 0))
        self.label_8.setMaximumSize(QtCore.QSize(220, 16777215))
        self.label_8.setObjectName(_fromUtf8("label_8"))
        self.gridLayout.addWidget(self.label_8, 4, 0, 1, 3)
        self.totNumSimEdit = QtGui.QLineEdit(Frame)
        self.totNumSimEdit.setMinimumSize(QtCore.QSize(100, 0))
        self.totNumSimEdit.setMaximumSize(QtCore.QSize(100, 16777215))
        self.totNumSimEdit.setObjectName(_fromUtf8("totNumSimEdit"))
        self.gridLayout.addWidget(self.totNumSimEdit, 3, 3, 1, 1)
        self.cnosdEdit = QtGui.QLineEdit(Frame)
        self.cnosdEdit.setMinimumSize(QtCore.QSize(100, 0))
        self.cnosdEdit.setMaximumSize(QtCore.QSize(100, 16777215))
        self.cnosdEdit.setObjectName(_fromUtf8("cnosdEdit"))
        self.gridLayout.addWidget(self.cnosdEdit, 4, 3, 1, 1)
        self.horizontalLayout.addLayout(self.gridLayout)
        self.verticalLayout_2.addLayout(self.horizontalLayout)

        self.retranslateUi(Frame)
        QtCore.QMetaObject.connectSlotsByName(Frame)

    def retranslateUi(self, Frame):
        Frame.setWindowTitle(QtGui.QApplication.translate("Frame", "Frame", None, QtGui.QApplication.UnicodeUTF8))
        self.exitButton.setToolTip(QtGui.QApplication.translate("Frame", "Leave this screen without loosing any change", None, QtGui.QApplication.UnicodeUTF8))
        self.exitButton.setText(QtGui.QApplication.translate("Frame", "CANCEL", None, QtGui.QApplication.UnicodeUTF8))
        self.exitButton.setShortcut(QtGui.QApplication.translate("Frame", "Alt+E", None, QtGui.QApplication.UnicodeUTF8))
        self.okButton.setToolTip(QtGui.QApplication.translate("Frame", "Test and save data (Alt+o)", None, QtGui.QApplication.UnicodeUTF8))
        self.okButton.setText(QtGui.QApplication.translate("Frame", "VALIDATE", None, QtGui.QApplication.UnicodeUTF8))
        self.okButton.setShortcut(QtGui.QApplication.translate("Frame", "Alt+V", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Frame", "Comparison of scenarios", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("Frame", "Project directory : ", None, QtGui.QApplication.UnicodeUTF8))
        self.scenariosLabel.setText(QtGui.QApplication.translate("Frame", "TextLabel", None, QtGui.QApplication.UnicodeUTF8))
        self.redefButton.setText(QtGui.QApplication.translate("Frame", "Redefine", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("Frame", "Numer of selected data", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Frame", "(simulated data closest to observed)", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("Frame", "Direct Estimate", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("Frame", "Logistic Regression", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("Frame", "Total number of simulated data :", None, QtGui.QApplication.UnicodeUTF8))
        self.label_8.setText(QtGui.QApplication.translate("Frame", "Chosen number of simulated data : ", None, QtGui.QApplication.UnicodeUTF8))

