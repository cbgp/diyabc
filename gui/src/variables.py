# -*- coding: utf-8 -*-

import sys,os.path
from utils.cbgpUtils import getLastRevisionDate,getFsEncoding

# variable qui sera changée par le générateur d'exécutable
VERSION='development version'
VERSION_DATE='01/01/1970'
if ('development vers' in VERSION):
    try:
        VERSION_DATE=getLastRevisionDate("../../")
    except Exception as e:
        VERSION_DATE="unknown"

FSENCODING = getFsEncoding(logLevel=1)
DATAPATH = ""
DOCPATH = ""
ICONPATH = ""
IMAGEPATH = ""
UIPATH = "uis"

# 3 cas : sources, tarball, deb/rpm, .app
if "linux" in sys.platform and not os.path.exists(u"../data".encode(FSENCODING)) and not os.path.exists(u"./data".encode(FSENCODING)) and not getattr(sys, 'frozen', None):
    DATAPATH = "/usr/share/diyabc"
    ICONPATH = "/usr/share/icons/diyabc"
    IMAGEPATH = "/usr/share/images/diyabc"
elif os.path.exists(u"../data".encode(FSENCODING)):
    DATAPATH = "../data"
    ICONPATH = "../data/icons"
    IMAGEPATH = "../data/images/"
elif os.path.exists(u"./data".encode(FSENCODING)):
    DATAPATH = "./data"
    ICONPATH = "./data/icons"
    IMAGEPATH = "./data/images/"
elif getattr(sys, 'frozen', None):
    DATAPATH = sys._MEIPASS+"/data"
    ICONPATH = sys._MEIPASS+"/data/icons"
    IMAGEPATH = sys._MEIPASS+"/data/images"
    UIPATH = sys._MEIPASS+"/uis"

# paquet deb/rpm
if "linux" in sys.platform and not os.path.exists(u"docs/documentation".encode(FSENCODING)) and not getattr(sys, 'frozen', None):
    DOCPATH = "/usr/share/doc/diyabc-doc"
# sources
elif os.path.exists(u"../../doc/".encode(FSENCODING)):
    DOCPATH = "../../doc/"
# DMG
elif os.path.exists(u"../doc/".encode(FSENCODING)):
    DOCPATH = "../doc/"
elif os.path.exists(u"./doc/".encode(FSENCODING)):
    DOCPATH = "./doc/"
# onefile
elif getattr(sys, 'frozen', None):
    DOCPATH = sys._MEIPASS+"/doc/"
