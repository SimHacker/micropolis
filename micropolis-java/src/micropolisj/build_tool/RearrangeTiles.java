package micropolisj.build_tool;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.util.*;
import javax.imageio.*;
import javax.swing.ImageIcon;

/**
 * Just a little utility I wrote to rearrange 16x16 tiles into a grid
 * of a different width. Not included in the Micropolis build.
 */
public class RearrangeTiles
{
	static final int DEST_COLUMNS = 16;
	static final int TILE_SIZE = 16;

	public static void main(String [] args)
		throws Exception
	{
		File inputFile = new File(args[0]);
		File outputFile = new File(args[1]);

		// read in the image
		Image srcImage = new ImageIcon(inputFile.toString()).getImage();
		int srcCols = srcImage.getWidth(null) / TILE_SIZE;
		int srcRows = srcImage.getHeight(null) / TILE_SIZE;
		int ntiles = srcRows * srcCols;

		// actually assemble the image
		int destCols = DEST_COLUMNS;
		int destRows = (ntiles + destCols-1) / destCols;
		BufferedImage buf = new BufferedImage(TILE_SIZE*DEST_COLUMNS,TILE_SIZE*destRows,BufferedImage.TYPE_INT_RGB);
		Graphics2D gr = buf.createGraphics();

		for (int i = 0; i < ntiles; i++) {

			int srcRow = i / srcCols;
			int srcCol = i % srcCols;

			int destRow = i / destCols;
			int destCol = i % destCols;

			gr.drawImage(
				srcImage,
				destCol*TILE_SIZE, destRow*TILE_SIZE,
				(destCol+1)*TILE_SIZE,(destRow+1)*TILE_SIZE,
				srcCol*TILE_SIZE, srcRow*TILE_SIZE,
				(srcCol+1)*TILE_SIZE, (srcRow+1)*TILE_SIZE,
				null);
		}

		ImageIO.write(buf, "png", outputFile);
	}
}
