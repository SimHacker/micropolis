package micropolisj.engine;

import java.util.*;

public class TileSpec
{
	int tileNumber;
	TileSpec animNext;
	TileSpec onPower;
	boolean canBurn;
	boolean canConduct;
	boolean zone;

	Map<String,String> attributes;
	List<String> images;

	protected TileSpec(int tileNumber)
	{
		this.tileNumber = tileNumber;
		this.attributes = new HashMap<String,String>();
		this.images = new ArrayList<String>();
	}

	public static TileSpec parse(int tileNumber, String inStr, Properties tilesRc)
	{
		TileSpec ts = new TileSpec(tileNumber);
		ts.load(inStr, tilesRc);
		return ts;
	}

	public String getAttribute(String key)
	{
		return attributes.get(key);
	}

	public boolean getBooleanAttribute(String key)
	{
		String v = getAttribute(key);
		return (v != null && v.equals("true"));
	}

	public String [] getImages()
	{
		return images.toArray(new String[0]);
	}

	protected void load(String inStr, Properties tilesRc)
	{
		Scanner in = new Scanner(inStr);

		while (in.hasMore()) {

			if (in.peekChar() == '(') {
				in.eatChar('(');
				String k = in.readAttributeKey();
				String v = "true";
				if (in.peekChar() == '=') {
					in.eatChar('=');
					v = in.readAttributeValue();
				}
				in.eatChar(')');

				if (!attributes.containsKey(k)) {
					attributes.put(k, v);
					String sup = tilesRc.getProperty(k);
					if (sup != null) {
						load(sup, tilesRc);
					}
				}
				else {
					attributes.put(k, v);
				}
			}

			else if (in.peekChar() == '|' || in.peekChar() == ',') {
				in.eatChar(in.peekChar());
			}

			else {
				String v = in.readImageSpec();
				images.add(v);
			}
		}

		this.canBurn = !getBooleanAttribute("noburn");
		this.canConduct = getBooleanAttribute("conducts");
		this.zone = getBooleanAttribute("zone");
	}

	static class Scanner
	{
		String str;
		int off = 0;

		Scanner(String str)
		{
			this.str = str;
		}

		private void skipWhitespace()
		{
			while (off < str.length() && Character.isWhitespace(str.charAt(off))) {
				off++;
			}
		}

		public int peekChar()
		{
			skipWhitespace();
			if (off < str.length()) {
				return str.charAt(off);
			}
			else {
				return -1;
			}
		}

		public void eatChar(int ch)
		{
			skipWhitespace();
			assert str.charAt(off) == ch;
			off++;
		}

		public String readAttributeKey()
		{
			skipWhitespace();

			int start = off;
			while (off < str.length() && (str.charAt(off) == '-' || Character.isLetterOrDigit(str.charAt(off)))) {
				off++;
			}

			if (off != start) {
				return str.substring(start, off);
			}
			else {
				return null;
			}
		}

		public String readAttributeValue()
		{
			return readString();
		}

		public String readImageSpec()
		{
			return readString();
		}

		protected String readString()
		{
			skipWhitespace();

			int endQuote = 0; //any whitespace or certain punctuation
			if (peekChar() == '"') {
				off++;
				endQuote = '"';
			}

			int start = off;
			while (off < str.length()) {
				int c = str.charAt(off);
				if (c == endQuote) {
					int end = off;
					off++;
					return str.substring(start,end);
				}
				else if (endQuote == 0 && (Character.isWhitespace(c) || c == ')' || c == '|')) {
					int end = off;
					return str.substring(start, end);
				}
				off++;
			}
			return str.substring(start);
		}

		public boolean hasMore()
		{
			return peekChar() != -1;
		}
	}
}
