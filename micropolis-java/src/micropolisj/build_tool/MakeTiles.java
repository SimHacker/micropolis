package micropolisj.build_tool;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.nio.charset.Charset;
import java.util.*;
import javax.imageio.*;
import javax.swing.ImageIcon;

public class MakeTiles
{
	static HashMap<String,String> tileData = new HashMap<String,String>();
	static HashMap<String,Image> loadedImages = new HashMap<String,Image>();

	static final Charset UTF8 = Charset.forName("UTF-8");
	static final int NTILES = 960;
	static final int TILE_SIZE = 16;

	public static void main(String [] args)
		throws Exception
	{
		File recipeFile = new File(args[0]);
		File outputFile = new File(args[1]);

		Properties recipe = new Properties();
		recipe.load(
			new InputStreamReader(
				new FileInputStream(recipeFile),
				UTF8
			));

		// actually assemble the image
		BufferedImage buf = new BufferedImage(TILE_SIZE,TILE_SIZE*NTILES,BufferedImage.TYPE_INT_RGB);
		Graphics2D gr = buf.createGraphics();

		for (int i = 0; i < NTILES; i++) {
			String rawSpec = recipe.getProperty(Integer.toString(i));
			assert rawSpec != null;

			TileSpec tileSpec = TileSpec.parse(rawSpec);
			ImageSpec ref = parseImageSpec(tileSpec);
			drawTo(ref, gr, 0, TILE_SIZE*i);
		}

		ImageIO.write(buf, "png", outputFile);
	}

	static void drawTo(ImageSpec ref, Graphics2D gr, int destX, int destY)
		throws IOException
	{
		if (ref.background != null) {
			drawTo(ref.background, gr, destX, destY);
		}

		if (!loadedImages.containsKey(ref.fileName)) {
			loadedImages.put(ref.fileName,
				loadImage(ref.fileName));
		}

		Image sourceImg = loadedImages.get(ref.fileName);
		int srcWidth = sourceImg.getWidth(null);
		int srcHeight = sourceImg.getHeight(null);

		gr.drawImage(
			sourceImg,
			destX, destY,
			destX+TILE_SIZE, destY+TILE_SIZE,
			ref.offsetX, ref.offsetY,
			ref.offsetX + (ref.width != 0 ? ref.width : srcWidth),
			ref.offsetY + (ref.height != 0 ? ref.height : srcHeight),
			null);
	}

	static class ImageSpec
	{
		ImageSpec background;
		String fileName;
		int offsetX;
		int offsetY;
		int width;
		int height;
	}

	static ImageSpec parseImageSpec(TileSpec spec)
	{
		ImageSpec result = null;

		for (String layerStr : spec.getValues("image")) {

System.err.println("parsing "+layerStr);
		ImageSpec rv = new ImageSpec();
		rv.background = result;
		result = rv;

		String [] parts = layerStr.split("@", 2);
		rv.fileName = parts[0];

		if (parts.length >= 2) {
			String offsetInfo = parts[1];
			parts = offsetInfo.split(",");
			if (parts.length >= 1) {
				rv.offsetX = Integer.parseInt(parts[0]);
			}
			if (parts.length >= 2) {
				rv.offsetY = Integer.parseInt(parts[1]);
			}
			if (parts.length >= 3) {
				rv.width = Integer.parseInt(parts[2]);
			}
			if (parts.length >= 4) {
				rv.height = Integer.parseInt(parts[3]);
			}
		}//endif something given after '@' in image specifier

		}//end foreach layer in image specification

		return result;
	}

	static Image loadImage(String fileName)
		throws IOException
	{
		ImageIcon ii = new ImageIcon(fileName+".png");
		return ii.getImage();
	}
}
