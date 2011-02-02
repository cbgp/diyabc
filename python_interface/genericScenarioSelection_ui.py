# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'uis/genericScenarioSelection.ui'
#
# Created: Wed Feb  2 14:56:49 2011
#      by: PyQt4 UI code generator 4.7.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_Frame(object):
    def setupUi(self, Frame):
        Frame.setObjectName("Frame")
        Frame.resize(768, 474)
        Frame.setFrameShape(QtGui.QFrame.StyledPanel)
        Frame.setFrameShadow(QtGui.QFrame.Raised)
        self.verticalLayout_2 = QtGui.QVBoxLayout(Frame)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
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
        self.frame_6.setObjectName("frame_6")
        self.horizontalLayout_9 = QtGui.QHBoxLayout(self.frame_6)
        self.horizontalLayout_9.setObjectName("horizontalLayout_9")
        self.exitButton = QtGui.QPushButton(self.frame_6)
        font = QtGui.QFont()
        font.setPointSize(14)
        self.exitButton.setFont(font)
        self.exitButton.setObjectName("exitButton")
        self.horizontalLayout_9.addWidget(self.exitButton)
        self.okButton = QtGui.QPushButton(self.frame_6)
        font = QtGui.QFont()
        font.setPointSize(14)
        self.okButton.setFont(font)
        self.okButton.setObjectName("okButton")
        self.horizontalLayout_9.addWidget(self.okButton)
        self.verticalLayout_2.addWidget(self.frame_6)
        self.selectionLabel = QtGui.QLabel(Frame)
        self.selectionLabel.setMinimumSize(QtCore.QSize(0, 30))
        self.selectionLabel.setMaximumSize(QtCore.QSize(16777215, 30))
        font = QtGui.QFont()
        font.setPointSize(15)
        self.selectionLabel.setFont(font)
        self.selectionLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.selectionLabel.setObjectName("selectionLabel")
        self.verticalLayout_2.addWidget(self.selectionLabel)
        self.frame = QtGui.QFrame(Frame)
        self.frame.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame.setFrameShadow(QtGui.QFrame.Raised)
        self.frame.setObjectName("frame")
        self.verticalLayout = QtGui.QVBoxLayout(self.frame)
        self.verticalLayout.setObjectName("verticalLayout")
        self.sc1Check = QtGui.QCheckBox(self.frame)
        self.sc1Check.setObjectName("sc1Check")
        self.verticalLayout.addWidget(self.sc1Check)
        self.sc2Check = QtGui.QCheckBox(self.frame)
        self.sc2Check.setObjectName("sc2Check")
        self.verticalLayout.addWidget(self.sc2Check)
        self.verticalLayout_2.addWidget(self.frame)

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
        self.selectionLabel.setText(QtGui.QApplication.translate("Frame", "Label", None, QtGui.QApplication.UnicodeUTF8))
        self.sc1Check.setText(QtGui.QApplication.translate("Frame", "Scenario 1", None, QtGui.QApplication.UnicodeUTF8))
        self.sc2Check.setText(QtGui.QApplication.translate("Frame", "Scenario 2", None, QtGui.QApplication.UnicodeUTF8))

