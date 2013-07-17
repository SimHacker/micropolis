package micropolisj.engine;

import java.util.*;

public class TileSpec
{
	Map<String,String> attributes;
	List<String> images;

	protected TileSpec()
	{
		this.attributes = new HashMap<String,String>();
		this.images = new ArrayList<String>();
	}

	public static TileSpec parse(String inStr)
	{
		TileSpec ts = new TileSpec();
		ts.load(inStr);
		return ts;
	}

	public String getAttribute(String key)
	{
		return attributes.get(key);
	}

	public String [] getImages()
	{
		return images.toArray(new String[0]);
	}

	protected void load(String inStr)
	{
		Scanner in = new Scanner(inStr);

		while (in.hasMore()) {

			if (in.peekChar() == '(') {
				in.eatChar('(');
				String k = in.readAttributeKey();
				String v = "1";
				if (in.peekChar() == '=') {
					in.eatChar('=');
					v = in.readAttributeValue();
				}
				in.eatChar(')');
				attributes.put(k, v);
			}

			else if (in.peekChar() == '|' || in.peekChar() == ',') {
				in.eatChar(in.peekChar());
			}

			else {
				String v = in.readImageSpec();
				images.add(v);
			}
		}
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
