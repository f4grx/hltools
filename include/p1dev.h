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

enum
  {
    P1DEV_TYPE_METIS,
    P1DEV_TYPE_HERMES,
    P1DEV_TYPE_GRIFFIN,
    P1DEV_TYPE_UNDEFINED,
    P1DEV_TYPE_ANGELIA,
    P1DEV_TYPE_ORION,
    P1DEV_TYPE_HERMESLITE
  };

/* protocol 1 device management */

struct p1dev_s
  {
    struct in_addr ip;      /* IP address of the board */
    uint8_t        mac[6];  /* MAC address of the board */
    int            type;    /* Device type */
    uint8_t        version; /* FPGA gateware version */
    int            state;   /* Device state: stopped, started narrow, started wide */
    int            sock;    /* socket handle used for communication */
  };

/* This callback is called by the async version of the discovery process.
 */
typedef void (*p1dev_cb_f)(struct p1dev_s *device, void *context);
typedef void (*p1dev_narrow_cb_f)(struct p1dev_s *device, void *context,
                                  void *buffer1024);
typedef void (*p1dev_wide_cb_f)(struct p1dev_s *device, void *context,
                                void *buffer16384);

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

/* Return string values for some enumerations */
const char *p1dev_describe_type(uint8_t type);
const char *p1dev_describe_state(uint8_t type);

/* Open the connection to a protocol 1 device */
int p1dev_connect(struct p1dev_s *device);

/* Release connection to a protocol 1 device */
int p1dev_disconnect(struct p1dev_s *device);

/* Start receiving IQ samples from the narrow band receiver.
 * Samples will be stored in the buffer, that must be provided
 * by the caller.
 */
int p1dev_start_narrow(struct p1dev_s *device, void *buffer1024,
                       p1dev_narrow_cb_f *rxcallback);

/* Stop receiving narrow band data */
int p1dev_stop_narrow(struct p1dev_s *device);

/* Transmit some IQ samples */
int p1dev_send_narrow(struct p1dev_s *device, void *buffer1024);

/* Start receiving bandscope data. Data is stored in the buffer
 * that must be allocated by the caller. The RX callback is only
 * called when a full set of bandscope data has been received.
 */
int p1dev_start_wide(struct p1dev_s *device, void *buffer16384,
                     p1dev_wide_cb_f *rxcallback);

/* Stop receiving bandscope data */
int p1dev_stop_wide(struct p1dev_s *device);

#endif /* __P1DEV__H__ */
