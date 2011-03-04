# -*- coding: utf-8 -*-

from project import *

import time
import unittest
import sys
from diyabc import Diyabc
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui

class testDiyabc(unittest.TestCase):
    """
    A test class for DIYABC interface
    """

    def setUp(self):
        """
        set up data used in the tests.
        setUp is called before each test function execution.
        """
        self.app = QApplication(sys.argv)
        self.diyabc = Diyabc(self.app)
        self.diyabc.show()

    def testQuedale(self):
        self.assertEqual(self.diyabc.app, self.app)

    def testQuedale2(self):
        #QTest.mouseClick(self.diyabc.ui.skipButton,Qt.LeftButton)
        self.assertEqual(self.diyabc.ui.skipButton.isVisible(), True)
        self.diyabc.openProject("/home/julien/vcs/git/diyabc/python_interface/datafiles/test/ploop_2011_1_18-3/")
        self.assertEqual(len(self.diyabc.project_list) == 1,True)
        self.diyabc.openProject("/home/julien/vcs/git/diyabc/python_interface/datafiles/test/ploop_2011_1_18-3xx/")
        self.assertEqual(len(self.diyabc.project_list) == 1,True)
        self.diyabc.close()


if __name__ == '__main__':
    unittest.main()

