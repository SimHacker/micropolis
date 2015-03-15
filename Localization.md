# Introduction #

This page describes how to translate the text used in the
Java edition of Micropolis. This will allow Micropolis to be
played in any language.


# Details #

Micropolis (Java edition) reads translated messages from
[Java resource bundles](http://en.wikipedia.org/wiki/Java_resource_bundle). You can edit the resource files directly, if you know
how to do that, but starting with version 1.3 there is a built-in
tool to help you.

To begin working on translations:
  1. Start Micropolis (Java edition), and select "Translation Tool" from the "Help" menu.
  1. Click Add Locale, and enter the 2-letter language and/or country codes for the language you want to work on. The language code should be two **lowercase** letters, and the country code should be two **uppercase** letters.
  1. You should now see a column for your language. Click on each cell and enter translated text.
  1. Some of the strings are paths to graphics files, or are color codes. You probably should NOT translate these strings, so just leave the corresponding cells blank.
  1. To try running Micropolis with the translated strings, click the **Test** button. This will launch a copy of Micropolis using the translated strings you entered.
  1. When you are finished working on the language, click **Submit**. This will tell you what files you should upload so that the next version of Micropolis can include your strings.