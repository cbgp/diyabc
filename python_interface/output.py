# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
import os,sys
from datetime import datetime 

debug = False
LOG_LEVEL = 3

RED='\033[31m'
WHITE='\033[37m'
BLUE='\033[35m'
GREEN='\033[32m'
tabcolors=[WHITE,RED,BLUE,GREEN]

def notify(parent,title,message):
    # dans tous les cas, on logue
    log(2,message)
    # en temps normal on signale aussi à l'utilisateur
    if not debug:
        QMessageBox.information(parent,title,message)

def centerHeader(name,nbChar):
    if len(name) > nbChar:
        return name
    else:
        nbSpToAdd = nbChar - len(name)
        spBef = ""
        for i in range(nbSpToAdd/2):
            spBef+=" "
        if (nbSpToAdd % 2) == 1:
            spAft = spBef+" "
        else:
            spAft = spBef
        return spBef+name+spAft

class Tee(object):
    """ Classe qui remplace stdout et stderr pour logger sur 
    la sortie standard (ancienne stdout) et dans un fichier simultanément.
    Effectue aussi une sorte de logrotate
    ATTENTION effet de bord : stderr n'est plus différentié
    il est écrit dans stdout et un fichier
    """
    def __init__(self, name, mode, app):
        """ raccourci le fichier de log s'il est trop grand 
        et remplace stdout
        """
        self.app=app
        self.logRotate(name)

        self.file = open(name, mode)
        self.stdout = sys.stdout
        #sys.stdout = self
    def __del__(self):
        sys.stdout = self.stdout
        self.file.close()
    def write(self, data):
        data_without_color = data.replace(RED,'').replace(WHITE,'').replace(GREEN,'').replace(BLUE,'')
        #self.app.showLogFile_win.logText.moveCursor(QTextCursor.End)
        #self.app.showLogFile_win.logText.appendHtml("%s"%data_without_color.strip())
        #self.app.showLogFile_win.logText.insert("%s"%data_without_color.strip(),checkNewLine=True)
        self.file.write(data_without_color)
        self.file.flush()
        self.stdout.write(data)
    def logRotate(self,name):
        if os.path.exists(name):
            f=open(name,'r')
            lines = f.readlines()
            # si on a plus de 5000 lignes, on ne garde que les 3000 dernières
            if len(lines) > 5000:
                keptLines = lines[-3000:]
                f.close()
                fw=open(name,'w')
                fw.write(''.join(keptLines))
                fw.close()
                return
            f.close()

def log(level,message):
    if level <= LOG_LEVEL:
        dd = datetime.now()
        color = tabcolors[level]
        print "%s[%02d/%02d/%s %02d:%02d:%02d] {%s}%s : %s"%(color,dd.day,dd.month,dd.year,dd.hour,dd.minute,dd.second,level,WHITE,message)

