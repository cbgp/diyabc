#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys,os.path
#from diyabc import VERSION
VERSION=2
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
