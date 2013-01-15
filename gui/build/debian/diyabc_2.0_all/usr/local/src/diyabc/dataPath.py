#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys,os.path
import diyabc
DATAPATH = ""
DOCPATH = ""
if "linux" in sys.platform and not os.path.exists("docs"):
    DATAPATH = "/usr/share/diyabc-%s"%diyabc.VERSION
else:
    DATAPATH = "docs"

if "linux" in sys.platform and not os.path.exists("docs/documentation"):
    DOCPATH = "/usr/share/diyabc-%s/documentation"%diyabc.VERSION
else:
    DOCPATH = "docs/documentation"
