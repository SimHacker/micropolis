#!/usr/bin/env python
#
# Generate tile animation table.
#
# It takes a file with lines of the form
#
#    x080 -> x070 -> x060 -> x050 -> x080
#
# and generates a table where each entry in the table points to the next
# entry. For the above example, that means:
#
# table[0x80] = 0x70; table[0x70] = 0x60; table[0x50] = 0x80;
#
#
# The program takes both decimal and hexadecimal values. Also, empty lines and
# everything after a '#' is skipped.
# Every value not mentioned points to itself.
#
# TODO: Instead of pointing to self, the code could disable the animation in
#       some way, which may reduce CPU time used by the animation.
#
import sys

TOO_BIG_VALUE = 1024  #: All tile numbers must stay below this value
INITIAL_INDENT = "    " # Initial indent text
NUM_VALUES = 16 #: Number of values at one line


def decode_val(val_text, hexnum):
    """
    Decode tile value text to its numerical equvalent.

    @param val_text: Value text to decode.
    @type  val_text: C{str}

    @param hexnum: Value is a hexadecimal number.
    @type  hexnum: C{bool}

    @return: Its numeric value if it can be decoded, C{None} if it cannot be
             decoded.
    @rtype:  C{int} or C{None}
    """
    try:
        if hexnum:
            return int(val_text, 16)
        else:
            return int(val_text, 10)
    except ValueError:
        return None


def load_lines(fname):
    """
    Read the animation sequence lines from the file.

    @param fname: Name of the file to load.
    @type  fname: C{str}

    @return: Sequence (line number, tile number sequence).
    @rtype:  C{list} of (C{int}, C{list} of C{int})
    """
    fp = open(fname, 'r')
    lines = []

    lnumber = 0
    for line in fp:
        lnumber = lnumber + 1
        # Chop off comment
        i = line.find('#')
        if i >= 0:
            line = line[:i].strip()
        else:
            line = line.strip()

        if len(line) == 0:
            continue  # Ignore empty lines

        values = []
        for val_text in (val.strip() for val in line.split('->')):
            if val_text[0] in 'xX':
                val_text = val_text[1:]
                hexnum = True
            else:
                hexnum = False

            val = decode_val(val_text, hexnum)
            if val is None:
                sys.stderr.write('Cannot decode value %r at line %d\n', val_text, lnumber)
                sys.exit(1)
            if val < 0 or val >= TOO_BIG_VALUE:
                sys.stderr.write('Value %r at line %d is too small or too big\n', val_text, lnumber)
                sys.exit(1)

            values.append(val)

        lines.append((lnumber, values))

    fp.close()
    return lines

def fill_next_table(lines):
    """
    Get the sequences from the file, and construct a 'next' table.

    We make sure we don't create impossible animations (two 'next' values for
    one tile).

    @param lines: Lines loaded from the file.

    @return: A table with 'next' values as defined in the L{lines}.
    @rtype:  C{dict} of  C{int} to C{int}
    """
    next_table = {}
    line_table = {}  #: For each value, at what line it is set

    for lnum, vals in lines:
        prev = None
        for val in vals:
            if prev is not None:
                if prev not in next_table:
                    next_table[prev] = val
                    line_table[prev] = lnum
                elif next_table[prev] != val:
                    sys.stderr.write("Impossible sequence: Two 'next' tiles for value "
                                    "%d (at lines %d and %d)\n"
                                        % (prev, line_table[prev], lnum))
                    sys.exit(1)
                # else: Entry already in table, and same successor -> do nothing

            prev = val

    return next_table

def output_table(next_table):
    value = 0 #: Number to output next
    line_count = 0 #: Number of values printed at the line until now

    while value < TOO_BIG_VALUE:
        if line_count == 0:
            sys.stdout.write(INITIAL_INDENT)
        else:
            sys.stdout.write(" ")

        if value in next_table:
            sys.stdout.write(str(next_table[value]))
        else:
            sys.stdout.write(str(value))

        value = value + 1
        if value < TOO_BIG_VALUE: # We need to print another number
            sys.stdout.write(",")

        line_count = line_count + 1
        if line_count >= NUM_VALUES:
            sys.stdout.write("\n")
            line_count = 0

    if line_count > 0:
        sys.stdout.write('\n')

if __name__ == '__main__':
    lines = load_lines('anisequences.txt')
    next_table = fill_next_table(lines)
    output_table(next_table)

