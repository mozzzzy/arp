#ifndef ARP_TEST_H_
#define ARP_TEST_H_

#include <arp.h>

void build_arp_request(struct arp_msg *arp,
    uint8_t *sender_mac, uint8_t *sender_ip, uint8_t *target_ip);

int ip_char_to_octets(char *ip_char, uint8_t *ip);

#endif  // ARP_TEST_H_
