# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'uis/preferences.ui'
#
# Created: Thu Mar 24 17:36:13 2011
#      by: PyQt4 UI code generator 4.8.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.setWindowModality(QtCore.Qt.WindowModal)
        MainWindow.resize(1000, 750)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(MainWindow.sizePolicy().hasHeightForWidth())
        MainWindow.setSizePolicy(sizePolicy)
        MainWindow.setMinimumSize(QtCore.QSize(1000, 750))
        MainWindow.setMaximumSize(QtCore.QSize(1000, 750))
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.centralwidget)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.tabWidget = QtGui.QTabWidget(self.centralwidget)
        self.tabWidget.setObjectName(_fromUtf8("tabWidget"))
        self.tab = QtGui.QWidget()
        self.tab.setObjectName(_fromUtf8("tab"))
        self.verticalLayout_56 = QtGui.QVBoxLayout(self.tab)
        self.verticalLayout_56.setObjectName(_fromUtf8("verticalLayout_56"))
        self.gridLayout = QtGui.QGridLayout()
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.label = QtGui.QLabel(self.tab)
        self.label.setMinimumSize(QtCore.QSize(0, 20))
        self.label.setMaximumSize(QtCore.QSize(16777215, 20))
        self.label.setObjectName(_fromUtf8("label"))
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.label_2 = QtGui.QLabel(self.tab)
        self.label_2.setMinimumSize(QtCore.QSize(0, 20))
        self.label_2.setMaximumSize(QtCore.QSize(16777215, 20))
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.gridLayout.addWidget(self.label_2, 0, 1, 1, 1)
        self.addrEdit = QtGui.QLineEdit(self.tab)
        self.addrEdit.setObjectName(_fromUtf8("addrEdit"))
        self.gridLayout.addWidget(self.addrEdit, 1, 0, 1, 1)
        self.portEdit = QtGui.QLineEdit(self.tab)
        self.portEdit.setObjectName(_fromUtf8("portEdit"))
        self.gridLayout.addWidget(self.portEdit, 1, 1, 1, 1)
        self.connectButton = QtGui.QPushButton(self.tab)
        self.connectButton.setObjectName(_fromUtf8("connectButton"))
        self.gridLayout.addWidget(self.connectButton, 1, 2, 1, 1)
        self.verticalLayout_56.addLayout(self.gridLayout)
        self.tabWidget.addTab(self.tab, _fromUtf8(""))
        self.tab_2 = QtGui.QWidget()
        self.tab_2.setObjectName(_fromUtf8("tab_2"))
        self.verticalLayout_2 = QtGui.QVBoxLayout(self.tab_2)
        self.verticalLayout_2.setObjectName(_fromUtf8("verticalLayout_2"))
        self.frame_3 = QtGui.QFrame(self.tab_2)
        self.frame_3.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame_3.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_3.setObjectName(_fromUtf8("frame_3"))
        self.horizontalLayout_3 = QtGui.QHBoxLayout(self.frame_3)
        self.horizontalLayout_3.setObjectName(_fromUtf8("horizontalLayout_3"))
        self.label_5 = QtGui.QLabel(self.frame_3)
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.horizontalLayout_3.addWidget(self.label_5)
        self.execBrowseButton = QtGui.QPushButton(self.frame_3)
        self.execBrowseButton.setObjectName(_fromUtf8("execBrowseButton"))
        self.horizontalLayout_3.addWidget(self.execBrowseButton)
        self.execPathEdit = QtGui.QLineEdit(self.frame_3)
        self.execPathEdit.setObjectName(_fromUtf8("execPathEdit"))
        self.horizontalLayout_3.addWidget(self.execPathEdit)
        self.verticalLayout_2.addWidget(self.frame_3)
        self.frame = QtGui.QFrame(self.tab_2)
        self.frame.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame.setFrameShadow(QtGui.QFrame.Raised)
        self.frame.setObjectName(_fromUtf8("frame"))
        self.horizontalLayout = QtGui.QHBoxLayout(self.frame)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.label_3 = QtGui.QLabel(self.frame)
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.horizontalLayout.addWidget(self.label_3)
        self.formatCombo = QtGui.QComboBox(self.frame)
        self.formatCombo.setObjectName(_fromUtf8("formatCombo"))
        self.formatCombo.addItem(_fromUtf8(""))
        self.formatCombo.addItem(_fromUtf8(""))
        self.formatCombo.addItem(_fromUtf8(""))
        self.horizontalLayout.addWidget(self.formatCombo)
        self.verticalLayout_2.addWidget(self.frame)
        self.frame_2 = QtGui.QFrame(self.tab_2)
        self.frame_2.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame_2.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_2.setObjectName(_fromUtf8("frame_2"))
        self.horizontalLayout_2 = QtGui.QHBoxLayout(self.frame_2)
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.label_4 = QtGui.QLabel(self.frame_2)
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.horizontalLayout_2.addWidget(self.label_4)
        self.styleCombo = QtGui.QComboBox(self.frame_2)
        self.styleCombo.setObjectName(_fromUtf8("styleCombo"))
        self.horizontalLayout_2.addWidget(self.styleCombo)
        self.verticalLayout_2.addWidget(self.frame_2)
        self.tabWidget.addTab(self.tab_2, _fromUtf8(""))
        self.tab_4 = QtGui.QWidget()
        self.tab_4.setObjectName(_fromUtf8("tab_4"))
        self.verticalLayout_57 = QtGui.QVBoxLayout(self.tab_4)
        self.verticalLayout_57.setObjectName(_fromUtf8("verticalLayout_57"))
        self.setMutSeqLabel_2 = QtGui.QLabel(self.tab_4)
        self.setMutSeqLabel_2.setMinimumSize(QtCore.QSize(0, 25))
        self.setMutSeqLabel_2.setMaximumSize(QtCore.QSize(16777215, 25))
        font = QtGui.QFont()
        font.setPointSize(16)
        self.setMutSeqLabel_2.setFont(font)
        self.setMutSeqLabel_2.setAlignment(QtCore.Qt.AlignCenter)
        self.setMutSeqLabel_2.setObjectName(_fromUtf8("setMutSeqLabel_2"))
        self.verticalLayout_57.addWidget(self.setMutSeqLabel_2)
        self.frame_11 = QtGui.QFrame(self.tab_4)
        self.frame_11.setMinimumSize(QtCore.QSize(0, 21))
        self.frame_11.setMaximumSize(QtCore.QSize(16777215, 21))
        self.frame_11.setFrameShape(QtGui.QFrame.NoFrame)
        self.frame_11.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_11.setObjectName(_fromUtf8("frame_11"))
        self.horizontalLayout_27 = QtGui.QHBoxLayout(self.frame_11)
        self.horizontalLayout_27.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_27.setObjectName(_fromUtf8("horizontalLayout_27"))
        self.groupBox_14 = QtGui.QGroupBox(self.frame_11)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_14.sizePolicy().hasHeightForWidth())
        self.groupBox_14.setSizePolicy(sizePolicy)
        self.groupBox_14.setMinimumSize(QtCore.QSize(140, 20))
        self.groupBox_14.setMaximumSize(QtCore.QSize(140, 20))
        self.groupBox_14.setTitle(_fromUtf8(""))
        self.groupBox_14.setObjectName(_fromUtf8("groupBox_14"))
        self.horizontalLayout_28 = QtGui.QHBoxLayout(self.groupBox_14)
        self.horizontalLayout_28.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_28.setObjectName(_fromUtf8("horizontalLayout_28"))
        self.label_64 = QtGui.QLabel(self.groupBox_14)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_64.sizePolicy().hasHeightForWidth())
        self.label_64.setSizePolicy(sizePolicy)
        self.label_64.setMinimumSize(QtCore.QSize(140, 0))
        self.label_64.setMaximumSize(QtCore.QSize(140, 16777215))
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_64.setFont(font)
        self.label_64.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignVCenter)
        self.label_64.setObjectName(_fromUtf8("label_64"))
        self.horizontalLayout_28.addWidget(self.label_64)
        self.horizontalLayout_27.addWidget(self.groupBox_14)
        self.groupBox_15 = QtGui.QGroupBox(self.frame_11)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_15.sizePolicy().hasHeightForWidth())
        self.groupBox_15.setSizePolicy(sizePolicy)
        self.groupBox_15.setMinimumSize(QtCore.QSize(290, 20))
        self.groupBox_15.setMaximumSize(QtCore.QSize(290, 20))
        self.groupBox_15.setTitle(_fromUtf8(""))
        self.groupBox_15.setObjectName(_fromUtf8("groupBox_15"))
        self.horizontalLayout_29 = QtGui.QHBoxLayout(self.groupBox_15)
        self.horizontalLayout_29.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_29.setObjectName(_fromUtf8("horizontalLayout_29"))
        self.label_67 = QtGui.QLabel(self.groupBox_15)
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_67.setFont(font)
        self.label_67.setAlignment(QtCore.Qt.AlignCenter)
        self.label_67.setObjectName(_fromUtf8("label_67"))
        self.horizontalLayout_29.addWidget(self.label_67)
        self.label_68 = QtGui.QLabel(self.groupBox_15)
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_68.setFont(font)
        self.label_68.setAlignment(QtCore.Qt.AlignCenter)
        self.label_68.setObjectName(_fromUtf8("label_68"))
        self.horizontalLayout_29.addWidget(self.label_68)
        self.label_69 = QtGui.QLabel(self.groupBox_15)
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_69.setFont(font)
        self.label_69.setAlignment(QtCore.Qt.AlignCenter)
        self.label_69.setObjectName(_fromUtf8("label_69"))
        self.horizontalLayout_29.addWidget(self.label_69)
        self.label_70 = QtGui.QLabel(self.groupBox_15)
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_70.setFont(font)
        self.label_70.setAlignment(QtCore.Qt.AlignCenter)
        self.label_70.setObjectName(_fromUtf8("label_70"))
        self.horizontalLayout_29.addWidget(self.label_70)
        self.horizontalLayout_27.addWidget(self.groupBox_15)
        self.groupBox_16 = QtGui.QGroupBox(self.frame_11)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_16.sizePolicy().hasHeightForWidth())
        self.groupBox_16.setSizePolicy(sizePolicy)
        self.groupBox_16.setMinimumSize(QtCore.QSize(0, 20))
        self.groupBox_16.setMaximumSize(QtCore.QSize(16777215, 20))
        self.groupBox_16.setTitle(_fromUtf8(""))
        self.groupBox_16.setObjectName(_fromUtf8("groupBox_16"))
        self.horizontalLayout_30 = QtGui.QHBoxLayout(self.groupBox_16)
        self.horizontalLayout_30.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_30.setObjectName(_fromUtf8("horizontalLayout_30"))
        self.label_71 = QtGui.QLabel(self.groupBox_16)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_71.sizePolicy().hasHeightForWidth())
        self.label_71.setSizePolicy(sizePolicy)
        self.label_71.setMinimumSize(QtCore.QSize(80, 0))
        self.label_71.setMaximumSize(QtCore.QSize(80, 16777215))
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_71.setFont(font)
        self.label_71.setAlignment(QtCore.Qt.AlignCenter)
        self.label_71.setObjectName(_fromUtf8("label_71"))
        self.horizontalLayout_30.addWidget(self.label_71)
        self.label_72 = QtGui.QLabel(self.groupBox_16)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_72.sizePolicy().hasHeightForWidth())
        self.label_72.setSizePolicy(sizePolicy)
        self.label_72.setMinimumSize(QtCore.QSize(80, 0))
        self.label_72.setMaximumSize(QtCore.QSize(80, 16777215))
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_72.setFont(font)
        self.label_72.setAlignment(QtCore.Qt.AlignCenter)
        self.label_72.setObjectName(_fromUtf8("label_72"))
        self.horizontalLayout_30.addWidget(self.label_72)
        self.label_73 = QtGui.QLabel(self.groupBox_16)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_73.sizePolicy().hasHeightForWidth())
        self.label_73.setSizePolicy(sizePolicy)
        self.label_73.setMinimumSize(QtCore.QSize(80, 0))
        self.label_73.setMaximumSize(QtCore.QSize(80, 16777215))
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_73.setFont(font)
        self.label_73.setAlignment(QtCore.Qt.AlignCenter)
        self.label_73.setObjectName(_fromUtf8("label_73"))
        self.horizontalLayout_30.addWidget(self.label_73)
        self.label_74 = QtGui.QLabel(self.groupBox_16)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_74.sizePolicy().hasHeightForWidth())
        self.label_74.setSizePolicy(sizePolicy)
        self.label_74.setMinimumSize(QtCore.QSize(80, 0))
        self.label_74.setMaximumSize(QtCore.QSize(80, 16777215))
        font = QtGui.QFont()
        font.setPointSize(8)
        self.label_74.setFont(font)
        self.label_74.setAlignment(QtCore.Qt.AlignCenter)
        self.label_74.setObjectName(_fromUtf8("label_74"))
        self.horizontalLayout_30.addWidget(self.label_74)
        self.horizontalLayout_27.addWidget(self.groupBox_16)
        self.verticalLayout_57.addWidget(self.frame_11)
        self.frame_12 = QtGui.QFrame(self.tab_4)
        self.frame_12.setMinimumSize(QtCore.QSize(0, 40))
        self.frame_12.setMaximumSize(QtCore.QSize(16777215, 40))
        self.frame_12.setFrameShape(QtGui.QFrame.NoFrame)
        self.frame_12.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_12.setObjectName(_fromUtf8("frame_12"))
        self.horizontalLayout_31 = QtGui.QHBoxLayout(self.frame_12)
        self.horizontalLayout_31.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_31.setObjectName(_fromUtf8("horizontalLayout_31"))
        self.groupBox_17 = QtGui.QGroupBox(self.frame_12)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_17.sizePolicy().hasHeightForWidth())
        self.groupBox_17.setSizePolicy(sizePolicy)
        self.groupBox_17.setMinimumSize(QtCore.QSize(140, 40))
        self.groupBox_17.setMaximumSize(QtCore.QSize(140, 40))
        self.groupBox_17.setTitle(_fromUtf8(""))
        self.groupBox_17.setObjectName(_fromUtf8("groupBox_17"))
        self.horizontalLayout_32 = QtGui.QHBoxLayout(self.groupBox_17)
        self.horizontalLayout_32.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_32.setObjectName(_fromUtf8("horizontalLayout_32"))
        self.label_75 = QtGui.QLabel(self.groupBox_17)
        self.label_75.setAlignment(QtCore.Qt.AlignCenter)
        self.label_75.setObjectName(_fromUtf8("label_75"))
        self.horizontalLayout_32.addWidget(self.label_75)
        self.horizontalLayout_31.addWidget(self.groupBox_17)
        self.groupBox_18 = QtGui.QGroupBox(self.frame_12)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_18.sizePolicy().hasHeightForWidth())
        self.groupBox_18.setSizePolicy(sizePolicy)
        self.groupBox_18.setMinimumSize(QtCore.QSize(290, 40))
        self.groupBox_18.setMaximumSize(QtCore.QSize(290, 40))
        self.groupBox_18.setTitle(_fromUtf8(""))
        self.groupBox_18.setObjectName(_fromUtf8("groupBox_18"))
        self.horizontalLayout_33 = QtGui.QHBoxLayout(self.groupBox_18)
        self.horizontalLayout_33.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_33.setObjectName(_fromUtf8("horizontalLayout_33"))
        self.NUNRadio = QtGui.QRadioButton(self.groupBox_18)
        self.NUNRadio.setText(_fromUtf8(""))
        self.NUNRadio.setChecked(True)
        self.NUNRadio.setObjectName(_fromUtf8("NUNRadio"))
        self.horizontalLayout_33.addWidget(self.NUNRadio)
        self.NLURadio = QtGui.QRadioButton(self.groupBox_18)
        self.NLURadio.setText(_fromUtf8(""))
        self.NLURadio.setObjectName(_fromUtf8("NLURadio"))
        self.horizontalLayout_33.addWidget(self.NLURadio)
        self.NNORadio = QtGui.QRadioButton(self.groupBox_18)
        self.NNORadio.setText(_fromUtf8(""))
        self.NNORadio.setObjectName(_fromUtf8("NNORadio"))
        self.horizontalLayout_33.addWidget(self.NNORadio)
        self.NLNRadio = QtGui.QRadioButton(self.groupBox_18)
        self.NLNRadio.setText(_fromUtf8(""))
        self.NLNRadio.setObjectName(_fromUtf8("NLNRadio"))
        self.horizontalLayout_33.addWidget(self.NLNRadio)
        self.horizontalLayout_31.addWidget(self.groupBox_18)
        self.groupBox_19 = QtGui.QGroupBox(self.frame_12)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_19.sizePolicy().hasHeightForWidth())
        self.groupBox_19.setSizePolicy(sizePolicy)
        self.groupBox_19.setMinimumSize(QtCore.QSize(0, 40))
        self.groupBox_19.setMaximumSize(QtCore.QSize(16777215, 40))
        self.groupBox_19.setTitle(_fromUtf8(""))
        self.groupBox_19.setObjectName(_fromUtf8("groupBox_19"))
        self.horizontalLayout_34 = QtGui.QHBoxLayout(self.groupBox_19)
        self.horizontalLayout_34.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_34.setObjectName(_fromUtf8("horizontalLayout_34"))
        self.NminEdit = QtGui.QLineEdit(self.groupBox_19)
        self.NminEdit.setObjectName(_fromUtf8("NminEdit"))
        self.horizontalLayout_34.addWidget(self.NminEdit)
        self.NmaxEdit = QtGui.QLineEdit(self.groupBox_19)
        self.NmaxEdit.setObjectName(_fromUtf8("NmaxEdit"))
        self.horizontalLayout_34.addWidget(self.NmaxEdit)
        self.NmeanEdit = QtGui.QLineEdit(self.groupBox_19)
        self.NmeanEdit.setObjectName(_fromUtf8("NmeanEdit"))
        self.horizontalLayout_34.addWidget(self.NmeanEdit)
        self.NstdevEdit = QtGui.QLineEdit(self.groupBox_19)
        self.NstdevEdit.setObjectName(_fromUtf8("NstdevEdit"))
        self.horizontalLayout_34.addWidget(self.NstdevEdit)
        self.horizontalLayout_31.addWidget(self.groupBox_19)
        self.verticalLayout_57.addWidget(self.frame_12)
        self.frame_13 = QtGui.QFrame(self.tab_4)
        self.frame_13.setMinimumSize(QtCore.QSize(0, 40))
        self.frame_13.setMaximumSize(QtCore.QSize(16777215, 40))
        self.frame_13.setFrameShape(QtGui.QFrame.NoFrame)
        self.frame_13.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_13.setObjectName(_fromUtf8("frame_13"))
        self.horizontalLayout_35 = QtGui.QHBoxLayout(self.frame_13)
        self.horizontalLayout_35.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_35.setObjectName(_fromUtf8("horizontalLayout_35"))
        self.groupBox_20 = QtGui.QGroupBox(self.frame_13)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_20.sizePolicy().hasHeightForWidth())
        self.groupBox_20.setSizePolicy(sizePolicy)
        self.groupBox_20.setMinimumSize(QtCore.QSize(140, 40))
        self.groupBox_20.setMaximumSize(QtCore.QSize(140, 40))
        self.groupBox_20.setTitle(_fromUtf8(""))
        self.groupBox_20.setObjectName(_fromUtf8("groupBox_20"))
        self.horizontalLayout_36 = QtGui.QHBoxLayout(self.groupBox_20)
        self.horizontalLayout_36.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_36.setObjectName(_fromUtf8("horizontalLayout_36"))
        self.label_77 = QtGui.QLabel(self.groupBox_20)
        self.label_77.setAlignment(QtCore.Qt.AlignCenter)
        self.label_77.setObjectName(_fromUtf8("label_77"))
        self.horizontalLayout_36.addWidget(self.label_77)
        self.horizontalLayout_35.addWidget(self.groupBox_20)
        self.groupBox_21 = QtGui.QGroupBox(self.frame_13)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_21.sizePolicy().hasHeightForWidth())
        self.groupBox_21.setSizePolicy(sizePolicy)
        self.groupBox_21.setMinimumSize(QtCore.QSize(290, 40))
        self.groupBox_21.setMaximumSize(QtCore.QSize(290, 40))
        self.groupBox_21.setTitle(_fromUtf8(""))
        self.groupBox_21.setObjectName(_fromUtf8("groupBox_21"))
        self.horizontalLayout_37 = QtGui.QHBoxLayout(self.groupBox_21)
        self.horizontalLayout_37.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_37.setObjectName(_fromUtf8("horizontalLayout_37"))
        self.TUNRadio = QtGui.QRadioButton(self.groupBox_21)
        self.TUNRadio.setText(_fromUtf8(""))
        self.TUNRadio.setChecked(True)
        self.TUNRadio.setObjectName(_fromUtf8("TUNRadio"))
        self.horizontalLayout_37.addWidget(self.TUNRadio)
        self.TLURadio = QtGui.QRadioButton(self.groupBox_21)
        self.TLURadio.setText(_fromUtf8(""))
        self.TLURadio.setObjectName(_fromUtf8("TLURadio"))
        self.horizontalLayout_37.addWidget(self.TLURadio)
        self.TNORadio = QtGui.QRadioButton(self.groupBox_21)
        self.TNORadio.setText(_fromUtf8(""))
        self.TNORadio.setObjectName(_fromUtf8("TNORadio"))
        self.horizontalLayout_37.addWidget(self.TNORadio)
        self.TLNRadio = QtGui.QRadioButton(self.groupBox_21)
        self.TLNRadio.setText(_fromUtf8(""))
        self.TLNRadio.setObjectName(_fromUtf8("TLNRadio"))
        self.horizontalLayout_37.addWidget(self.TLNRadio)
        self.horizontalLayout_35.addWidget(self.groupBox_21)
        self.groupBox_22 = QtGui.QGroupBox(self.frame_13)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_22.sizePolicy().hasHeightForWidth())
        self.groupBox_22.setSizePolicy(sizePolicy)
        self.groupBox_22.setMinimumSize(QtCore.QSize(0, 40))
        self.groupBox_22.setMaximumSize(QtCore.QSize(16777215, 40))
        self.groupBox_22.setTitle(_fromUtf8(""))
        self.groupBox_22.setObjectName(_fromUtf8("groupBox_22"))
        self.horizontalLayout_38 = QtGui.QHBoxLayout(self.groupBox_22)
        self.horizontalLayout_38.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_38.setObjectName(_fromUtf8("horizontalLayout_38"))
        self.TminEdit = QtGui.QLineEdit(self.groupBox_22)
        self.TminEdit.setObjectName(_fromUtf8("TminEdit"))
        self.horizontalLayout_38.addWidget(self.TminEdit)
        self.TmaxEdit = QtGui.QLineEdit(self.groupBox_22)
        self.TmaxEdit.setObjectName(_fromUtf8("TmaxEdit"))
        self.horizontalLayout_38.addWidget(self.TmaxEdit)
        self.TmeanEdit = QtGui.QLineEdit(self.groupBox_22)
        self.TmeanEdit.setObjectName(_fromUtf8("TmeanEdit"))
        self.horizontalLayout_38.addWidget(self.TmeanEdit)
        self.TstdevEdit = QtGui.QLineEdit(self.groupBox_22)
        self.TstdevEdit.setObjectName(_fromUtf8("TstdevEdit"))
        self.horizontalLayout_38.addWidget(self.TstdevEdit)
        self.horizontalLayout_35.addWidget(self.groupBox_22)
        self.verticalLayout_57.addWidget(self.frame_13)
        self.frame_14 = QtGui.QFrame(self.tab_4)
        self.frame_14.setMinimumSize(QtCore.QSize(0, 40))
        self.frame_14.setMaximumSize(QtCore.QSize(16777215, 40))
        self.frame_14.setFrameShape(QtGui.QFrame.NoFrame)
        self.frame_14.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_14.setObjectName(_fromUtf8("frame_14"))
        self.horizontalLayout_39 = QtGui.QHBoxLayout(self.frame_14)
        self.horizontalLayout_39.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_39.setObjectName(_fromUtf8("horizontalLayout_39"))
        self.groupBox_23 = QtGui.QGroupBox(self.frame_14)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_23.sizePolicy().hasHeightForWidth())
        self.groupBox_23.setSizePolicy(sizePolicy)
        self.groupBox_23.setMinimumSize(QtCore.QSize(140, 40))
        self.groupBox_23.setMaximumSize(QtCore.QSize(140, 40))
        self.groupBox_23.setTitle(_fromUtf8(""))
        self.groupBox_23.setObjectName(_fromUtf8("groupBox_23"))
        self.horizontalLayout_40 = QtGui.QHBoxLayout(self.groupBox_23)
        self.horizontalLayout_40.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_40.setObjectName(_fromUtf8("horizontalLayout_40"))
        self.label_76 = QtGui.QLabel(self.groupBox_23)
        self.label_76.setAlignment(QtCore.Qt.AlignCenter)
        self.label_76.setObjectName(_fromUtf8("label_76"))
        self.horizontalLayout_40.addWidget(self.label_76)
        self.horizontalLayout_39.addWidget(self.groupBox_23)
        self.groupBox_24 = QtGui.QGroupBox(self.frame_14)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_24.sizePolicy().hasHeightForWidth())
        self.groupBox_24.setSizePolicy(sizePolicy)
        self.groupBox_24.setMinimumSize(QtCore.QSize(290, 40))
        self.groupBox_24.setMaximumSize(QtCore.QSize(290, 40))
        self.groupBox_24.setTitle(_fromUtf8(""))
        self.groupBox_24.setObjectName(_fromUtf8("groupBox_24"))
        self.horizontalLayout_41 = QtGui.QHBoxLayout(self.groupBox_24)
        self.horizontalLayout_41.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_41.setObjectName(_fromUtf8("horizontalLayout_41"))
        self.AUNRadio = QtGui.QRadioButton(self.groupBox_24)
        self.AUNRadio.setText(_fromUtf8(""))
        self.AUNRadio.setChecked(True)
        self.AUNRadio.setObjectName(_fromUtf8("AUNRadio"))
        self.horizontalLayout_41.addWidget(self.AUNRadio)
        self.ALURadio = QtGui.QRadioButton(self.groupBox_24)
        self.ALURadio.setText(_fromUtf8(""))
        self.ALURadio.setObjectName(_fromUtf8("ALURadio"))
        self.horizontalLayout_41.addWidget(self.ALURadio)
        self.ANORadio = QtGui.QRadioButton(self.groupBox_24)
        self.ANORadio.setText(_fromUtf8(""))
        self.ANORadio.setObjectName(_fromUtf8("ANORadio"))
        self.horizontalLayout_41.addWidget(self.ANORadio)
        self.ALNRadio = QtGui.QRadioButton(self.groupBox_24)
        self.ALNRadio.setText(_fromUtf8(""))
        self.ALNRadio.setObjectName(_fromUtf8("ALNRadio"))
        self.horizontalLayout_41.addWidget(self.ALNRadio)
        self.horizontalLayout_39.addWidget(self.groupBox_24)
        self.groupBox_25 = QtGui.QGroupBox(self.frame_14)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_25.sizePolicy().hasHeightForWidth())
        self.groupBox_25.setSizePolicy(sizePolicy)
        self.groupBox_25.setMinimumSize(QtCore.QSize(0, 40))
        self.groupBox_25.setMaximumSize(QtCore.QSize(16777215, 40))
        self.groupBox_25.setTitle(_fromUtf8(""))
        self.groupBox_25.setObjectName(_fromUtf8("groupBox_25"))
        self.horizontalLayout_42 = QtGui.QHBoxLayout(self.groupBox_25)
        self.horizontalLayout_42.setContentsMargins(-1, 1, -1, 1)
        self.horizontalLayout_42.setObjectName(_fromUtf8("horizontalLayout_42"))
        self.AminEdit = QtGui.QLineEdit(self.groupBox_25)
        self.AminEdit.setObjectName(_fromUtf8("AminEdit"))
        self.horizontalLayout_42.addWidget(self.AminEdit)
        self.AmaxEdit = QtGui.QLineEdit(self.groupBox_25)
        self.AmaxEdit.setObjectName(_fromUtf8("AmaxEdit"))
        self.horizontalLayout_42.addWidget(self.AmaxEdit)
        self.AmeanEdit = QtGui.QLineEdit(self.groupBox_25)
        self.AmeanEdit.setObjectName(_fromUtf8("AmeanEdit"))
        self.horizontalLayout_42.addWidget(self.AmeanEdit)
        self.AstdevEdit = QtGui.QLineEdit(self.groupBox_25)
        self.AstdevEdit.setObjectName(_fromUtf8("AstdevEdit"))
        self.horizontalLayout_42.addWidget(self.AstdevEdit)
        self.horizontalLayout_39.addWidget(self.groupBox_25)
        self.verticalLayout_57.addWidget(self.frame_14)
        self.tabWidget.addTab(self.tab_4, _fromUtf8(""))
        self.verticalLayout.addWidget(self.tabWidget)
        self.frame_10 = QtGui.QFrame(self.centralwidget)
        self.frame_10.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame_10.setFrameShadow(QtGui.QFrame.Raised)
        self.frame_10.setObjectName(_fromUtf8("frame_10"))
        self.horizontalLayout_26 = QtGui.QHBoxLayout(self.frame_10)
        self.horizontalLayout_26.setObjectName(_fromUtf8("horizontalLayout_26"))
        self.cancelButton = QtGui.QPushButton(self.frame_10)
        self.cancelButton.setObjectName(_fromUtf8("cancelButton"))
        self.horizontalLayout_26.addWidget(self.cancelButton)
        self.saveButton = QtGui.QPushButton(self.frame_10)
        self.saveButton.setObjectName(_fromUtf8("saveButton"))
        self.horizontalLayout_26.addWidget(self.saveButton)
        self.verticalLayout.addWidget(self.frame_10)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 1000, 23))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName(_fromUtf8("statusbar"))
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        self.tabWidget.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QtGui.QApplication.translate("MainWindow", "MainWindow", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("MainWindow", "Address of the server", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("MainWindow", "Port", None, QtGui.QApplication.UnicodeUTF8))
        self.addrEdit.setText(QtGui.QApplication.translate("MainWindow", "localhost", None, QtGui.QApplication.UnicodeUTF8))
        self.portEdit.setText(QtGui.QApplication.translate("MainWindow", "666", None, QtGui.QApplication.UnicodeUTF8))
        self.connectButton.setText(QtGui.QApplication.translate("MainWindow", "Connect", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab), QtGui.QApplication.translate("MainWindow", "Connexion", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("MainWindow", "Path to the executable file", None, QtGui.QApplication.UnicodeUTF8))
        self.execBrowseButton.setText(QtGui.QApplication.translate("MainWindow", "browse", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("MainWindow", "Graphics and pictures save format\n"
"(scenario trees, PCA graphics)", None, QtGui.QApplication.UnicodeUTF8))
        self.formatCombo.setItemText(0, QtGui.QApplication.translate("MainWindow", "svg", None, QtGui.QApplication.UnicodeUTF8))
        self.formatCombo.setItemText(1, QtGui.QApplication.translate("MainWindow", "jpg", None, QtGui.QApplication.UnicodeUTF8))
        self.formatCombo.setItemText(2, QtGui.QApplication.translate("MainWindow", "png", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("MainWindow", "Style", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_2), QtGui.QApplication.translate("MainWindow", "Various", None, QtGui.QApplication.UnicodeUTF8))
        self.setMutSeqLabel_2.setText(QtGui.QApplication.translate("MainWindow", "Default values of historical model", None, QtGui.QApplication.UnicodeUTF8))
        self.label_64.setText(QtGui.QApplication.translate("MainWindow", "parameters", None, QtGui.QApplication.UnicodeUTF8))
        self.label_67.setText(QtGui.QApplication.translate("MainWindow", "Uniform", None, QtGui.QApplication.UnicodeUTF8))
        self.label_68.setText(QtGui.QApplication.translate("MainWindow", "Log-uniform", None, QtGui.QApplication.UnicodeUTF8))
        self.label_69.setText(QtGui.QApplication.translate("MainWindow", "Normal", None, QtGui.QApplication.UnicodeUTF8))
        self.label_70.setText(QtGui.QApplication.translate("MainWindow", "Log-normal", None, QtGui.QApplication.UnicodeUTF8))
        self.label_71.setText(QtGui.QApplication.translate("MainWindow", "minimum", None, QtGui.QApplication.UnicodeUTF8))
        self.label_72.setText(QtGui.QApplication.translate("MainWindow", "maximum", None, QtGui.QApplication.UnicodeUTF8))
        self.label_73.setText(QtGui.QApplication.translate("MainWindow", "mean", None, QtGui.QApplication.UnicodeUTF8))
        self.label_74.setText(QtGui.QApplication.translate("MainWindow", "st-deviation", None, QtGui.QApplication.UnicodeUTF8))
        self.label_75.setText(QtGui.QApplication.translate("MainWindow", "Effective\n"
"population size", None, QtGui.QApplication.UnicodeUTF8))
        self.NminEdit.setText(QtGui.QApplication.translate("MainWindow", "10", None, QtGui.QApplication.UnicodeUTF8))
        self.NmaxEdit.setText(QtGui.QApplication.translate("MainWindow", "10000", None, QtGui.QApplication.UnicodeUTF8))
        self.NmeanEdit.setText(QtGui.QApplication.translate("MainWindow", "0", None, QtGui.QApplication.UnicodeUTF8))
        self.NstdevEdit.setText(QtGui.QApplication.translate("MainWindow", "0", None, QtGui.QApplication.UnicodeUTF8))
        self.label_77.setText(QtGui.QApplication.translate("MainWindow", "Time in generation", None, QtGui.QApplication.UnicodeUTF8))
        self.TminEdit.setText(QtGui.QApplication.translate("MainWindow", "10", None, QtGui.QApplication.UnicodeUTF8))
        self.TmaxEdit.setText(QtGui.QApplication.translate("MainWindow", "10000", None, QtGui.QApplication.UnicodeUTF8))
        self.TmeanEdit.setText(QtGui.QApplication.translate("MainWindow", "0", None, QtGui.QApplication.UnicodeUTF8))
        self.TstdevEdit.setText(QtGui.QApplication.translate("MainWindow", "0", None, QtGui.QApplication.UnicodeUTF8))
        self.label_76.setText(QtGui.QApplication.translate("MainWindow", "Admixture\n"
"coefficient", None, QtGui.QApplication.UnicodeUTF8))
        self.AminEdit.setText(QtGui.QApplication.translate("MainWindow", "0.001", None, QtGui.QApplication.UnicodeUTF8))
        self.AmaxEdit.setText(QtGui.QApplication.translate("MainWindow", "0.999", None, QtGui.QApplication.UnicodeUTF8))
        self.AmeanEdit.setText(QtGui.QApplication.translate("MainWindow", "0", None, QtGui.QApplication.UnicodeUTF8))
        self.AstdevEdit.setText(QtGui.QApplication.translate("MainWindow", "0", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_4), QtGui.QApplication.translate("MainWindow", "Historical", None, QtGui.QApplication.UnicodeUTF8))
        self.cancelButton.setText(QtGui.QApplication.translate("MainWindow", "Cancel", None, QtGui.QApplication.UnicodeUTF8))
        self.cancelButton.setShortcut(QtGui.QApplication.translate("MainWindow", "Esc", None, QtGui.QApplication.UnicodeUTF8))
        self.saveButton.setText(QtGui.QApplication.translate("MainWindow", "Save", None, QtGui.QApplication.UnicodeUTF8))

