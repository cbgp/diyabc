# -*- coding: utf-8 -*-

#from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from utils.cbgpUtils import log,getFsEncoding
#import codecs

# mettre à vrai pour ne pas afficher les popups
debug = False

whatsthis_header = '\
<head>\
<style type="text/css">\
.different-text-color { color: black; }\
</style>\
</head>\
<body>\
<p>\
<span class="different-text-color">'

whatsthis_footer = '</span></p>'

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


