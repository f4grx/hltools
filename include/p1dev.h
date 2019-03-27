#ifndef __P1DEV__H__
#define __P1DEV__H__

#include <stdint.h>
#include <netinet/in.h>

enum
  {
    P1DEV_STATE_DISCONNECTED,
    P1DEV_STATE_CONNECTED,
    P1DEV_STATE_STREAMINGNARROW,
    P1DEV_STATE_STREAMINGWIDE,
    P1DEV_STATE_TRANSMIT,
    P1DEV_STATE_FULLDUPLEX
  };

/* protocol 1 device management */

struct p1dev_s
  {
    struct in_addr ip;     /* IP address of the board */
    uint8_t        mac[6]; /* MAC address of the board */
    int            type;   /* Device type */
    int            state;  /* Device state: stopped, started narrow, started wide */
    int            sock;   /* socket handle used for communication */
  };

/* This callback is called by the async version of the discovery process.
 */
typedef void (*p1dev_cb_f)(struct p1dev_s *device, void *context);

/* Discover protocol 1 devices asynchronously. The callback is triggered
 * each time a device is discovered. Discovery will run for delay seconds.
 */
int p1dev_discover_async(p1dev_cb_f callback, void *context, int delay);

/* Discover protocol 1 devices and store their info in the devtable.
 * at most maxdevs devices are discovered.
 * Discovery is run for delay seconds.
 */
int p1dev_discover(struct p1dev_s *devtable, int maxdevs, int delay);

/* Initialize a protocol 1 device from an explicit IP*/
int p1dev_fromip(struct p1dev_s *device, struct in_addr ip);

/* Open the connection to a protocol 1 device */
int p1dev_connect(struct p1dev_s *device);

/* Release connection to a protocol 1 device */
int p1dev_disconnect(struct p1dev_s *device);


#endif /* __P1DEV__H__ */
