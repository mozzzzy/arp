#include <stdio.h>
#include <stdint.h> // uintXX_t
#include "arp.h"

int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("usage: %s SENDER_IP TARGET_IP INTERFACE_NAME\n", argv[0]);
    printf("  SENDER_IP:         source ip address.\n");
    printf("  TARGET_IP:         target ip address.\n");
    printf("  INTERFACE_NAME:    interface name to send arp frame.\n");
    return 1;
  }

  char *src_interface = argv[3];

  int ret = 0;
  uint8_t src_mac[6];
  ret = get_mac_from_ifname(src_interface, src_mac);
  if (ret > 0) {
    printf("failed to resolve mac address from interface name.\n");
    return -1;
  }

  uint8_t target_mac[6];
  ret = arp_request(src_interface, src_mac, argv[1], target_mac, argv[2]);
  if (ret > 0) {
    printf("failed to resolve mac address from ip address.\n");
    return -1;
  }

  printf("%s -> %02x:%02x:%02x:%02x:%02x:%02x\n", argv[2],
      target_mac[0], target_mac[1], target_mac[2],
      target_mac[3], target_mac[4], target_mac[5]);

  return 0;
}
