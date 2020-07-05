#ifndef ARP_H_
#define ARP_H_

#include <stdint.h>

#define HW_TYPE_ETHERNET   0x0001
#define PROTOCOL_IPV4      0x0800
#define HW_SIZE_MAC        0x06
#define PROTOCOL_SIZE_IPV4 0x04
#define OPERATION_ARP_REQ  0x0001
#define OPERATION_ARP_RESP 0x0002

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |         Hardware Type         |         Protocol Type         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Hardware Size | Protocol Size |       Operation Code          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Sender Mac Address                      |
 * +                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                               |       Sender IP Address       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |       Sender IP Address       |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 * |                       Target Mac Address                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Target IP Address                       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
struct arp_msg {
  uint16_t hw_type;
  uint16_t prtcl_type;
  uint8_t  hw_size;
  uint8_t  prtcl_size;
  uint16_t operation;
  uint8_t  sender_mac[6];
  uint8_t  sender_ip[4];
  uint8_t  target_mac[6];
  uint8_t  target_ip[4];
};

int get_mac_from_ifname(char *ifname, uint8_t *mac);

int arp_request(char *src_interface,
    uint8_t *sender_mac, char *sender_ip,
    uint8_t *target_mac, char *target_ip);

#endif  // ARP_H_
