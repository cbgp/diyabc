# -*- coding: utf-8 -*-

## @package python_interface.preferences
# @author Julien Veyssier
#
# @brief Fenêtre pour gérer les préférences personnelles

import os,sys,platform,multiprocessing
import ConfigParser
import codecs
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from PyQt4 import QtCore
import output
import utils.cbgpUtils as utilsPack
from utils.cbgpUtils import log
from utils.configobj.configobj import *

visible = "1"
invisible = "0"

class AutoPreferences(QFrame):
    """ Class to manage big amount of user settings into a Qt application

    This class inherits from QFrame and does the following jobs :
        - display the user settings in a tabWidget (each category of field is a tab)
        - save/load automaticaly the settings in a given config file using ConfigObj
          where the structure is the same as the tabWidget (categories, fields)
        - ability to manage invisible fields for 'non-user set' value (like toolbar
          position for example)

    To use an instance of this class, create a hashtable describing the structure
    of your settings and call the 'digest' method to initialize the data and the
    QFrame shape. For example : 

    >>> myconfStructure = {
    ...     category_1 : [
    ...         ["check","showTrayIcon","Show tray icon",False],
    ...         ["check","showTrayIcon","Show tray icon",False]
    ...     ]
    ...     category_2 : [
    ...         ["check","showTrayIcon","Show tray icon",False],
    ...         ["check","showTrayIcon","Show tray icon",False]
    ...     ]
    ... }
    >>> myconfManager = AutoPreferences(None,"/home/user/.myapp/config.cfg","Settings")
    >>> myconfManager.digest(myconfStructure)

    See the add* methods to know what to put in the list representing fields

    You can add different kind of fields to the settings :
        - "check" : labeled check box
        - "lineEdit" : labeled lineEdit
        - "combo" : labeled combo box
        - "pathEdit : labeled line edit with a 'browse' button to select a path with 
          a Qt file dialog

    It is of course possible to inherit from this class to add more complicated 
    functionnalities like handling field change events or adding complex settings
    which call external views...
    """
    def __init__(self,parent=None,confFilePath=None,title="Settings"):
        super(AutoPreferences,self).__init__(parent)
        self.parent = parent
        self.title = title

        # dico indexé par le nom de la catégorie qui contient la liste des propriétés
        self.dicoCategory = {}
        # liste pour retenir l'ordre des catégories
        self.categoryList = []

        self.configFile = confFilePath
        self.config = ConfigObj(self.configFile)

        self.createWidgets()

    def createWidgets(self):
        self.ui = self

        self.ui.verticalLayout = QtGui.QVBoxLayout(self)
        self.ui.verticalLayout.setObjectName("centralVerticalLayout")

        self.ui.frame_title = QtGui.QFrame(self)
        self.ui.frame_title.setFrameShape(QtGui.QFrame.StyledPanel)
        self.ui.frame_title.setFrameShadow(QtGui.QFrame.Raised)
        self.ui.frame_title.setObjectName("frame_title")
        self.ui.frame_title.setFrameShape(QtGui.QFrame.NoFrame)
        self.ui.horizontalLayout_title = QtGui.QHBoxLayout(self.frame_title)
        self.ui.horizontalLayout_title.setObjectName("horizontalLayout_title")
        self.ui.titleLabel = QtGui.QLabel(self.frame_title,)
        self.ui.titleLabel.setText(self.title)
        self.ui.titleLabel.setAlignment(Qt.AlignHCenter)
        self.ui.titleLabel.setObjectName("titleLabel")
        font = QFont()
        font.setPixelSize(15)
        font.setBold(True)
        self.ui.titleLabel.setFont(font)
        self.ui.horizontalLayout_title.addWidget(self.titleLabel)
        self.ui.verticalLayout.addWidget(self.frame_title)

        self.ui.tabWidget = QtGui.QTabWidget(self)
        self.ui.tabWidget.setObjectName("tabWidget")
        self.ui.verticalLayout.addWidget(self.ui.tabWidget)

        self.ui.frame_button = QtGui.QFrame(self)
        self.ui.frame_button.setFrameShape(QtGui.QFrame.StyledPanel)
        self.ui.frame_button.setFrameShadow(QtGui.QFrame.Raised)
        self.ui.frame_button.setObjectName("frame_button")
        self.ui.horizontalLayout_26 = QtGui.QHBoxLayout(self.frame_button)
        self.ui.horizontalLayout_26.setObjectName("horizontalLayout_26")
        self.ui.cancelPreferencesButton = QtGui.QPushButton(self.frame_button)
        self.ui.cancelPreferencesButton.setText("Cancel")
        self.ui.cancelPreferencesButton.setShortcut(QtGui.QApplication.translate("MainWindow", "Esc", None, QtGui.QApplication.UnicodeUTF8))
        self.ui.cancelPreferencesButton.setObjectName("cancelPreferencesButton")
        self.ui.horizontalLayout_26.addWidget(self.cancelPreferencesButton)
        self.ui.savePreferencesButton = QtGui.QPushButton(self.frame_button)
        self.ui.savePreferencesButton.setText("Save")
        self.ui.savePreferencesButton.setObjectName("savePreferencesButton")
        self.ui.horizontalLayout_26.addWidget(self.savePreferencesButton)
        self.ui.verticalLayout.addWidget(self.frame_button)

        QObject.connect(self.ui.savePreferencesButton,SIGNAL("clicked()"),self.savePreferences)
        QObject.connect(self.ui.cancelPreferencesButton,SIGNAL("clicked()"),self.cancel)

    def digest(self,dico_fields):
        """ Takes a hashtable as a parameter to build the graphical preference structure
        @param dico_fields: hashtable which describes the settings structure
        """
        for cat in dico_fields.keys():
            self.addCategory(cat)
            for f in dico_fields[cat]:
                # *f[1:] is equivalent to f[1],f[2],f[3]... in a function call contex
                #exec('self.addProp%s(cat,*f[1:])'%(f[0][0].upper() + f[0][1:]))
                proptype = f["proptype"]
                f.pop("proptype")
                exec('self.addProp%s(cat,**f)'%(proptype[0].upper() + proptype[1:]))
                #if f[0] == "combo":
                #    self.addPropCombo(cat,f[1],f[2],f[3],f[4],f[5])
                #elif f[0] == "check":
                #    self.addPropCheck(cat,f[1],f[2],f[3])
                #elif f[0] == "lineEdit":
                #    self.addPropLineEdit(cat,f[1],f[2],f[3],f[4])
                #elif f[0] == "pathEdit":
                #    self.addPropPathEdit(cat,f[1],f[2],f[3])

    def addCategory(self,catname):
        """ Add a category i.e a tab which will contain fields
        @param catname: name of the new category
        """
        if catname in self.categoryList:
            raise Exception("Category already exists")
        else:
            self.categoryList.append(catname)
            self.dicoCategory[catname] = []

            exec("self.tab_%s = QtGui.QWidget() "%catname)
            exec("self.tab_%s.setObjectName('tab_%s')"%(catname,catname))
            exec('self.verticalLayout_%s = QtGui.QVBoxLayout(self.tab_%s) '%(catname,catname))
            exec('self.verticalLayout_%s.setObjectName("verticalLayout_%s") '%(catname,catname))
            exec('self.scrollArea_%s = QtGui.QScrollArea(self.tab_%s) '%(catname,catname))
            exec('self.scrollArea_%s.setWidgetResizable(True) '%(catname))
            exec('self.scrollArea_%s.setObjectName("scrollArea_%s") '%(catname,catname))
            exec('self.scrollAreaWidgetContents_%s = QtGui.QWidget() '%(catname))
            exec('self.scrollAreaWidgetContents_%s.setGeometry(QtCore.QRect(0, 0, 956, 589)) '%(catname))
            exec('self.scrollAreaWidgetContents_%s.setObjectName("scrollAreaWidgetContents_%s") '%(catname,catname))
            exec('self.scrollArea_%s.setWidget(self.scrollAreaWidgetContents_%s)'%(catname,catname))
            exec('self.verticalLayoutScroll_%s = QtGui.QVBoxLayout(self.scrollAreaWidgetContents_%s) '%(catname,catname))
            exec('self.verticalLayoutScroll_%s.setObjectName("verticalLayoutScroll_%s") '%(catname,catname))
            exec('self.ui.verticalLayoutScroll_%s.setAlignment(Qt.AlignTop)'%catname)
            exec('self.verticalLayout_%s.addWidget(self.scrollArea_%s) '%(catname,catname))
            exec('self.ui.tabWidget.addTab(self.tab_%s,"%s")'%(catname,catname))

    def addPropCheck(self,catname,propname,labelText,defaultState):
        """ Add a field which is composed by a label and a checkBox
        @param propname: name of the field
        @param labelText: text associated to the field (label)
        @param defaultState: checked or unchecked (True,False)
        """
        self.dicoCategory[catname].append([propname,"check",labelText,defaultState])

        exec('self.frame_%s_%s = QtGui.QFrame(self.scrollAreaWidgetContents_%s) '%(catname,propname,catname))
        exec('self.frame_%s_%s.setFrameShape(QtGui.QFrame.StyledPanel) '%(catname,propname))
        exec('self.frame_%s_%s.setFrameShadow(QtGui.QFrame.Raised) '%(catname,propname))
        exec('self.frame_%s_%s.setObjectName("frame_%s_%s") '%(catname,propname,catname,propname))
        exec('self.horizontalLayout_%s_%s = QtGui.QHBoxLayout(self.frame_%s_%s) '%(catname,propname,catname,propname))
        exec('self.horizontalLayout_%s_%s.setObjectName("horizontalLayout_%s_%s") '%(catname,propname,catname,propname))
        exec('self.%sCheck = QtGui.QCheckBox(self.frame_%s_%s) '%(propname,catname,propname))
        exec('self.%sCheck.setText("%s") '%(propname,labelText))
        exec('self.%sCheck.setChecked(defaultState) '%(propname))
        exec('self.%sCheck.setObjectName("%sCheck") '%(propname,propname))
        exec('self.horizontalLayout_%s_%s.addWidget(self.%sCheck) '%(catname,propname,propname))
        exec('self.verticalLayoutScroll_%s.addWidget(self.frame_%s_%s) '%(catname,catname,propname))

    def addPropCombo(self,catname,propname,labelText,dicoValTxt,l_ordered_val,default_value=None):
        """ Add a field which is composed by a label and a comboBox
        @param propname: name of the field
        @param labelText: text associated to the field (label)
        """
        self.dicoCategory[catname].append( [propname,"combo",dicoValTxt,l_ordered_val,default_value,labelText] )

        exec('self.frame_%s_%s = QtGui.QFrame(self.scrollAreaWidgetContents_%s)'%(catname,propname,catname))
        exec('self.frame_%s_%s.setFrameShape(QtGui.QFrame.StyledPanel)'%(catname,propname))
        exec('self.frame_%s_%s.setFrameShadow(QtGui.QFrame.Raised)'%(catname,propname))
        exec('self.frame_%s_%s.setObjectName("frame_%s_%s")'%(catname,propname,catname,propname))
        exec('self.horizontalLayout_%s_%s = QtGui.QHBoxLayout(self.frame_%s_%s)'%(catname,propname,catname,propname))
        exec('self.horizontalLayout_%s_%s.setObjectName("horizontalLayout_%s_%s")'%(catname,propname,catname,propname))
        exec('self.label_%s = QtGui.QLabel(self.frame_%s_%s)'%(propname,catname,propname))
        exec('self.label_%s.setText("%s")'%(propname,labelText))
        exec('self.label_%s.setObjectName("label_%s")'%(propname,propname))
        exec('self.horizontalLayout_%s_%s.addWidget(self.label_%s)'%(catname,propname,propname))
        exec('self.%sCombo = QtGui.QComboBox(self.frame_%s_%s)'%(propname,catname,propname))
        exec('self.%sCombo.setObjectName("%sCombo")'%(propname,propname))
        exec('self.horizontalLayout_%s_%s.addWidget(self.%sCombo)'%(catname,propname,propname))
        exec('self.verticalLayoutScroll_%s.addWidget(self.frame_%s_%s)'%(catname,catname,propname))

        for k in l_ordered_val:
            exec('self.%sCombo.addItem("%s")'%(propname,dicoValTxt[k]))
        if default_value != None and dicoValTxt.has_key(default_value):
            exec('ind = self.ui.%sCombo.findText(dicoValTxt[default_value])'%propname)
            if ind != -1:
                exec('self.%sCombo.setCurrentIndex(%s)'%(propname,ind))

    def addPropLineEdit(self,catname,propname,labelText,default_value="",visibility=visible):
        """ Add a field which is composed by a label and a lineEdit
        The visibility can be set with a boolean parameter for example to save a 'non-user-set' value
        @param propname: name of the field
        @param labelText: text associated to the field (label)
        """
        self.dicoCategory[catname].append( [propname, "lineEdit", labelText, default_value] )

        exec('self.frame_%s_%s = QtGui.QFrame(self.scrollAreaWidgetContents_%s)'%(catname,propname,catname))
        exec('self.frame_%s_%s.setFrameShape(QtGui.QFrame.StyledPanel)'%(catname,propname))
        exec('self.frame_%s_%s.setFrameShadow(QtGui.QFrame.Raised)'%(catname,propname))
        exec('self.frame_%s_%s.setObjectName("frame_%s_%s")'%(catname,propname,catname,propname))
        exec('self.horizontalLayout_%s = QtGui.QHBoxLayout(self.frame_%s_%s)'%(propname,catname,propname))
        exec('self.horizontalLayout_%s.setObjectName("horizontalLayout_%s")'%(propname,propname))
        exec('self.label_%s = QtGui.QLabel(self.frame_%s_%s)'%(propname,catname,propname))
        exec('self.label_%s.setText("%s")'%(propname,labelText))
        exec('self.label_%s.setObjectName("label_%s")'%(propname,propname))
        exec('self.horizontalLayout_%s.addWidget(self.label_%s)'%(propname,propname))
        exec('self.%sEdit = QtGui.QLineEdit(self.frame_%s_%s)'%(propname,catname,propname))
        exec('self.%sEdit.setMinimumSize(QtCore.QSize(150, 0))'%(propname))
        exec('self.%sEdit.setMaximumSize(QtCore.QSize(150, 16777215))'%(propname))
        exec('self.%sEdit.setText("%s")'%(propname,default_value))
        exec('self.%sEdit.setObjectName("%sEdit")'%(propname,propname))
        exec('self.%sEdit.setAlignment(Qt.AlignRight)'%(propname))
        exec('self.horizontalLayout_%s.addWidget(self.%sEdit)'%(propname,propname))
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        exec('self.horizontalLayout_%s.addItem(spacerItem)'%(propname))
        exec('self.verticalLayoutScroll_%s.addWidget(self.frame_%s_%s)'%(catname,catname,propname))
        if visibility == invisible:
            #print catname,propname," invisible"
            exec('self.frame_%s_%s.setVisible(False)'%(catname,propname))

    def addPropTextEdit(self,catname,propname,labelText,default_value="",visibility=visible, readOnly=False):
        """ Add a field which is composed by a label and a QTextEdit
        The visibility can be set with a boolean parameter for example to save a 'non-user-set' value
        @param propname: name of the field
        @param labelText: text associated to the field (label)
        """
        self.dicoCategory[catname].append( [propname, "textEdit", labelText, default_value] )

        exec('self.frame_%s_%s = QtGui.QFrame(self.scrollAreaWidgetContents_%s)'%(catname,propname,catname))
        exec('self.frame_%s_%s.setFrameShape(QtGui.QFrame.StyledPanel)'%(catname,propname))
        exec('self.frame_%s_%s.setFrameShadow(QtGui.QFrame.Raised)'%(catname,propname))
        exec('self.frame_%s_%s.setObjectName("frame_%s_%s")'%(catname,propname,catname,propname))
        exec('self.horizontalLayout_%s = QtGui.QHBoxLayout(self.frame_%s_%s)'%(propname,catname,propname))
        exec('self.horizontalLayout_%s.setObjectName("horizontalLayout_%s")'%(propname,propname))
        exec('self.label_%s = QtGui.QLabel(self.frame_%s_%s)'%(propname,catname,propname))
        exec('self.label_%s.setText("%s")'%(propname,labelText))
        exec('self.label_%s.setObjectName("label_%s")'%(propname,propname))
        exec('self.horizontalLayout_%s.addWidget(self.label_%s)'%(propname,propname))
        exec('self.%sTextEdit = QtGui.QTextEdit(self.frame_%s_%s)'%(propname,catname,propname))
        #exec('self.%sTextEdit.setMinimumSize(QtCore.QSize(150, 0))'%(propname))
        #exec('self.%sTextEdit.setMaximumSize(QtCore.QSize(150, 16777215))'%(propname))
        exec('self.%sTextEdit.setObjectName("%sEdit")'%(propname,propname))
        exec('self.%sTextEdit.setAlignment(Qt.AlignLeft)'%(propname))
        exec('self.horizontalLayout_%s.addWidget(self.%sTextEdit)'%(propname,propname))
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        exec('self.horizontalLayout_%s.addItem(spacerItem)'%(propname))
        exec('self.verticalLayoutScroll_%s.addWidget(self.frame_%s_%s)'%(catname,catname,propname))
        exec('self.%sTextEdit.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOn)'%(propname))
        exec('self.%sTextEdit.setLineWrapMode(QtGui.QTextEdit.NoWrap)'%(propname))
        if visibility == invisible:
            #print catname,propname," invisible"
            exec('self.frame_%s_%s.setVisible(False)'%(catname,propname))
        if readOnly :
            exec('self.%sTextEdit.setReadOnly(True)'%(propname))
            
        #exec('self.%sTextEdit.setPlainText("""%s""")'%(propname,default_value))
        for l in default_value.splitlines() :
            #l = l + "\n"
            exec('self.%sTextEdit.append(QString("""%s"""))' % (propname, l))


    def addPropPathEdit(self,catname,propname,labelText,default_value=""):
        """ Add a field which is composed by a label, a 'browse' button and a lineEdit
        @param propname: name of the field
        @param labelText: text associated to the field (label)
        """
        self.dicoCategory[catname].append( [propname, "path", labelText, default_value] )

        exec('self.frame_%s_%s = QtGui.QFrame(self.scrollAreaWidgetContents_%s)'%(catname,propname,catname))
        exec('self.frame_%s_%s.setFrameShape(QtGui.QFrame.StyledPanel)'%(catname,propname))
        exec('self.frame_%s_%s.setFrameShadow(QtGui.QFrame.Raised)'%(catname,propname))
        exec('self.frame_%s_%s.setObjectName("frame_%s_%s")'%(catname,propname,catname,propname))
        exec('self.horizontalLayout_%s = QtGui.QHBoxLayout(self.frame_%s_%s)'%(propname,catname,propname))
        exec('self.horizontalLayout_%s.setObjectName("horizontalLayout_%s")'%(propname,propname))
        exec('self.label_%s = QtGui.QLabel(self.frame_%s_%s)'%(propname,catname,propname))
        exec('self.label_%s.setText("%s")'%(propname,labelText))
        exec('self.label_%s.setObjectName("label_%s")'%(propname,propname))
        exec('self.horizontalLayout_%s.addWidget(self.label_%s)'%(propname,propname))
        exec('self.%sBrowseButton = QtGui.QPushButton(self.frame_%s_%s)'%(propname,catname,propname))
        exec('self.%sBrowseButton.setText("browse")'%(propname))
        exec('self.%sBrowseButton.setObjectName("%sBrowseButton")'%(propname,propname))
        exec('self.horizontalLayout_%s.addWidget(self.%sBrowseButton)'%(propname,propname))
        exec('self.%sPathEdit = QtGui.QLineEdit(self.frame_%s_%s)'%(propname,catname,propname))
        exec('self.%sPathEdit.setObjectName("%sPathEdit")'%(propname,propname))
        qc = QCompleter()
        qc.setModel(QDirModel(qc))
        exec('self.%sPathEdit.setCompleter(qc)'%(propname))
        exec('self.horizontalLayout_%s.addWidget(self.%sPathEdit)'%(propname,propname))
        exec('self.verticalLayoutScroll_%s.addWidget(self.frame_%s_%s)'%(catname,catname,propname))

        exec('QObject.connect(self.ui.%sBrowseButton,SIGNAL("clicked()"),self.browse)'%(propname))

    def browse(self):
        """ Open a qt browsing window to fill interactively the pathEdit fields
        """
        but = self.sender()
        ed = but.parent().findChild(QLineEdit)
        qfd = QFileDialog()
        path = str(qfd.getOpenFileName(self,"Where is %s ?"%(str(ed.objectName()).replace("PathEdit",""))))
        ed.setText(path)

    def savePreferences(self):
        """ Automatically save values of fields in a ConfigObj config file
        """
        for cat in self.categoryList:
            if not self.config.has_key(cat):
                self.config[cat] = {}

            for propl in self.dicoCategory[cat]:
                propname = propl[0]
                proptype = propl[1]
                if proptype == "check":
                    exec('val_to_save = str(self.%sCheck.isChecked())'%propname)
                elif proptype == "lineEdit":
                    exec('val_to_save = str(self.ui.%sEdit.text())'%propname)
                elif proptype == "textEdit":
                    exec('val_to_save = str(self.ui.%sTextEdit.toPlainText())'%propname)                    
                elif proptype == "path":
                    exec('val_to_save = str(self.ui.%sPathEdit.text())'%propname)
                elif proptype == "combo":
                    exec('txt = str(self.ui.%sCombo.currentText())'%propname)
                    for k in propl[2].keys():
                        a = propl[2][k]
                        if a == txt:
                            break
                    val_to_save = k

                self.config[cat][propname] = val_to_save
        
        self.writeConfigFile()

    def loadPreferences(self):
        """ Automatically match config file and preferences fields and fill values
        """
        cfg = self.config

        for cat in self.dicoCategory.keys():
            if cfg.has_key(cat):
                for propl in self.dicoCategory[cat]:
                    propname = propl[0]
                    if cfg[cat].has_key(propname):
                        val = cfg[cat][propname]
                        proptype = propl[1]
                        if proptype == "check":
                            checked = (val.lower() == "true")
                            exec('self.ui.%sCheck.setChecked(not checked)'%propname)
                            exec('self.ui.%sCheck.setChecked(checked)'%propname)
                        elif proptype == "lineEdit":
                            exec('self.ui.%sEdit.setText(str(val).strip())'%propname)
                        elif proptype == "textEdit":
                            exec('self.ui.%sTextEdit.setText(str(val))'%propname)                            
                        elif proptype == "path":
                            exec('self.ui.%sPathEdit.setText(str(val).strip())'%propname)
                        elif proptype == "combo":
                            #print('ind = self.ui.%sCombo.findText(propl[2][str(val)])'%propname)
                            if propl[2].has_key(str(val)):
                                exec('ind = self.ui.%sCombo.findText(propl[2][str(val)])'%propname)
                                if ind != -1:
                                    exec('self.ui.%sCombo.setCurrentIndex(ind)'%propname)

    def cancel(self):
        """ To cancel, the config is re-loaded from the last saved state (the file)
        """
        self.loadPreferences()
        self.close()

    def writeConfigFile(self):
        self.config.write()

    def allValid(self):
        return True
        
