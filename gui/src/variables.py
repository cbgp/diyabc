#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys,os.path

# variable qui sera changée par le générateur d'exécutable
VERSION='development version'
VERSION_DATE='01/01/1970'

DATAPATH = ""
DOCPATH = ""

# 3 cas : sources, tarball, deb/rpm
if "linux" in sys.platform and not os.path.exists("../data") and not os.path.exists("./data"):
    DATAPATH = "/usr/share/diyabc-%s"%VERSION
elif os.path.exists("../data"):
    DATAPATH = "../data"
else:
    DATAPATH = "./data"

if "linux" in sys.platform and not os.path.exists("docs/documentation"):
    DOCPATH = "/usr/share/diyabc-%s/documentation"%VERSION
else:
    DOCPATH = "docs/documentation"
