########################################################################
# testming.py
# Convert tiles to swf files with Ming.
# Written for Micropolis by Don Hopkins.
# Licensed under GPLv3.


########################################################################
# Import modules.


import ming
import math
import cairo
import tempfile
import os



########################################################################


def makeTiles(
    tilesFileName='htdocs/static/images/micropolis_tiles.png',
    tileFileFormat='htdocs/static/images/micropolis_tile_%04d.png',
    swfFileName='laszlo/micropolis/resources/micropolis_tiles.swf',
    tileSize=16,
    tileCount=960):

    tilesSurface = cairo.ImageSurface.create_from_png(tilesFileName)
    tilesWidth = tilesSurface.get_width()
    tilesHeight = tilesSurface.get_height()

    tileSurface = tilesSurface.create_similar(cairo.CONTENT_COLOR, tileSize, tileSize)
    tileCtx = cairo.Context(tileSurface)

    movie = ming.SWFMovie()
    movie.setFrames(tileCount)
    movie.setDimension(tileSize, tileSize)
    movie.setRate(10.0)
    
    cols = int(math.floor(tilesWidth / tileSize))
    rows = int(math.ceil(float(tileCount) / float(cols)))
    print "cols", cols, "rows", rows

    bitmaps = {}

    for tile in range(0, tileCount):

        col = int(tile % cols)
        row = int(tile / cols)
        x = col * tileSize
        y = row * tileSize

        tileCtx.save()
        tileCtx.set_source_surface(tilesSurface, -x, -y)
        tileCtx.paint()
        tileCtx.restore()

        tileFileName = tileFileFormat % (tile,)

        tileSurface.write_to_png(tileFileName)

        shape = ming.SWFShape()
        
        tileBitmap = ming.SWFBitmap(tileFileName)
        bitmaps[tileFileName] = tileBitmap
        fill = shape.addBitmapFill(tileBitmap, ming.SWFFILL_CLIPPED_BITMAP)
        shape.setRightFill(fill)
        shape.movePenTo(0, 0)
        shape.drawLine(tileSize, 0)
        shape.drawLine(0, tileSize)
        shape.drawLine(-tileSize, 0)
        shape.drawLine(0, -tileSize)
        di = movie.add(shape)
        movie.nextFrame()

    print "saving " + swfFileName
    movie.save(swfFileName)


########################################################################


if __name__ == '__main__':
    makeTiles()


########################################################################
