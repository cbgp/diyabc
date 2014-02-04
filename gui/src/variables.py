# -*- coding: utf-8 -*-

import sys,os.path
from utils.cbgpUtils import getLastRevisionDate


fsCoding = sys.getfilesystemencoding()

# variable qui sera changée par le générateur d'exécutable
VERSION='development version'
VERSION_DATE='01/01/1970'
if ('development vers' in VERSION):
    try:
        VERSION_DATE=getLastRevisionDate("../../")
    except Exception as e:
        VERSION_DATE="unknown"

DATAPATH = ""
DOCPATH = ""
ICONPATH = ""
IMAGEPATH = ""
UIPATH = "uis"

# 3 cas : sources, tarball, deb/rpm, .app
if "linux" in sys.platform and not os.path.exists(u"../data".encode(fsCoding)) and not os.path.exists(u"./data".encode(fsCoding)) and not getattr(sys, 'frozen', None):
    DATAPATH = "/usr/share/diyabc"
    ICONPATH = "/usr/share/icons/diyabc"
    IMAGEPATH = "/usr/share/images/diyabc"
elif os.path.exists(u"../data".encode(fsCoding)):
    DATAPATH = "../data"
    ICONPATH = "../data/icons"
    IMAGEPATH = "../data/images/"
elif os.path.exists(u"./data".encode(fsCoding)):
    DATAPATH = "./data"
    ICONPATH = "./data/icons"
    IMAGEPATH = "./data/images/"
elif getattr(sys, 'frozen', None):
    DATAPATH = sys._MEIPASS+"/data"
    ICONPATH = sys._MEIPASS+"/data/icons"
    IMAGEPATH = sys._MEIPASS+"/data/images"
    UIPATH = sys._MEIPASS+"/uis"

# paquet deb/rpm
if "linux" in sys.platform and not os.path.exists(u"docs/documentation".encode(fsCoding)) and not getattr(sys, 'frozen', None):
    DOCPATH = "/usr/share/doc/diyabc-doc"
# sources
elif os.path.exists(u"../../doc/".encode(fsCoding)):
    DOCPATH = "../../doc/"
# DMG
elif os.path.exists(u"../doc/".encode(fsCoding)):
    DOCPATH = "../doc/"
elif os.path.exists(u"./doc/".encode(fsCoding)):
    DOCPATH = "./doc/"
# onefile
elif getattr(sys, 'frozen', None):
    DOCPATH = sys._MEIPASS+"/doc/"
