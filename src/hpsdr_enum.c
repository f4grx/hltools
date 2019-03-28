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

#include <stdio.h>
#include <arpa/inet.h>
#include "p1dev.h"

void enumerator(struct p1dev_s *dev, void *ctx)
  {
    printf("[%02X:%02X:%02X:%02X:%02X:%02X] %s - type %s [%d] (state %s [%d])\n",
           dev->mac[0], dev->mac[1], dev->mac[2],
           dev->mac[3], dev->mac[4], dev->mac[5],
           inet_ntoa(dev->ip),
           p1dev_describe_type(dev->type), dev->type,
           p1dev_describe_state(dev->state), dev->state);
  }

int main(int argc, char **argv)
  {
    int delay = 3;

    printf("hpsdr_enum - (c) 2019 by f4grx, all rights reserved\n");

    printf("Start of enumeration, timeout = %d s\n", delay);
    
    p1dev_discover_async(enumerator, NULL, delay);

    printf("End of enumeration\n");

    return 0;
  }
