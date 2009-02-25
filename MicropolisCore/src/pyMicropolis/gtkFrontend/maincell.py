"""
@file maincell.py Main file for running the CellEngine with the GTK frontend

@todo Implement run()
"""


import pyMicropolis.cellEngine
from pyMicropolis.cellEngine import cellengine
from pyMicropolis.cellEngine import cellmodel
from pyMicropolis.cellEngine import celltool
from pyMicropolis.cellEngine import celldrawingarea
from pyMicropolis.cellEngine import cellpiemenus
from pyMicropolis.cellEngine import cellwindow
from pyMicropolis.cellEngine import cellconfigurations
from pyMicropolis.cellEngine import cellrulecompiler
import gtk
import random
import math


def run():

    engine = cellmodel.CreateTestEngine()

    x = 0
    y = 0
    w = 512
    h = 512

    if True:
        win1 = cellwindow.CellWindow(engine=engine)
        win1.set_size_request(w, h)
        win1.move(x, y)
        win1.show_all()

    gtk.main()

