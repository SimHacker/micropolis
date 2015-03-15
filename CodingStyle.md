# Introduction #

This document describes the coding style for Micropolis.

The code does not yet subscribe to these style guidelines in all places (or even any).

## Python ##

Four space indentation.

Use spaces instead of tabs.

Always use trailing commas in lists and tuples, so the last element is not different, and you don't have to do extra work (or forget to do the work and cause syntax errors) when adding a new element to the end of the list. This policy also avoids the problem of "(1)" evaluating to 1 instead of "(1,)".

## C++ ##

Most of these style guidelines also apply to JavaScript.

Four space indentation.

Open braces on the same line as control structures.

Close and open braces before and after "else" on the same line.

Break long lines up into one logical sub-expression per line, properly indented.

Use appropriately named temporary variables, instead of complex expressions, or whenever repeating a value.

Don't drill down again and again into the same structure or function call -- always put a value into a descriptive temporary variable if it's used more than once.

Put magic numbers into descriptive variables, so they can be documented and configured.

No weird added or omitted white space.
No creative indentation.

Always use white space around operators.
Space between control structure keyword and open paren.

Never insert extra white space before and after parens, or in other weird places.
No space between function name and open paren for parameters.

## OpenLaszlo ##

Use the same style that Laszlo Systems uses to format the OpenLaszlo core code.

[TODO: find a link to any style guides that OpenLaszlo publishes.]

Apply XML formatting style to the XML text, and JavaScript formatting style to the JavaScript text.

## XML ##

Four space indentation.

Usually put each attribute on its own line, unless you're writing short compact data.

You can put an id or name attribute on the same line as the tag.

Put the "/>" after the last attribute on its own line.

This makes it easier to copy and paste attributes around with line oriented editing commands, without having to do a lot of cursor motion and precise selection.

## JavaScript ##

Apply the C++ coding style guidelines as appropriate.

Be careful about using "this" in callbacks.
Use "makeCallback" (or whatever it's called, using apply or call to bind "this"), instead of the old "var me = this;" hack.

Never use "with".

When accessing values up in general purpose dictionaries (as opposed to actual objects), use foo['key'] instead of foo.key, to be explicit about it being a dictionary as opposed to an object. Use foo.key to access keys of objects, not foo['key']. (Except when accessing a dynamic key like foo[dynamicKey](dynamicKey.md), of course).

OpenLaszlo JavaScript does not currently support try/catch. But you can use it in browser JavaScript.