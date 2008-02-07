/* w_resrc.c:  Get resources (from files)
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
#include "sim.h"


#ifdef MSDOS
#define PATHSTR	"%s\\%c%c%c%c.%d"
#define PERMSTR	"rb"
#else
#define PATHSTR	"%s/%c%c%c%c.%d"
#define PERMSTR	"r"
#endif


char *HomeDir, *ResourceDir, *KeyDir, *HostName;

struct Resource *Resources = NULL;

struct StringTable {
  QUAD id;
  int lines;
  char **strings;
  struct StringTable *next;
} *StringTables;


Handle GetResource(char *name, QUAD id)
{
  struct Resource *r = Resources;
  char fname[256];
  struct stat st;
  FILE *fp = NULL;

  while (r != NULL) {
    if ((r->id == id) &&
	(strncmp(r->name, name, 4) == 0)) {
      return ((Handle)&r->buf);
    }
    r = r->next;
  }

  r = (struct Resource *)ckalloc(sizeof(struct Resource));

  r->name[0] = name[0];
  r->name[1] = name[1];
  r->name[2] = name[2];
  r->name[3] = name[3];
  r->id = id;

  sprintf(fname, PATHSTR, ResourceDir,
	  r->name[0], r->name[1], r->name[2], r->name[3],
	  r->id);

  if ((stat(fname, &st) < 0) ||
      ((r->size = st.st_size) == 0) ||
      ((r->buf = (char *)ckalloc(r->size)) == NULL) ||
      ((fp = fopen(fname, PERMSTR)) == NULL) ||
      (fread(r->buf, sizeof(char), r->size, fp) != r->size)) {
    if (fp)
      fclose(fp);
    r->buf = NULL;
    r->size = 0;
    fprintf(stderr, "Can't find resource file \"%s\"!\n", fname);
    perror("GetResource");
    return(NULL);
  }
  fclose(fp);
  r->next = Resources; Resources = r;
  return ((Handle)&r->buf);
}


void
ReleaseResource(Handle r)
{
}


QUAD
ResourceSize(Handle h)
{
  struct Resource *r = (struct Resource *)h;

  return (r->size);
}


char *
ResourceName(Handle h)
{
  struct Resource *r = (struct Resource *)h;

  return (r->name);
}


QUAD
ResourceID(Handle h)
{
  struct Resource *r = (struct Resource *)h;

  return (r->id);
}


GetIndString(char *str, int id, short num)
{
  struct StringTable **tp, *st = NULL;
  Handle h;

  tp = &StringTables;

  while (*tp) {
    if ((*tp)->id == id) {
      st = *tp;
      break;
    }
    tp = &((*tp)->next);
  }
  if (!st) {
    QUAD i, lines, size;
    char *buf;

    st = (struct StringTable *)ckalloc(sizeof (struct StringTable));
    st->id = id;
    h = GetResource("stri", id);
    size = ResourceSize(h);
    buf = (char *)*h;
    for (i=0, lines=0; i<size; i++)
      if (buf[i] == '\n') {
	buf[i] = 0;
	lines++;
      }
    st->lines = lines;
    st->strings = (char **)ckalloc(size * sizeof(char *));
    for (i=0; i<lines; i++) {
      st->strings[i] = buf;
      buf += strlen(buf) + 1;
    }
    st->next = StringTables;
    StringTables = st;
  }
  if ((num < 1) || (num > st->lines)) {
    fprintf(stderr, "Out of range string index: %d\n", num);
    strcpy(str, "Well I'll be a monkey's uncle!");
  } {
    strcpy(str, st->strings[num-1]);
  }
}
