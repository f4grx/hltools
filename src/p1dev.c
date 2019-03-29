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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "p1dev.h"

struct p1dev_discover_sync_s
  {
    struct p1dev_s *list;
    int             available;
    int             index;
  };

static const char * gBoardTypes[] =
  {
    "METIS",
    "HERMES",
    "GRIFFIN",
    "(Undefined)",
    "ANGELIA",
    "ORION",
    "HERMESLITE"
  };

static const char * gBoardStates[] =
  {
    "DISCONNECTED",
    "CONNECTED",
    "STREAMINGNARROW",
    "STREAMINGWIDE",
    "TRANSMIT",
    "FULLDUPLEX"
  };

/*----------------------------------------------------------------------------*/
const char *p1dev_describe_type(uint8_t type)
  {
    if(type < sizeof(gBoardTypes))
      {
        return gBoardTypes[type];
      }
    return "(Unknown)";
  }

/*----------------------------------------------------------------------------*/
const char *p1dev_describe_state(uint8_t state)
  {
    if(state < sizeof(gBoardStates))
      {
        return gBoardStates[state];
      }
    return "(Unknown)";
  }

/*----------------------------------------------------------------------------*/
/* Discover protocol 1 devices asynchronously. The callback is triggered
 * each time a device is discovered. Discovery will run for delay seconds.
 */
int p1dev_discover_async(p1dev_cb_f callback, void *context, unsigned int delay)
  {
    int                sock;
    struct sockaddr_in local;
    struct sockaddr_in remote;
    struct sockaddr_in sender;
    int                sender_len;
    struct timespec    rcv_timeout;
    uint8_t            disc[63] = {0xEF, 0xFE, 0x02};
    uint8_t            buf[256];
    int                i_true = 1;
    int                ret;
    struct p1dev_s     dev;

    /* Validate parameters */

    if(callback == NULL || delay == 0)
      {
        return -1;
      }

    /* Create an UDP socket */

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock < 0)
      {
        perror("Creating enumeration socket");
        return -1;
      }

    /* Enable this socket to send broadcasts */

    if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &i_true, sizeof(i_true)) < 0)
      {
        perror("Allowing broadcast operations");
        goto errclose;
      }

    /* Define receive timeout */

    rcv_timeout.tv_sec  = delay;
    rcv_timeout.tv_nsec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
                   &rcv_timeout, sizeof(rcv_timeout)) )
      {
        perror("Setting RX timeout");
        goto errclose;
      }

    /* Bind UDP socket to a local address to receive responses */

    local.sin_family      = AF_INET;
    local.sin_port        = 0;
    local.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
      {
        perror("Binding discovery socket");
        goto errclose;
      }

    /* Send broadcast discovery packet */

    remote.sin_family      = AF_INET;
    remote.sin_port        = htons(1024);
    remote.sin_addr.s_addr = INADDR_BROADCAST;

    if(sendto(sock, disc, sizeof(disc), 0,
              (struct sockaddr*)&remote, sizeof(remote)) < 0)
      {
        perror("Sending discovery packet");
        goto errclose;
      }

    /* Wait for responses */
    while(1)
      {
        sender_len = sizeof(sender);
        ret = recvfrom(sock, buf, sizeof(buf), 0,
                       (struct sockaddr*)&sender, &sender_len);
        if(ret > 0)
          {
            if(ret != 60)
              {
                fprintf(stderr, "reply with unexpected len of %d bytes\n", ret);
                continue;
              }
            if(buf[0] != 0xEF || buf[1] != 0xFE)
              {
                fprintf(stderr, "reply with bad header %02X %02X\n", buf[0]&0xFF, buf[1]&0xFF);
                continue;
              }
            memcpy(dev.mac, buf + 3, 6);
            dev.version = buf[9];
            dev.type    = buf[10];
            dev.state   = P1DEV_STATE_DISCONNECTED;
            dev.ip      = sender.sin_addr;
            callback(&dev, context);
          }
        else if(ret == 0)
          {
            fprintf(stderr, "zero ret, unexpected, please contact f4grx@f4grx.net\n");
          }
        else
          {
            if(errno == EAGAIN)
              {
                //fprintf(stderr, "Time has elapsed.\n");
                break;
              }
            fprintf(stderr, "negative ret = %d, errno=%d\n", ret, errno);
          }
      }

    /* We're done */
    close(sock);
    return 0;

errclose:
    close(sock);
    return -1;
  }

/*----------------------------------------------------------------------------*/
/* Internal callback used to implement the sync version of discovery */
static void p1dev_discover_sync_cb(struct p1dev_s *dev, void *context)
  {
    struct p1dev_discover_sync_s *sync = context;
    if(sync->available)
      {
        memcpy(sync->list + sync->index, dev, sizeof(struct p1dev_s));
        sync->available -= 1;
        sync->index     += 1;
      }
    else
      {
        fprintf(stderr, "Not enough room to enumerate a p1 device!\n");
      }
  }

/*----------------------------------------------------------------------------*/
/* Discover protocol 1 devices and store their info in the devtable.
 * at most maxdevs devices are discovered.
 * Discovery is run for delay seconds.
 */
int p1dev_discover(struct p1dev_s *devtable, int maxdevs, unsigned int delay)
  {
    struct p1dev_discover_sync_s context;
    int ret;

    /* Validate parameters */

    if(devtable == NULL || maxdevs == 0 || delay == 0)
      {
        return -1;
      }

    context.list      = devtable;
    context.available = maxdevs;
    context.index     = 0;

    ret = p1dev_discover_async(p1dev_discover_sync_cb, &context, delay);
    if(ret < 0)
      {
        return ret;
      }

    return context.index;
  }

/*----------------------------------------------------------------------------*/
/* Initialize a protocol 1 device from an explicit IP*/
int p1dev_fromip(struct p1dev_s *device, struct in_addr ip)
  {
    device->ip = ip;
    device->state = P1DEV_STATE_DISCONNECTED;
    return 0;
  }

/*----------------------------------------------------------------------------*/
/* Open the connection to a protocol 1 device */
int p1dev_connect(struct p1dev_s *device)
  {
    int sock;

    fprintf(stderr, "Connect to p1dev at %s\n", inet_ntoa(device->ip));

    if(device->state != P1DEV_STATE_DISCONNECTED)
      {
        fprintf(stderr, "Device is already connected!\n");
        return -1;
      }

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock < 0)
      {
        perror("Connecting to p1dev");
        return -1;
      }

    device->sock  = sock;
    device->state = P1DEV_STATE_CONNECTED;
    return 0;
  }

/*----------------------------------------------------------------------------*/
/* Release connection to a protocol 1 device */
int p1dev_disconnect(struct p1dev_s *device)
  {
    fprintf(stderr, "Disconnect from p1dev at %s\n", inet_ntoa(device->ip));

    if(device->state == P1DEV_STATE_DISCONNECTED)
      {
        fprintf(stderr, "Device is already connected!\n");
        return -1;
      }

    if(close(device->sock) < 0)
      {
        perror("Disconnecting from p1dev");
        return -1;
      }
    device->state = P1DEV_STATE_DISCONNECTED;
    return 0;
  }

/*----------------------------------------------------------------------------*/
/* Start receiving IQ samples from the narrow band receiver.
 * Samples will be stored in the buffer, that must be provided
 * by the caller.
 */
int p1dev_start_narrow(struct p1dev_s *device, void *buffer1024,
                       p1dev_narrow_cb_f *rxcallback)
  {
    return 0;
  }

/*----------------------------------------------------------------------------*/
/* Stop receiving narrow band data */
int p1dev_stop_narrow(struct p1dev_s *device)
  {
    return 0;
  }

/*----------------------------------------------------------------------------*/
/* Transmit some IQ samples */
int p1dev_send_narrow(struct p1dev_s *device, void *buffer1024)
  {
    return 0;
  }

/*----------------------------------------------------------------------------*/
/* Start receiving bandscope data. Data is stored in the buffer
 * that must be allocated by the caller. The RX callback is only
 * called when a full set of bandscope data has been received.
 */
int p1dev_start_wide(struct p1dev_s *device, void *buffer16384,
                     p1dev_wide_cb_f *rxcallback)
  {
    return 0;
  }

/*----------------------------------------------------------------------------*/
/* Stop receiving bandscope data */
int p1dev_stop_wide(struct p1dev_s *device)
  {
    return 0;
  }

