#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys,os.path
import diyabc
DATAPATH = "plop"
if "linux" in sys.platform and not os.path.exists("docs"):
    DATAPATH = "/usr/share/diyabc-%s"%diyabc.VERSION
else:
    DATAPATH = "docs"
