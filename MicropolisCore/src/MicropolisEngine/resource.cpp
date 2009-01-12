/* resource.cpp
 * Get resources (from files)
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 *
 *             ADDITIONAL TERMS per GNU GPL Section 7
 *
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 *
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 *
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 *
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 *
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */

/** @file resource.cpp
 * Get resources (from files)
 */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


////////////////////////////////////////////////////////////////////////

/**
 * Find the resource with the given name and identification.
 * @param name Name of the resource (a 4 character string)
 * @param id   Identification of the resource.
 * @return Pointer to the resource.
 * @bug Function is not safely handling strings.
 * @bug File handling is not safe across platforms (text-mode may modify data).
 * @todo What is the point of a \c Quad \a id when we cast it to an \c int ?
 */
Resource *Micropolis::getResource(const char *name, Quad id)
{
    Resource *r = resources;
    char fname[4096];

    while (r != NULL) {
        if (r->id == id && strncmp(r->name, name, 4) == 0) {
            return r;
        }
        r = r->next;
    }

    // Resource not found, load it from disk

    // Allocate memory for the resource administration itself
    r = (Resource *)NewPtr(sizeof(Resource));
    assert(r != NULL);

    /// @bug Not safe!
    r->name[0] = name[0];
    r->name[1] = name[1];
    r->name[2] = name[2];
    r->name[3] = name[3];
    r->id = id;

    // Load the file into memory

    /// @bug Not safe (overflow, non-printable chars)
    sprintf(fname, "%s/%c%c%c%c.%d", resourceDir.c_str(),
                        r->name[0], r->name[1], r->name[2], r->name[3],
                        (int)r->id);

    struct stat st;
    FILE *fp = NULL;

    if (stat(fname, &st) < 0) {  // File cannot be found/loaded
        goto loadFailed;
    }
    if (st.st_size == 0) { // File is empty
        goto loadFailed;
    }

    r->size = st.st_size;
    r->buf = (char *)NewPtr(r->size);
    if (r->buf == NULL) { // No memory allocated
        goto loadFailed;
    }

    // XXX Opening in text-mode
    fp = fopen(fname, "r"); // Open file for reading
    if (fp == NULL) {
        goto loadFailed;
    }

    // File succesfully opened. Below here we must always close the file!!

    // XXX This may break due to use of text-mode
    if ((int)fread(r->buf, sizeof(char), r->size, fp) != r->size) {
        fclose(fp);
        goto loadFailed;
    }

    // File-load ok !!
    fclose(fp);

    r->next = resources;
    resources = r;

    return r;



loadFailed:
    // Load failed, print an error and quit
    r->buf = NULL;
    r->size = 0;
    fprintf(stderr, "Can't find resource file \"%s\"!\n", fname);
    perror("getResource");
    return NULL;
}

/**
 * Get the text of a message.
 * @param str Destination of the text (usually 256 characters long).
 * @param id  Identification of the resource.
 * @param num Number of the string in the resource.
 * @bug Make the function safe (should never overwrite data outside \a str,
 *      handle case where last line of file is not terminated with new-line)
 * @bug Out of range \a num seems not correctly handled (\c strcpy(str,"Oops")
 *      is overwritten at least. Maybe use an \c assert() instead?).
 * @todo Why do we copy the text? Can we not return its address instead?
 */
void Micropolis::getIndString(char *str, int id, short num)
{
    StringTable *tp, *st;

    // Try to find requested string table in already loaded files.
    tp = stringTables;
    st = NULL;
    while (tp != NULL) {
        if (tp->id == id) {
            st = tp;
            break;
        }
        tp = tp->next;
    }

    if (st == NULL) {
        // String table is not loaded yet -> get it

        // Create new string table
        st = (StringTable *)NewPtr(sizeof(StringTable));
        assert(st != NULL);

        st->id = id;
        Resource *r = getResource("stri", id);
        Quad size = r->size;
        char *buf = r->buf;

        // Count number of lines in loaded file, terminate each line
        Quad lines = 0;
        for (Quad i = 0; i < size; i++) {
            if (buf[i] == '\n') {
                buf[i] = '\0';
                lines++;
            }
        }

        // XXX What about termination of last line?

        st->lines = lines;
        st->strings = (char **)NewPtr(size * sizeof(char *));
        assert(st->strings != NULL);

        // Store starting points of texts in st->strings array
        for (Quad i = 0; i < lines; i++) {
            st->strings[i] = buf;
            buf += strlen(buf) + 1;
        }

        st->next = stringTables;
        stringTables = st;
    }

    // st points to the (possibly just loaded) string table
    assert(num >= 1 && num <= st->lines); // Stay in range of the file

    strcpy(str, st->strings[num - 1]);
}


////////////////////////////////////////////////////////////////////////

