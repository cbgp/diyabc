# -*- coding: utf-8 -*-

## @package utils.trayIconHandler
# @author Julien Veyssier
#
# @brief class to manipulate system tray icon

from PyQt4.QtCore import *
from PyQt4.QtGui import *

class TrayIconHandler(QObject):

    def __init__(self,iconpath,icongifpath=None,menu=None,parent=None):
        super(TrayIconHandler,self).__init__(parent)
        self.parent=parent
        self.iconpath = iconpath
        self.icongifpath = icongifpath

        self.systray = QSystemTrayIcon(QIcon(iconpath),parent)
        if menu != None:
            self.systray.setContextMenu(menu)
        #self.systray.show()
        QObject.connect(self.systray,SIGNAL("activated(QSystemTrayIcon::ActivationReason)"),self.systrayClicked)
        
        if icongifpath != None:
            self.m_movie = QMovie()
            self.m_movie.setFileName(icongifpath)
            #self.m_movie.start()
            QObject.connect(self.m_movie, SIGNAL("updated ( const QRect & )"),self,SLOT("updateTrayIcon()"))

    def hide(self):
        self.systray.hide()

    def systrayClicked(self,reason):
        if reason == QSystemTrayIcon.Trigger:
            self.parent.setVisible(not self.parent.isVisible())

    def toggleTrayIcon(self,state):
        self.systray.setVisible(state)

    def showTrayMessage(self,tit,msg):
        """ Affiche un message dans le systray ssi le systray et visible
        Si la diyabc n'est pas visible, fait clignoter le systrayicon
        """
        if self.systray.isVisible():
            self.systray.showMessage(tit,msg)
            if not self.parent.isActiveWindow():
                self.enterSystrayBlink()

    def enterSystrayBlink(self):
        """ Initie le clignotement de l'icone du systray
        """
        if self.icongifpath != None:
            self.m_movie.start()

    def leaveSystrayBlink(self):
        """ Stoppe le clignottement de l'icone du systray
        """
        if self.icongifpath != None:
            self.m_movie.stop()
            self.systray.setIcon(QIcon(self.iconpath))

    @pyqtSignature("")
    def updateTrayIcon(self):
        """ anime l'icone du systray en changeant son image
        Est déclenchée par un évennement d'un QMovie
        """
        icon = QIcon(self.m_movie.currentPixmap())
        self.systray.setIcon(icon)
