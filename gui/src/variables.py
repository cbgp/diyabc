#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys,os.path

# variable qui sera changée par le générateur d'exécutable
VERSION='development version'
VERSION_DATE='01/01/1970'

DATAPATH = ""
DOCPATH = ""
ICONPATH = ""
IMAGEPATH = ""
UIPATH = "uis"

# 3 cas : sources, tarball, deb/rpm, .app
if "linux" in sys.platform and not os.path.exists("../data") and not os.path.exists("./data") and not getattr(sys, 'frozen', None):
    DATAPATH = "/usr/share/diyabc-%s"%VERSION
    ICONPATH = "/usr/share/icons/diyabc"
    IMAGEPATH = "/usr/share/images/diyabc"
elif os.path.exists("../data"):
    DATAPATH = "../data"
    ICONPATH = "../data/icons"
    IMAGEPATH = "../data/images/"
elif os.path.exists("./data"):
    DATAPATH = "./data"
    ICONPATH = "./data/icons"
    IMAGEPATH = "./data/images/"
elif getattr(sys, 'frozen', None):
    DATAPATH = sys._MEIPASS+"/data"
    ICONPATH = sys._MEIPASS+"/data/icons"
    IMAGEPATH = sys._MEIPASS+"/data/images"
    UIPATH = sys._MEIPASS+"/uis"


if "linux" in sys.platform and not os.path.exists("docs/documentation"):
    DOCPATH = "/usr/share/doc/diyabc"
else:
    DOCPATH = "doc/documentation"
