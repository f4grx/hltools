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
