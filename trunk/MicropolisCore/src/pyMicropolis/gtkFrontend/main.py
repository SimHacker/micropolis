"""
@file main.py Main file for running Micropolis with the GTK frontend

@todo Implement run()
"""


import pyMicropolis.simEngine
from pyMicropolis.simEngine import micropolisengine, micropolismodel, micropoliswindow, micropolisrobot
import gtk


def run():

    engine = micropolismodel.CreateTestEngine()

    engine.cityTax = 10
    engine.setPasses(200)
    setTile = engine.setTile

    if True:
        pacman = micropolisrobot.MicropolisRobot_PacMan(
            x=(16 * 8) + 8,
            y=(16 * 7) + 8,
            speed=1)
        print "PACMAN", pacman
        engine.addRobot(pacman)

    if False:
        for y in range(0, micropolisengine.WORLD_H):
            for x in range(0, micropolisengine.WORLD_W):
                setTile(x, y, micropolisengine.RUBBLE | micropolisengine.BLBNBIT)

        for y in range(10, 15):
            for x in range(10, 15):
                setTile(x, y, micropolisengine.FIRE | micropolisengine.ANIMBIT)

    fudge = 0
    width = int((120 * 4) + fudge)
    height = int((100 * 4) + fudge)

    w = width
    h = height

    x1 = 0
    y1 = 0
    x2 = w + 20
    y2 = h + 40

    if True:
        win2 = micropoliswindow.MicropolisPanedWindow(engine=engine)
        win2.set_default_size(800, 800)
        win2.move(x1, y1)
        win2.show_all()

    gtk.main()

