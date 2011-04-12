# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'uis/viewTextFile.ui'
#
# Created: Tue Apr 12 16:24:32 2011
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
        Frame.resize(857, 629)
        Frame.setFrameShape(QtGui.QFrame.StyledPanel)
        Frame.setFrameShadow(QtGui.QFrame.Raised)
        self.verticalLayout = QtGui.QVBoxLayout(Frame)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.okButton = QtGui.QPushButton(Frame)
        font = QtGui.QFont()
        font.setPointSize(13)
        self.okButton.setFont(font)
        self.okButton.setObjectName(_fromUtf8("okButton"))
        self.verticalLayout.addWidget(self.okButton)
        self.dataPlain = QtGui.QPlainTextEdit(Frame)
        self.dataPlain.setTextInteractionFlags(QtCore.Qt.TextSelectableByKeyboard|QtCore.Qt.TextSelectableByMouse)
        self.dataPlain.setObjectName(_fromUtf8("dataPlain"))
        self.verticalLayout.addWidget(self.dataPlain)

        self.retranslateUi(Frame)
        QtCore.QMetaObject.connectSlotsByName(Frame)

    def retranslateUi(self, Frame):
        Frame.setWindowTitle(QtGui.QApplication.translate("Frame", "Frame", None, QtGui.QApplication.UnicodeUTF8))
        self.okButton.setText(QtGui.QApplication.translate("Frame", "OK", None, QtGui.QApplication.UnicodeUTF8))

