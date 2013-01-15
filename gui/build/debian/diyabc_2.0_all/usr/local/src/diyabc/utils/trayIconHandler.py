# -*- coding: utf-8 -*-

## @package utils.trayIconHandler
# @author Julien Veyssier
#
# @brief class to manipulate system tray icon

from PyQt4.QtCore import *
from PyQt4.QtGui import *

class TrayIconHandler(QObject):
    """ Class to manipulate system tray icon

    As far as i could test, all functionnalities work on :
        - GNU/Linux (Gnome 2.* , KDE , *box , Xfce4)
        - Windows (XP)
        - MacOS (10.6)

    automatic behaviour :
        - display selected icon in the systray
        - right click (on the icon) access to a QMenu
        - show/hide the parent QWidget when the trayicon is clicked

    functionnalities accessible by method calling :
        - show/hide the systray icon
        - display notifications
        - play a gif instead of the icon image (to draw user's attention to an event for example)
    """
    def __init__(self,iconpath,icongifpath=None,menu=None,parent=None):
        """
        @param menu: QMenu to show on right click on the systray icon
        @param parent: QWidget which will be shown/hidden on left click on the systray icon
        """
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

    def showTrayMessage(self,title,msg):
        """ Print a systray notification if the icon is visible. If the parent
        window is not visible, enters in blink mode
        """
        if self.systray.isVisible():
            self.systray.showMessage(title,msg)
            if not self.parent.isActiveWindow():
                self.enterSystrayBlink()

    def enterSystrayBlink(self):
        """ Initiates animation of the systray icon
        """
        if self.icongifpath != None:
            self.m_movie.start()

    def leaveSystrayBlink(self):
        """ Stops animation of systray icon
        """
        if self.icongifpath != None:
            self.m_movie.stop()
            self.systray.setIcon(QIcon(self.iconpath))

    @pyqtSignature("")
    def updateTrayIcon(self):
        """ Change the systray icon by the current image of the playing QMovie
        Triggered by the image change of the QMovie
        """
        icon = QIcon(self.m_movie.currentPixmap())
        self.systray.setIcon(icon)
