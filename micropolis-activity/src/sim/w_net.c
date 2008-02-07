/* w_net.c
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


#ifdef NET


#define NET_BUFFER_SIZE 1024


int net_listen_port;
int net_listen_socket;


int
udp_listen(int port)
{
  struct sockaddr_in addr;
  int flags;

  net_listen_port = port;

  net_listen_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (net_listen_socket < 0) {
    perror("socket()");
    return 0;
  }

  flags = 1;
  if (setsockopt(net_listen_socket, SOL_SOCKET, SO_REUSEADDR,
		 (char *)&flags, sizeof(flags)) == -1) {
    perror("setsockopt SO_REUSEADDR");
    return 0;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = net_listen_port;
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(net_listen_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind()");
    return 0;
  }

  if ((flags = fcntl(net_listen_socket, F_GETFL)) < 0) {
    perror("fcntl F_GETFL");
    return 0;
  }
  
  if (fcntl(net_listen_socket, F_SETFL, flags|O_NDELAY) < 0) {
    perror("fcntl F_SETFL");
    return 0;
  }

  Tcp_MakeOpenFile(tk_mainInterp, net_listen_socket, 1, 1);

  return (net_listen_socket);
}


udp_hear(int sock)
{
  struct sockaddr_in addr;
  int addr_len;
  int len, i;
  unsigned char buf[NET_BUFFER_SIZE];
  char cmd[NET_BUFFER_SIZE * 4 + 256];
  char *cp;

  while (1) {
    len = recvfrom(sock, buf, NET_BUFFER_SIZE, 0,
		   (struct sockaddr *)&addr, &addr_len);

    if (len < 0) {
      if (errno == EINTR) continue;
      if (errno == EWOULDBLOCK) break;
      perror("recvfrom");
      return;
    }

    sprintf(cmd, "HandlePacket %d {%s} {", sock, inet_ntoa(addr.sin_addr));

    cp = cmd + strlen(cmd);
    for (i = 0; i < len; i++) {
      sprintf(cp, "%3d ", buf[i]);
      cp += 4;
    }
    sprintf(cp, "}");
    Eval(cmd);
  }
}


#endif
