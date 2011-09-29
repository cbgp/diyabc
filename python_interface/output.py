# -*- coding: utf-8 -*-

#from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
import os,sys
from datetime import datetime 
import inspect
import re
import time
#import codecs

# mettre à vrai pour ne pas afficher les popups
debug = False
LOG_LEVEL = 4

RED='\033[31m'
WHITE='\033[00m'
BLUE='\033[35m'
GREEN='\033[32m'
YELLOW='\033[33m'
CYAN='\033[36m'
CYAN_BLINK='\033[5;36m'
tabcolors=[WHITE,RED,BLUE,GREEN,YELLOW,CYAN,CYAN_BLINK]

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

def sizedirectory(path):
    size = 0
    for root, dirs, files in os.walk(path):
        for fic in files:
            size += os.path.getsize(os.path.join(root, fic))
    return size


def logRotate(logFolder,nbDaysOld,sizeThreshold):
    """ fonction qui nettoie le dossier logFolder de 
    tous les fichiers plus vieux que nbDaysOld
    SSI le dossier de logs est plus lourd que sizeThreshold Mo
    """
    logSize = sizedirectory(logFolder)
    if (logSize / (1024*1024)) > sizeThreshold:
        log(2,"Logrotate process launched")
        ddNow = datetime.now()
        for root,dirs,files in os.walk(os.path.expanduser("~/.diyabc/logs/")):
            for name in files:
                dateLastModif = time.gmtime(os.stat(os.path.join(root, name)).st_mtime)
                ddMod = datetime(dateLastModif.tm_year,dateLastModif.tm_mon,dateLastModif.tm_mday)
                delta = ddNow - ddMod
                if delta.days > nbDaysOld :
                    log(4,"Deleting %s because it is %s days old"%(os.path.join(root, name),delta.days))
                    os.remove(os.path.join(root, name))

class TeeLogger(object):
    """ Classe qui remplace stdout et stderr pour logger sur 
    la sortie standard (ancienne stdout), dans un fichier et dans
    la fenetre de log simultanément
    Effectue aussi une sorte de logrotate
    ATTENTION effet de bord : stderr n'est plus différentié
    il est écrit dans les trois sorties
    """
    def __init__(self, name, mode, app, out_or_err):
        """ raccourci le fichier de log s'il est trop grand 
        et remplace stdout
        """
        self.app=app
        #self.logRotate(name)

        #self.file = open(name, mode)
        self.filename = name 
        self.out_or_err = out_or_err
        # on sauvegarde le out que l'on remplace
        if out_or_err:
            self.out = sys.stdout
        else:
            self.out = sys.stderr
    def __del__(self):
        pass
        ## genere une exception que je ne comprends pas
        # Exception AttributeError: "'NoneType' object has no attribute 'stdout'" in <bound method TeeLogger.__del__ of <output.TeeLogger object at 0x92ce5ec>> ignored
        #if self.out_or_err:
        #    sys.stdout = self.out
        #else:
        #    sys.stderr = self.out

        #self.file.close()
    def write(self, data):
        #data = data.replace(u'\xb5','u')
        data_without_color = data.replace(RED,'').replace(WHITE,'').replace(GREEN,'').replace(BLUE,'').replace(YELLOW,'').replace(CYAN,'').replace(CYAN_BLINK,'')
        pattern = re.compile(r' \[\[.*\]\]')
        data_short = pattern.sub('',data)
        # on bouge le curseur au début de la dernière ligne
        self.app.showLogFile_win.logText.moveCursor(QTextCursor.End)
        self.app.showLogFile_win.logText.moveCursor(QTextCursor.StartOfLine)
        self.app.showLogFile_win.logText.appendPlainText("%s"%pattern.sub('',data_without_color.strip()))
        ftmp = open(self.filename,'a')
        ftmpdata = data_without_color
        ftmp.write(ftmpdata)
        #ftmp.file.flush()
        ftmp.close()
        self.out.write(data_short)
    def logRotate(self,name):
        if os.path.exists(name):
            f = open(name,'r')
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

        lastFrame = inspect.stack()[1][0]
        params = lastFrame.f_locals

        params = inspect.currentframe().f_back.f_locals

        func = ""
        if len(inspect.stack()) > 3:
            func = "%s >> %s(%s)"%(inspect.stack()[3][3],inspect.stack()[2][3],params)
        elif len(inspect.stack()) > 2 and len(inspect.stack()[2]) > 3:
            func = "%s(%s)"%(inspect.stack()[2][3],params)

        print "%s[%02d/%02d/%s %02d:%02d:%02d] {%s} [[%s]] %s : %s"%(color,dd.day,dd.month,dd.year,dd.hour,dd.minute,dd.second,level,func,WHITE,message)

