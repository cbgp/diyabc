# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'uis/setHistFrame.ui'
#
# Created: Mon Feb 21 12:01:56 2011
#      by: PyQt4 UI code generator 4.7.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_Frame(object):
    def setupUi(self, Frame):
        Frame.setObjectName("Frame")
        Frame.resize(927, 738)
        Frame.setFrameShape(QtGui.QFrame.StyledPanel)
        Frame.setFrameShadow(QtGui.QFrame.Raised)
        self.verticalLayout = QtGui.QVBoxLayout(Frame)
        self.verticalLayout.setObjectName("verticalLayout")
        self.frame_6 = QtGui.QFrame(Frame)
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
        self.clearButton = QtGui.QPushButton(self.frame_6)
        font = QtGui.QFont()
        font.setPointSize(14)
        self.clearButton.setFont(font)
        self.clearButton.setObjectName("clearButton")
        self.horizontalLayout_9.addWidget(self.clearButton)
        self.okButton = QtGui.QPushButton(self.frame_6)
        font = QtGui.QFont()
        font.setPointSize(14)
        self.okButton.setFont(font)
        self.okButton.setObjectName("okButton")
        self.horizontalLayout_9.addWidget(self.okButton)
        self.verticalLayout.addWidget(self.frame_6)
        self.frame_4 = QtGui.QFrame(Frame)
        self.frame_4.setFrameShape(QtGui.QFrame.NoFrame)
        self.frame_4.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_4.setObjectName("frame_4")
        self.horizontalLayout_4 = QtGui.QHBoxLayout(self.frame_4)
        self.horizontalLayout_4.setContentsMargins(-1, 0, -1, 0)
        self.horizontalLayout_4.setObjectName("horizontalLayout_4")
        self.scScroll = QtGui.QScrollArea(self.frame_4)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.scScroll.sizePolicy().hasHeightForWidth())
        self.scScroll.setSizePolicy(sizePolicy)
        self.scScroll.setMinimumSize(QtCore.QSize(500, 200))
        self.scScroll.setMaximumSize(QtCore.QSize(16777215, 200))
        self.scScroll.setWidgetResizable(True)
        self.scScroll.setObjectName("scScroll")
        self.scScrollContent = QtGui.QWidget(self.scScroll)
        self.scScrollContent.setGeometry(QtCore.QRect(0, 0, 769, 196))
        self.scScrollContent.setObjectName("scScrollContent")
        self.horizontalLayout_3 = QtGui.QHBoxLayout(self.scScrollContent)
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.scScroll.setWidget(self.scScrollContent)
        self.horizontalLayout_4.addWidget(self.scScroll)
        self.verticalLayout_3 = QtGui.QVBoxLayout()
        self.verticalLayout_3.setObjectName("verticalLayout_3")
        self.addScButton = QtGui.QPushButton(self.frame_4)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.addScButton.sizePolicy().hasHeightForWidth())
        self.addScButton.setSizePolicy(sizePolicy)
        self.addScButton.setMaximumSize(QtCore.QSize(110, 16777215))
        self.addScButton.setObjectName("addScButton")
        self.verticalLayout_3.addWidget(self.addScButton)
        self.chkScButton = QtGui.QPushButton(self.frame_4)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.chkScButton.sizePolicy().hasHeightForWidth())
        self.chkScButton.setSizePolicy(sizePolicy)
        self.chkScButton.setMaximumSize(QtCore.QSize(110, 16777215))
        self.chkScButton.setObjectName("chkScButton")
        self.verticalLayout_3.addWidget(self.chkScButton)
        self.defPrButton = QtGui.QPushButton(self.frame_4)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.defPrButton.sizePolicy().hasHeightForWidth())
        self.defPrButton.setSizePolicy(sizePolicy)
        self.defPrButton.setMaximumSize(QtCore.QSize(110, 16777215))
        self.defPrButton.setObjectName("defPrButton")
        self.verticalLayout_3.addWidget(self.defPrButton)
        self.horizontalLayout_4.addLayout(self.verticalLayout_3)
        self.verticalLayout.addWidget(self.frame_4)
        self.frame_3 = QtGui.QFrame(Frame)
        self.frame_3.setFrameShape(QtGui.QFrame.NoFrame)
        self.frame_3.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_3.setObjectName("frame_3")
        self.horizontalLayout = QtGui.QHBoxLayout(self.frame_3)
        self.horizontalLayout.setContentsMargins(-1, 0, -1, 0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.label = QtGui.QLabel(self.frame_3)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)
        font = QtGui.QFont()
        font.setPointSize(12)
        self.label.setFont(font)
        self.label.setObjectName("label")
        self.horizontalLayout.addWidget(self.label)
        self.groupBox_2 = QtGui.QGroupBox(self.frame_3)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_2.sizePolicy().hasHeightForWidth())
        self.groupBox_2.setSizePolicy(sizePolicy)
        self.groupBox_2.setTitle("")
        self.groupBox_2.setObjectName("groupBox_2")
        self.verticalLayout_5 = QtGui.QVBoxLayout(self.groupBox_2)
        self.verticalLayout_5.setSpacing(1)
        self.verticalLayout_5.setContentsMargins(-1, 1, -1, 1)
        self.verticalLayout_5.setObjectName("verticalLayout_5")
        self.uniformRadio = QtGui.QRadioButton(self.groupBox_2)
        self.uniformRadio.setChecked(True)
        self.uniformRadio.setObjectName("uniformRadio")
        self.verticalLayout_5.addWidget(self.uniformRadio)
        self.otherRadio = QtGui.QRadioButton(self.groupBox_2)
        self.otherRadio.setChecked(False)
        self.otherRadio.setObjectName("otherRadio")
        self.verticalLayout_5.addWidget(self.otherRadio)
        self.horizontalLayout.addWidget(self.groupBox_2)
        self.repScroll = QtGui.QScrollArea(self.frame_3)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.repScroll.sizePolicy().hasHeightForWidth())
        self.repScroll.setSizePolicy(sizePolicy)
        self.repScroll.setMinimumSize(QtCore.QSize(0, 60))
        self.repScroll.setMaximumSize(QtCore.QSize(16777215, 60))
        self.repScroll.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.repScroll.setWidgetResizable(True)
        self.repScroll.setObjectName("repScroll")
        self.repScrollContent = QtGui.QWidget(self.repScroll)
        self.repScrollContent.setGeometry(QtCore.QRect(0, 0, 711, 56))
        self.repScrollContent.setObjectName("repScrollContent")
        self.horizontalLayout_6 = QtGui.QHBoxLayout(self.repScrollContent)
        self.horizontalLayout_6.setSpacing(2)
        self.horizontalLayout_6.setContentsMargins(-1, 0, -1, 1)
        self.horizontalLayout_6.setObjectName("horizontalLayout_6")
        self.repScroll.setWidget(self.repScrollContent)
        self.horizontalLayout.addWidget(self.repScroll)
        self.verticalLayout.addWidget(self.frame_3)
        self.frame_7 = QtGui.QFrame(Frame)
        self.frame_7.setFrameShape(QtGui.QFrame.NoFrame)
        self.frame_7.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_7.setObjectName("frame_7")
        self.verticalLayout_4 = QtGui.QVBoxLayout(self.frame_7)
        self.verticalLayout_4.setSpacing(1)
        self.verticalLayout_4.setMargin(1)
        self.verticalLayout_4.setObjectName("verticalLayout_4")
        self.frame_5 = QtGui.QFrame(self.frame_7)
        self.frame_5.setMinimumSize(QtCore.QSize(0, 21))
        self.frame_5.setMaximumSize(QtCore.QSize(16777215, 21))
        self.frame_5.setFrameShape(QtGui.QFrame.NoFrame)
        self.frame_5.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_5.setObjectName("frame_5")
        self.horizontalLayout_5 = QtGui.QHBoxLayout(self.frame_5)
        self.horizontalLayout_5.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_5.setObjectName("horizontalLayout_5")
        self.groupBox_11 = QtGui.QGroupBox(self.frame_5)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_11.sizePolicy().hasHeightForWidth())
        self.groupBox_11.setSizePolicy(sizePolicy)
        self.groupBox_11.setMinimumSize(QtCore.QSize(140, 20))
        self.groupBox_11.setMaximumSize(QtCore.QSize(140, 20))
        self.groupBox_11.setTitle("")
        self.groupBox_11.setObjectName("groupBox_11")
        self.horizontalLayout_12 = QtGui.QHBoxLayout(self.groupBox_11)
        self.horizontalLayout_12.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_12.setObjectName("horizontalLayout_12")
        self.label_2 = QtGui.QLabel(self.groupBox_11)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_2.sizePolicy().hasHeightForWidth())
        self.label_2.setSizePolicy(sizePolicy)
        self.label_2.setMinimumSize(QtCore.QSize(140, 0))
        self.label_2.setMaximumSize(QtCore.QSize(140, 16777215))
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_2.setFont(font)
        self.label_2.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignVCenter)
        self.label_2.setObjectName("label_2")
        self.horizontalLayout_12.addWidget(self.label_2)
        self.horizontalLayout_5.addWidget(self.groupBox_11)
        self.groupBox_12 = QtGui.QGroupBox(self.frame_5)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_12.sizePolicy().hasHeightForWidth())
        self.groupBox_12.setSizePolicy(sizePolicy)
        self.groupBox_12.setMinimumSize(QtCore.QSize(290, 20))
        self.groupBox_12.setMaximumSize(QtCore.QSize(290, 20))
        self.groupBox_12.setTitle("")
        self.groupBox_12.setObjectName("groupBox_12")
        self.horizontalLayout_7 = QtGui.QHBoxLayout(self.groupBox_12)
        self.horizontalLayout_7.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_7.setObjectName("horizontalLayout_7")
        self.label_3 = QtGui.QLabel(self.groupBox_12)
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_3.setFont(font)
        self.label_3.setAlignment(QtCore.Qt.AlignCenter)
        self.label_3.setObjectName("label_3")
        self.horizontalLayout_7.addWidget(self.label_3)
        self.label_4 = QtGui.QLabel(self.groupBox_12)
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_4.setFont(font)
        self.label_4.setAlignment(QtCore.Qt.AlignCenter)
        self.label_4.setObjectName("label_4")
        self.horizontalLayout_7.addWidget(self.label_4)
        self.label_5 = QtGui.QLabel(self.groupBox_12)
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_5.setFont(font)
        self.label_5.setAlignment(QtCore.Qt.AlignCenter)
        self.label_5.setObjectName("label_5")
        self.horizontalLayout_7.addWidget(self.label_5)
        self.label_6 = QtGui.QLabel(self.groupBox_12)
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_6.setFont(font)
        self.label_6.setAlignment(QtCore.Qt.AlignCenter)
        self.label_6.setObjectName("label_6")
        self.horizontalLayout_7.addWidget(self.label_6)
        self.horizontalLayout_5.addWidget(self.groupBox_12)
        self.groupBox_13 = QtGui.QGroupBox(self.frame_5)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_13.sizePolicy().hasHeightForWidth())
        self.groupBox_13.setSizePolicy(sizePolicy)
        self.groupBox_13.setMinimumSize(QtCore.QSize(0, 20))
        self.groupBox_13.setMaximumSize(QtCore.QSize(16777215, 20))
        self.groupBox_13.setTitle("")
        self.groupBox_13.setObjectName("groupBox_13")
        self.horizontalLayout_11 = QtGui.QHBoxLayout(self.groupBox_13)
        self.horizontalLayout_11.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_11.setObjectName("horizontalLayout_11")
        self.label_7 = QtGui.QLabel(self.groupBox_13)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_7.sizePolicy().hasHeightForWidth())
        self.label_7.setSizePolicy(sizePolicy)
        self.label_7.setMinimumSize(QtCore.QSize(80, 0))
        self.label_7.setMaximumSize(QtCore.QSize(80, 16777215))
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_7.setFont(font)
        self.label_7.setAlignment(QtCore.Qt.AlignCenter)
        self.label_7.setObjectName("label_7")
        self.horizontalLayout_11.addWidget(self.label_7)
        self.label_8 = QtGui.QLabel(self.groupBox_13)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_8.sizePolicy().hasHeightForWidth())
        self.label_8.setSizePolicy(sizePolicy)
        self.label_8.setMinimumSize(QtCore.QSize(80, 0))
        self.label_8.setMaximumSize(QtCore.QSize(80, 16777215))
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_8.setFont(font)
        self.label_8.setAlignment(QtCore.Qt.AlignCenter)
        self.label_8.setObjectName("label_8")
        self.horizontalLayout_11.addWidget(self.label_8)
        self.label_9 = QtGui.QLabel(self.groupBox_13)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_9.sizePolicy().hasHeightForWidth())
        self.label_9.setSizePolicy(sizePolicy)
        self.label_9.setMinimumSize(QtCore.QSize(80, 0))
        self.label_9.setMaximumSize(QtCore.QSize(80, 16777215))
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_9.setFont(font)
        self.label_9.setAlignment(QtCore.Qt.AlignCenter)
        self.label_9.setObjectName("label_9")
        self.horizontalLayout_11.addWidget(self.label_9)
        self.label_10 = QtGui.QLabel(self.groupBox_13)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_10.sizePolicy().hasHeightForWidth())
        self.label_10.setSizePolicy(sizePolicy)
        self.label_10.setMinimumSize(QtCore.QSize(80, 0))
        self.label_10.setMaximumSize(QtCore.QSize(80, 16777215))
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_10.setFont(font)
        self.label_10.setAlignment(QtCore.Qt.AlignCenter)
        self.label_10.setObjectName("label_10")
        self.horizontalLayout_11.addWidget(self.label_10)
        self.horizontalLayout_5.addWidget(self.groupBox_13)
        self.verticalLayout_4.addWidget(self.frame_5)
        self.verticalLayout.addWidget(self.frame_7)
        self.scrollArea = QtGui.QScrollArea(Frame)
        font = QtGui.QFont()
        font.setPointSize(6)
        self.scrollArea.setFont(font)
        self.scrollArea.setFrameShape(QtGui.QFrame.NoFrame)
        self.scrollArea.setWidgetResizable(True)
        self.scrollArea.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignTop)
        self.scrollArea.setObjectName("scrollArea")
        self.scrollAreaWidgetContents = QtGui.QWidget(self.scrollArea)
        self.scrollAreaWidgetContents.setGeometry(QtCore.QRect(0, 0, 905, 242))
        font = QtGui.QFont()
        font.setPointSize(8)
        self.scrollAreaWidgetContents.setFont(font)
        self.scrollAreaWidgetContents.setObjectName("scrollAreaWidgetContents")
        self.verticalLayout_6 = QtGui.QVBoxLayout(self.scrollAreaWidgetContents)
        self.verticalLayout_6.setSpacing(1)
        self.verticalLayout_6.setMargin(1)
        self.verticalLayout_6.setObjectName("verticalLayout_6")
        self.scrollArea.setWidget(self.scrollAreaWidgetContents)
        self.verticalLayout.addWidget(self.scrollArea)
        self.frame = QtGui.QFrame(Frame)
        self.frame.setFrameShape(QtGui.QFrame.NoFrame)
        self.frame.setFrameShadow(QtGui.QFrame.Raised)
        self.frame.setObjectName("frame")
        self.verticalLayout_2 = QtGui.QVBoxLayout(self.frame)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.scrollArea_3 = QtGui.QScrollArea(self.frame)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.scrollArea_3.sizePolicy().hasHeightForWidth())
        self.scrollArea_3.setSizePolicy(sizePolicy)
        self.scrollArea_3.setMinimumSize(QtCore.QSize(0, 60))
        self.scrollArea_3.setMaximumSize(QtCore.QSize(16777215, 60))
        font = QtGui.QFont()
        font.setPointSize(8)
        self.scrollArea_3.setFont(font)
        self.scrollArea_3.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.scrollArea_3.setWidgetResizable(True)
        self.scrollArea_3.setObjectName("scrollArea_3")
        self.scrollAreaWidgetContents_3 = QtGui.QWidget(self.scrollArea_3)
        self.scrollAreaWidgetContents_3.setGeometry(QtCore.QRect(0, 0, 883, 56))
        self.scrollAreaWidgetContents_3.setObjectName("scrollAreaWidgetContents_3")
        self.horizontalLayout_2 = QtGui.QHBoxLayout(self.scrollAreaWidgetContents_3)
        self.horizontalLayout_2.setSpacing(1)
        self.horizontalLayout_2.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.scrollArea_3.setWidget(self.scrollAreaWidgetContents_3)
        self.verticalLayout_2.addWidget(self.scrollArea_3)
        self.frame_2 = QtGui.QFrame(self.frame)
        self.frame_2.setFrameShape(QtGui.QFrame.NoFrame)
        self.frame_2.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_2.setObjectName("frame_2")
        self.horizontalLayout_8 = QtGui.QHBoxLayout(self.frame_2)
        self.horizontalLayout_8.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_8.setObjectName("horizontalLayout_8")
        self.drawUntilRadio = QtGui.QRadioButton(self.frame_2)
        font = QtGui.QFont()
        font.setPointSize(8)
        self.drawUntilRadio.setFont(font)
        self.drawUntilRadio.setChecked(True)
        self.drawUntilRadio.setObjectName("drawUntilRadio")
        self.horizontalLayout_8.addWidget(self.drawUntilRadio)
        self.drawOnceRadio = QtGui.QRadioButton(self.frame_2)
        font = QtGui.QFont()
        font.setPointSize(8)
        self.drawOnceRadio.setFont(font)
        self.drawOnceRadio.setObjectName("drawOnceRadio")
        self.horizontalLayout_8.addWidget(self.drawOnceRadio)
        self.verticalLayout_2.addWidget(self.frame_2)
        self.verticalLayout.addWidget(self.frame)

        self.retranslateUi(Frame)
        QtCore.QMetaObject.connectSlotsByName(Frame)

    def retranslateUi(self, Frame):
        Frame.setWindowTitle(QtGui.QApplication.translate("Frame", "Frame", None, QtGui.QApplication.UnicodeUTF8))
        self.exitButton.setToolTip(QtGui.QApplication.translate("Frame", "Leave this screen without loosing any change", None, QtGui.QApplication.UnicodeUTF8))
        self.exitButton.setText(QtGui.QApplication.translate("Frame", "EXIT", None, QtGui.QApplication.UnicodeUTF8))
        self.exitButton.setShortcut(QtGui.QApplication.translate("Frame", "Alt+E", None, QtGui.QApplication.UnicodeUTF8))
        self.clearButton.setToolTip(QtGui.QApplication.translate("Frame", "Clear all data related to this historical model", None, QtGui.QApplication.UnicodeUTF8))
        self.clearButton.setText(QtGui.QApplication.translate("Frame", "CLEAR", None, QtGui.QApplication.UnicodeUTF8))
        self.clearButton.setShortcut(QtGui.QApplication.translate("Frame", "Alt+C", None, QtGui.QApplication.UnicodeUTF8))
        self.okButton.setToolTip(QtGui.QApplication.translate("Frame", "Test and save data (Alt+o)", None, QtGui.QApplication.UnicodeUTF8))
        self.okButton.setText(QtGui.QApplication.translate("Frame", "VALIDATE", None, QtGui.QApplication.UnicodeUTF8))
        self.okButton.setShortcut(QtGui.QApplication.translate("Frame", "Alt+V", None, QtGui.QApplication.UnicodeUTF8))
        self.addScButton.setToolTip(QtGui.QApplication.translate("Frame", "(Alt+a)", None, QtGui.QApplication.UnicodeUTF8))
        self.addScButton.setText(QtGui.QApplication.translate("Frame", "Add scenario", None, QtGui.QApplication.UnicodeUTF8))
        self.addScButton.setShortcut(QtGui.QApplication.translate("Frame", "Alt+A", None, QtGui.QApplication.UnicodeUTF8))
        self.chkScButton.setText(QtGui.QApplication.translate("Frame", "Check scenario", None, QtGui.QApplication.UnicodeUTF8))
        self.defPrButton.setText(QtGui.QApplication.translate("Frame", "Define priors", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Frame", "Scenario", None, QtGui.QApplication.UnicodeUTF8))
        self.uniformRadio.setText(QtGui.QApplication.translate("Frame", "Uniform", None, QtGui.QApplication.UnicodeUTF8))
        self.otherRadio.setText(QtGui.QApplication.translate("Frame", "Other", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("Frame", "parameters", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Frame", "Uniform", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("Frame", "Log-uniform", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("Frame", "Normal", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("Frame", "Log-normal", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("Frame", "minimum", None, QtGui.QApplication.UnicodeUTF8))
        self.label_8.setText(QtGui.QApplication.translate("Frame", "maximum", None, QtGui.QApplication.UnicodeUTF8))
        self.label_9.setText(QtGui.QApplication.translate("Frame", "mean", None, QtGui.QApplication.UnicodeUTF8))
        self.label_10.setText(QtGui.QApplication.translate("Frame", "st-deviation", None, QtGui.QApplication.UnicodeUTF8))
        self.drawUntilRadio.setText(QtGui.QApplication.translate("Frame", "Draw parameters values until conditions are fulfilled", None, QtGui.QApplication.UnicodeUTF8))
        self.drawOnceRadio.setText(QtGui.QApplication.translate("Frame", "Draw parameters values only once. Discard if any condition is not fulfilled", None, QtGui.QApplication.UnicodeUTF8))

