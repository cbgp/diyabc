#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys,os.path
from diyabc import VERSION
DATAPATH = ""
DOCPATH = ""
if "linux" in sys.platform and not os.path.exists("../data"):
    DATAPATH = "/usr/share/diyabc-%s"%VERSION
else:
    DATAPATH = "../data"

if "linux" in sys.platform and not os.path.exists("docs/documentation"):
    DOCPATH = "/usr/share/diyabc-%s/documentation"%VERSION
else:
    DOCPATH = "docs/documentation"
