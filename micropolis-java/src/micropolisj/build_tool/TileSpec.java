package micropolisj.build_tool;

import java.util.*;

public class TileSpec
{
	Map<String, List<String> > multiValues;

	protected TileSpec()
	{
		this.multiValues = new HashMap<String, List<String> >();
	}

	public static TileSpec parse(String inStr)
	{
		TileSpec ts = new TileSpec();
		ts.load(inStr);
		return ts;
	}

	public String getValue(String key)
	{
		List<String> v = multiValues.get(key);
		if (v != null && v.size() >= 1) {
			return v.get(0);
		}
		else {
			return null;
		}
	}

	public String [] getValues(String key)
	{
		List<String> v = multiValues.get(key);
		if (v != null) {
			return v.toArray(new String[0]);
		}
		else {
			return new String[0];
		}
	}

	protected void load(String inStr)
	{
		Scanner in = new Scanner(inStr);
		String k;
		while ( (k=in.nextKey()) != null ) {
			String v = in.nextValue();
			if (v == null) {
				if (k.startsWith("no")) {
					k = k.substring(2);
					v = "0";
				}
				else {
					v = "1";
				}
			}

			if (multiValues.containsKey(k)) {
				multiValues.get(k).add(v);
			}
			else {
				ArrayList<String> a = new ArrayList<String>();
				a.add(v);
				multiValues.put(k, a);
			}
		}
	}

	static class Scanner
	{
		String str;
		int off = 0;
		int st = ST_VALUE;

		static int ST_KEY = 0;
		static int ST_VALUE = 1;
		static int ST_COMPAT = 2;
		static int ST_EOF = 3;

		Scanner(String str)
		{
			this.str = str;
			if (str.indexOf('(') == -1) {
				st = ST_COMPAT;
			}
		}

		private void skipWhitespace()
		{
			while (off < str.length() && (Character.isWhitespace(str.charAt(off)) || str.charAt(off) == ',')) {
				off++;
			}
		}

		public String nextKey()
		{
			if (st == ST_EOF || off >= str.length()) {
				return null;
			}

			if (st == ST_COMPAT) {
				st = ST_VALUE;
				return "image";
			}

			assert st == ST_KEY;

			if (str.charAt(off) == '|') {
				st = ST_VALUE;
				off++;
				skipWhitespace();
				return "image";
			}
			else {
				int start = off;
				while (off < str.length() && (str.charAt(off) == '-' || Character.isLetterOrDigit(str.charAt(off)))) {
					off++;
				}
				st = ST_VALUE;
				return str.substring(start, off);
			}
		}

		public String nextValue()
		{
			assert st == ST_VALUE;

			if (off == str.length()) {
				st = ST_EOF;
				return null;
			}

			int c = str.charAt(off);
			if (Character.isWhitespace(c) || c == ',') {
				skipWhitespace();
				st = ST_KEY;
				return null;
			}

			int endQuote = 0;
			if (c == '(') {
				off++;
				endQuote = ')';
			}

			int start = off;
			while (off < str.length()) {
				c = str.charAt(off);
				if (c == endQuote) {
					int end = off;
					off++;
					skipWhitespace();
					st = ST_KEY;
					return str.substring(start,end);
				}
				else if (endQuote == 0 && (Character.isWhitespace(c) || c == '|')) {
					int end = off;
					skipWhitespace();
					st = ST_KEY;
					return str.substring(start, end);
				}
				off++;
			}
			st = ST_EOF;
			return str.substring(start);
		}
	}
}
