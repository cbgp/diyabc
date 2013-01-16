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

# 3 cas : sources, tarball, deb/rpm
if "linux" in sys.platform and not os.path.exists("../data") and not os.path.exists("./data"):
    DATAPATH = "/usr/share/diyabc-%s"%VERSION
    ICONPATH = "/usr/share/icons/diyabc"
    IMAGEPATH = "/usr/share/images/diyabc"
elif os.path.exists("../data"):
    DATAPATH = "../data"
    ICONPATH = "../data/icons"
    IMAGEPATH = "../data/images/"
else:
    DATAPATH = "./data"
    ICONPATH = "./data/icons"
    IMAGEPATH = "./data/images/"

if "linux" in sys.platform and not os.path.exists("docs/documentation"):
    DOCPATH = "/usr/share/doc/diyabc"
else:
    DOCPATH = "doc/documentation"
