package micropolisj.build_tool;

import micropolisj.engine.TileSpec;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.nio.charset.Charset;
import java.util.*;
import javax.imageio.*;
import javax.swing.ImageIcon;

import static micropolisj.engine.TileSpec.generateTileNames;

public class MakeTiles
{
	static HashMap<String,String> tileData = new HashMap<String,String>();
	static HashMap<String,SourceImage> loadedImages = new HashMap<String,SourceImage>();

	static final Charset UTF8 = Charset.forName("UTF-8");
	static int SKIP_TILES = 0;
	static int COUNT_TILES = -1;
	static int TILE_SIZE = 16;

	public static void main(String [] args)
		throws Exception
	{
		if (args.length != 2) {
			throw new Exception("Wrong number of arguments");
		}

		if (System.getProperty("tile_size") != null) {
			TILE_SIZE = Integer.parseInt(System.getProperty("tile_size"));
		}
		if (System.getProperty("skip_tiles") != null) {
			SKIP_TILES = Integer.parseInt(System.getProperty("skip_tiles"));
		}
		if (System.getProperty("tile_count") != null) {
			COUNT_TILES = Integer.parseInt(System.getProperty("tile_count"));
		}

		File recipeFile = new File(args[0]);
		File outputFile = new File(args[1]);

		generateFromRecipe(recipeFile, outputFile);
	}

	static void generateFromRecipe(File recipeFile, File outputFile)
		throws IOException
	{
		Properties recipe = new Properties();
		recipe.load(
			new InputStreamReader(
				new FileInputStream(recipeFile),
				UTF8
			));

		// count number of images
		String [] tileNames = generateTileNames(recipe);
		int ntiles = COUNT_TILES == -1 ? tileNames.length : COUNT_TILES;

		// actually assemble the image
		BufferedImage buf = new BufferedImage(TILE_SIZE,TILE_SIZE*ntiles,BufferedImage.TYPE_INT_RGB);
		Graphics2D gr = buf.createGraphics();

		for (int i = 0; i < ntiles; i++) {
			int tileNumber = SKIP_TILES + i;
			if (!(tileNumber >= 0 && tileNumber < tileNames.length)) {
				continue;
			}

			String tileName = tileNames[tileNumber];
			String rawSpec = recipe.getProperty(tileName);
			assert rawSpec != null;

			TileSpec tileSpec = TileSpec.parse(tileNumber, tileName, rawSpec, recipe);
			FrameSpec ref = parseFrameSpec(tileSpec);
			if (ref == null) {
				// tile is defined, but it has no images
				continue;
			}

			drawTo(ref, gr, 0, TILE_SIZE*i);
		}

		System.out.println("Generating tiles array: "+outputFile);
		ImageIO.write(buf, "png", outputFile);

		File indexFile = new File(
				outputFile.getParentFile(),
				outputFile.getName()
					.replaceFirst("\\.png$","")
					+ ".idx"
				);
		System.out.println("Generating tiles index: "+indexFile);
		writeIndexFile(tileNames, indexFile);
	}

	static void writeIndexFile(String [] tileNames, File indexFile)
		throws IOException
	{
		PrintWriter out = new PrintWriter(
			new FileWriter(indexFile)
			);
		for (int i = 0; i < tileNames.length; i++) {
			out.printf("%s %d\n", tileNames[i], i);
		}
		out.close();
	}

	static void drawTo(FrameSpec ref, Graphics2D gr, int destX, int destY)
		throws IOException
	{
		if (ref.background != null) {
			drawTo(ref.background, gr, destX, destY);
		}

		if (!loadedImages.containsKey(ref.fileName)) {
			loadedImages.put(ref.fileName,
				loadImage(ref.fileName));
		}

		SourceImage sourceImg = loadedImages.get(ref.fileName);

		gr.drawImage(
			sourceImg.image,
			destX, destY,
			destX+TILE_SIZE, destY+TILE_SIZE,
			ref.offsetX * sourceImg.basisSize / 16,
			ref.offsetY * sourceImg.basisSize / 16,
			(ref.offsetX + (ref.width != 0 ? ref.width : 16)) * sourceImg.basisSize / 16,
			(ref.offsetY + (ref.height != 0 ? ref.height : 16)) * sourceImg.basisSize / 16,
			null);
	}

	static class SourceImage
	{
		Image image;
		int basisSize;

		SourceImage(Image image, int basisSize) {
			this.image = image;
			this.basisSize = basisSize;
		}
	}

	static class FrameSpec
	{
		FrameSpec background;
		String fileName;
		int offsetX;
		int offsetY;
		int width;
		int height;
	}

	static FrameSpec parseFrameSpec(TileSpec spec)
	{
		FrameSpec result = null;

		for (String layerStr : spec.getImages()) {

		FrameSpec rv = new FrameSpec();
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

	static File findInkscape()
	{
		String exeName = "inkscape";
		if (System.getProperty("os.name").startsWith("Windows")) {
			exeName += ".exe";
		}

		File [] pathsToTry = {
			new File("/usr/bin"),
			new File("c:\\Program Files\\Inkscape"),
			new File("c:\\Program Files (x86)\\Inkscape")
			};
		for (File p : pathsToTry) {
			File f = new File(p, exeName);
			if (f.exists()) {
				return f;
			}
		}
		throw new Error("INKSCAPE not installed (or not found)");
	}

	static File stagingDir = new File("generated");
	static File renderSvg(String fileName, File svgFile)
		throws IOException
	{
		File pngFile = new File(stagingDir, fileName+"_"+TILE_SIZE+"x"+TILE_SIZE+".png");
		if (pngFile.exists() &&
			pngFile.lastModified() > svgFile.lastModified())
		{
			// looks like the PNG file is already up-to-date
			return pngFile;
		}

		File inkscapeBin = findInkscape();

		System.out.println("Generating raster image: "+pngFile);
		if (pngFile.exists()) {
			pngFile.delete();
		}
		else {
			pngFile.getParentFile().mkdirs();
		}

		String [] cmdline = {
			inkscapeBin.toString(),
			"--export-dpi="+(TILE_SIZE*90.0/16.0),
			"--export-png="+pngFile.toString(),
			svgFile.toString()
			};
		Process p = Runtime.getRuntime().exec(cmdline);
		int exit_value;
		try {
			exit_value = p.waitFor();
		}
		catch (InterruptedException e) {
			throw new RuntimeException(e);
		}

		if (exit_value != 0) {
			throw new RuntimeException("Helper exit status: "+exit_value);
		}

		if (!pngFile.exists()) {
			throw new RuntimeException("File not found: "+pngFile);
		}

		return pngFile;
	}

	static SourceImage loadImage(String fileName)
		throws IOException
	{
		File svgFile, pngFile = null;

		svgFile = new File(fileName+"_"+TILE_SIZE+"x"+TILE_SIZE+".svg");

		if (svgFile.exists()) {
			pngFile = renderSvg(fileName, svgFile);
		}
		else {
			svgFile = new File(fileName+".svg");
			if (svgFile.exists()) {
				pngFile = renderSvg(fileName, svgFile);
			}
		}

		if (pngFile != null && pngFile.exists()) {
			ImageIcon ii = new ImageIcon(pngFile.toString());
			return new SourceImage(
				ii.getImage(),
				TILE_SIZE);
		}

		pngFile = new File(fileName+"_"+TILE_SIZE+"x"+TILE_SIZE+".png");
		if (pngFile.exists()) {
			ImageIcon ii = new ImageIcon(pngFile.toString());
			return new SourceImage(
				ii.getImage(),
				TILE_SIZE);
		}

		pngFile = new File(fileName+".png");
		if (pngFile.exists()) {
			ImageIcon ii = new ImageIcon(pngFile.toString());
			return new SourceImage(
				ii.getImage(),
				16);
		}

		throw new IOException("File not found: "+fileName+".{svg,png}");
	}
}
