#include <arpa/inet.h>        // htons
#include <net/if.h>           // ifreq
#include <net/ethernet.h>     // ETH_P_ARP
#include <netpacket/packet.h> // sockaddr_ll
#include <stdlib.h>           // calloc, free
#include <string.h>           // strncpy
#include <sys/ioctl.h>        // ioctl
#include <sys/socket.h>       // socket
#include <unistd.h>           // close
#include "arp.h"

#define HW_TYPE_ETHERNET   0x0001
#define PROTOCOL_IPV4      0x0800
#define HW_SIZE_MAC        0x06
#define PROTOCOL_SIZE_IPV4 0x04
#define OPERATION_ARP_REQ  0x0001
#define OPERATION_ARP_RESP 0x0002

void build_arp_msg(struct arp_msg *arp, uint16_t ope_code,
    uint8_t *sender_mac, uint8_t *sender_ip,
    uint8_t *target_mac, uint8_t *target_ip) {
  arp->hw_type    = htons(HW_TYPE_ETHERNET);
  arp->prtcl_type = htons(PROTOCOL_IPV4);
  arp->hw_size    = HW_SIZE_MAC;
  arp->prtcl_size = PROTOCOL_SIZE_IPV4;
  arp->operation  = htons(ope_code);

  int i;
  for (i = 0; i < sizeof(arp->sender_mac)/sizeof(uint8_t); ++i) {
    arp->sender_mac[i] = sender_mac[i];
  }
  for (i = 0; i < sizeof(arp->sender_ip)/sizeof(uint8_t); ++i) {
    arp->sender_ip[i]  = sender_ip[i];
  }
  for (i = 0; i < sizeof(arp->target_mac)/sizeof(uint8_t); ++i) {
    arp->target_mac[i] = target_mac[i];
  }
  for (i = 0; i < sizeof(arp->target_ip)/sizeof(uint8_t); ++i) {
    arp->target_ip[i]  = target_ip[i];
  }
  return;
}

void build_arp_request(struct arp_msg *arp,
    uint8_t *sender_mac, uint8_t *sender_ip,
    uint8_t *target_mac, uint8_t *target_ip) {
  int i = 0;
  for (i = 0; i < HW_SIZE_MAC; ++i) {
    target_mac[i] = 0xff;
  }
  return build_arp_msg(arp, OPERATION_ARP_REQ,
      sender_mac, sender_ip, target_mac, target_ip);
}

int get_mac_from_ifname(char *ifname, uint8_t *mac) {
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    return sock;
  }

  struct ifreq ifr;
  ifr.ifr_addr.sa_family = AF_INET;
  strncpy(ifr.ifr_name, ifname, IFNAMSIZ-1);

  int ret = ioctl(sock, SIOCGIFHWADDR, &ifr);
  close(sock);
  if (ret < 0) {
    return ret;
  }

  int i = 0;
  for (i = 0; i < HW_SIZE_MAC; ++i) {
    mac[i] = (uint8_t)ifr.ifr_hwaddr.sa_data[i];
  }
  return 0;
}

int send_arp_msg(int sock, struct arp_msg *arp, char *src_interface) {
  struct sockaddr_ll addr;
  memset(&addr, 0, sizeof(addr));
  addr.sll_family = AF_PACKET;
  addr.sll_protocol = htons(ETH_P_ARP);
  addr.sll_ifindex = if_nametoindex(src_interface);
  addr.sll_halen = IFHWADDRLEN;
  memset(&addr.sll_addr, 0xff, IFHWADDRLEN);  // broadcast

  return sendto(sock, arp, sizeof(struct arp_msg), 0, (struct sockaddr *)&addr, sizeof(addr));
}

int receive_arp_msg(int sock, struct arp_msg *arp) {
  char buf[2048];
  size_t rcv_size = recv(sock, buf, sizeof(buf)/sizeof(char), 0);
  if (rcv_size < 1) {
    return rcv_size;
  }
  int i;
  for(i = 0; i < HW_SIZE_MAC; ++i) {
    arp->target_mac[i] = ((struct arp_msg *)buf)->target_mac[i];
  }
}

int ip_char_to_octets(char *ip_char, uint8_t *ip) {
  uint32_t ip_uint32_t = inet_addr(ip_char);
  // if failed to parse ip string
  if (ip_uint32_t == INADDR_NONE) {
    return -1;
  }
  int i;
  for (i = 0; i < sizeof(uint32_t)/sizeof(uint8_t); ++i) {
    uint8_t *ptr = (uint8_t *)&ip_uint32_t+i;
    ip[i] = *ptr;
  }
  ip = (uint8_t *)&ip_uint32_t;
  return 0;
}

int arp_request(char *src_interface,
    uint8_t *sender_mac, char *sender_ip,
    uint8_t *target_mac, char *target_ip) {

  int ret;
  uint8_t sender_ip_octs[4];
  uint8_t target_ip_octs[4];
  ret = ip_char_to_octets(sender_ip, sender_ip_octs);
  ret = ip_char_to_octets(target_ip, target_ip_octs);

  struct arp_msg *arp_req = calloc(1, sizeof(struct arp_msg));
  build_arp_request(arp_req,
      sender_mac, sender_ip_octs,
      target_mac, target_ip_octs);

  int sock = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP));
  if (sock < 0) {
    return sock;
  }

  int send_size = send_arp_msg(sock, arp_req, src_interface);
  if (send_size < 0) {
    free(arp_req);
    close(sock);
    return send_size;
  }

  struct arp_msg *arp_rsp = calloc(1, sizeof(struct arp_msg));
  int rcv_size = receive_arp_msg(sock, arp_rsp);
  if (rcv_size < 1) {
    free(arp_req);
    free(arp_rsp);
    close(sock);
    return -1;
  }

  int i;
  for (i = 0; i < HW_SIZE_MAC; ++i) {
    target_mac[i] = arp_rsp->target_mac[i];
  }
  free(arp_req);
  free(arp_rsp);
  close(sock);
  return 0;
}
