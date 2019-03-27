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

/*----------------------------------------------------------------------------*/
/* Discover protocol 1 devices asynchronously. The callback is triggered
 * each time a device is discovered. Discovery will run for delay seconds.
 */
int p1dev_discover_async(p1dev_cb_f callback, void *context, int delay)
  {
    int                sock;
    struct sockaddr_in local;
    struct sockaddr_in remote;
    struct sockaddr_in sender;
    int                sender_len;
    struct timespec    rcv_timeout;
    char               disc[63] = {0xEF, 0xFE, 0x02};
    char               buf[256];
    int                i_true = 1;
    int                ret;

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
    remote.sin_port        = 1024;
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
            fprintf(stderr, "positive ret=%d\n", ret);
          }
        else if(ret == 0)
          {
            fprintf(stderr, "zero ret\n");
          }
        else
          {
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
int p1dev_discover(struct p1dev_s *devtable, int maxdevs, int delay)
  {
    struct p1dev_discover_sync_s context;
    int ret;

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
  }

/*----------------------------------------------------------------------------*/
/* Open the connection to a protocol 1 device */
int p1dev_connect(struct p1dev_s *device)
  {
    int sock;
    fprintf(stderr, "Connect to p1dev at %s\n", inet_ntoa(device->ip));

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock < 0)
      {
        perror("Connecting to p1dev");
        return -1;
      }

    device->sock = sock;
    device->state = P1DEV_STATE_CONNECTED;
    return 0;
  }

/*----------------------------------------------------------------------------*/
/* Release connection to a protocol 1 device */
int p1dev_disconnect(struct p1dev_s *device)
  {
    fprintf(stderr, "Disconnect from p1dev at %s\n", inet_ntoa(device->ip));
    if(close(device->sock) < 0)
      {
        perror("Disconnecting from p1dev");
        return -1;
      }
    device->state = P1DEV_STATE_DISCONNECTED;
    return 0;
  }

