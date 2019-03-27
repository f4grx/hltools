#include <stdio.h>
#include <arpa/inet.h>
#include "p1dev.h"

void enumerator(struct p1dev_s *dev, void *ctx)
  {
    printf("[%02X:%02X:%02X:%02X:%02X:%02X] %s - type %d (state %d)\n",
           dev->mac[0], dev->mac[1], dev->mac[2],
           dev->mac[3], dev->mac[4], dev->mac[5],
           inet_ntoa(dev->ip), dev->type, dev->state);
  }

int main(int argc, char **argv)
  {
    printf("hpsdr_enum - (c) 2019 by f4grx, all rights reserved\n");

    printf("Start of enumeration\n");
    
    p1dev_discover_async(enumerator, NULL, 20);

    printf("End of enumeration\n");

    return 0;
  }
