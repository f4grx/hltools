/*
 * newhpsdr - an implementation of the hpsdr protocol
 * Copyright (C) 2019 Sebastien Lorquet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "p1dev.h"

#define NDEVICES 4
struct p1dev_s gDevices[NDEVICES];

int main(int argc, char **argv)
  {
    int count;
    int i;

    printf("hpsdr_dummyrx - (c) 2019 by f4grx, all rights reserved\n");

    count = p1dev_discover(gDevices, NDEVICES, 3);

    printf("Found %d devices\n", count);

    if(!count)
      {
        return 1;
      }

    for(i = 0; i < count; i++)
      {
        struct p1dev_s *dev = gDevices + i;
        printf("%2d. [%02X:%02X:%02X:%02X:%02X:%02X] %s - type %s [%d] (state %s [%d])\n",
               i,
               dev->mac[0], dev->mac[1], dev->mac[2],
               dev->mac[3], dev->mac[4], dev->mac[5],
               inet_ntoa(dev->ip),
               p1dev_describe_type(dev->type), dev->type,
               p1dev_describe_state(dev->state), dev->state);
      }

    printf("Using first device\n");

    return 0;
  }
