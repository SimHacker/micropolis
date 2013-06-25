import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.util.*;
import javax.imageio.*;
import javax.swing.ImageIcon;

public class MakeTiles
{
	static HashMap<String,String> tileData = new HashMap<String,String>();
	static HashMap<String,Image> loadedImages = new HashMap<String,Image>();

	static final int NTILES = 960;
	static final int TILE_SIZE = 16;

	public static void main(String [] args)
		throws Exception
	{
		File recipeFile = new File(args[0]);
		File outputFile = new File(args[1]);

		BufferedReader in = new BufferedReader(
			new FileReader(recipeFile));
		String l;
		while ( (l=in.readLine()) != null )
		{
			if (l.startsWith("#")) {
				continue;
			}
			String[] parts = l.split("\\s+");
			String tileName = parts[0];
			String tileImage = parts[1];
			tileData.put(tileName, tileImage);
		}
		in.close();

		// actually assemble the image
		BufferedImage buf = new BufferedImage(TILE_SIZE,TILE_SIZE*NTILES,BufferedImage.TYPE_INT_RGB);
		Graphics2D gr = buf.createGraphics();

		for (int i = 0; i < NTILES; i++) {
			String tileImage = tileData.get(Integer.toString(i));
			assert tileImage != null;

			ImageRef ref = parseImageRef(tileImage);
			if (!loadedImages.containsKey(ref.fileName)) {
				loadedImages.put(ref.fileName,
					loadImage(ref.fileName));
			}

			Image sourceImg = loadedImages.get(ref.fileName);
			int srcWidth = sourceImg.getWidth(null);
			int srcHeight = sourceImg.getHeight(null);

			gr.drawImage(
				sourceImg,
				0, TILE_SIZE*i,
				TILE_SIZE, TILE_SIZE*(i+1),
				ref.offsetX, ref.offsetY,
				ref.offsetX + (ref.width != 0 ? ref.width : srcWidth),
				ref.offsetY + (ref.height != 0 ? ref.height : srcHeight),
				null);
		}

		ImageIO.write(buf, "png", outputFile);
	}

	static class ImageRef
	{
		String fileName;
		int offsetX;
		int offsetY;
		int width;
		int height;
	}

	static ImageRef parseImageRef(String s)
	{
		ImageRef rv = new ImageRef();

		String [] parts = s.split("@", 2);
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

		return rv;
	}

	static Image loadImage(String fileName)
		throws IOException
	{
		ImageIcon ii = new ImageIcon(fileName+".png");
		return ii.getImage();
	}
}
