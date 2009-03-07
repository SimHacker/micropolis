"""
@file main.py Main file for running Micropolis with the GTK frontend

@todo Implement run()
"""


import pyMicropolis.simEngine
from pyMicropolis.simEngine import micropolisengine, micropolisgtkengine, micropoliswindow, micropolisrobot
import gtk
import random
import math


def run():

    engine = micropolisgtkengine.CreateGTKEngine()

    engine.cityTax = 10
    engine.setPasses(200)
    setTile = engine.setTile

    if False:
        for i in range(0, 4):
            engine.addRobot(
                micropolisrobot.MicropolisRobot_PacMan(
                    x=(8 * 16) + 3 + (16 * 2 * i),
                    y=(7 * 16) + 3,
                    direction=0))

    if False:
        for i in range(0, 20):
            engine.addRobot(
                micropolisrobot.MicropolisRobot_PacMan(
                    x=random.randint(0, (micropolisengine.WORLD_W * 16) - 1),
                    y=random.randint(0, (micropolisengine.WORLD_H * 16) - 1),
                    direction = random.randint(0, 3) * math.pi / 2))

    if False:
        for y in range(0, micropolisengine.WORLD_H):
            for x in range(0, micropolisengine.WORLD_W):
                setTile(x, y, micropolisengine.RUBBLE | micropolisengine.BLBNBIT)

        for y in range(10, 15):
            for x in range(10, 15):
                setTile(x, y, micropolisengine.FIRE | micropolisengine.ANIMBIT)

    x = 0
    y = 0

    w = 800
    h = 600

    if True:
        win1 = micropoliswindow.MicropolisPanedWindow(engine=engine)
        #win1.set_default_size(w, h)
        win1.set_size_request(w, h)
        win1.move(x, y)
        win1.show_all()

    gtk.main()

