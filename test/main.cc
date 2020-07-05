#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <arpa/inet.h>  // htons
#include <stdlib.h>     // free
#include <stdint.h>     // uintXX_t
#include <string>       // string
#include <cstring>      // strcpy
#include "arp_test.h"

class ArpTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ArpTest);

  CPPUNIT_TEST(build_arp_request_test);
  CPPUNIT_TEST(ip_char_to_octets_test);

  CPPUNIT_TEST_SUITE_END();

 public:
  void setUp() {}
  void tearDown() {}
  void build_arp_request_test();
  void ip_char_to_octets_test();
};

void ArpTest::build_arp_request_test() {
  struct arp_msg *arp_req = (struct arp_msg *)calloc(1, sizeof(struct arp_msg));
  // ether 02:42:ac:11:00:02
  uint8_t sender_mac[6] = {0x02, 0x42, 0xac, 0x11, 0x00, 0x02};
  // 172.17.0.2
  uint8_t sender_ip[4]  = {0xac, 0x11, 0x00, 0x02};
  // 172.17.0.1
  uint8_t target_ip[4]  = {0xac, 0x11, 0x00, 0x01};

  build_arp_request(arp_req, sender_mac, sender_ip, target_ip);

  CPPUNIT_ASSERT(arp_req->hw_type == htons(HW_TYPE_ETHERNET));
  CPPUNIT_ASSERT(arp_req->prtcl_type == htons(PROTOCOL_IPV4));
  CPPUNIT_ASSERT(arp_req->hw_size == HW_SIZE_MAC);
  CPPUNIT_ASSERT(arp_req->prtcl_size == PROTOCOL_SIZE_IPV4);
  CPPUNIT_ASSERT(arp_req->operation == htons(OPERATION_ARP_REQ));
  // sender mac
  CPPUNIT_ASSERT(arp_req->sender_mac[0] == sender_mac[0]);
  CPPUNIT_ASSERT(arp_req->sender_mac[1] == sender_mac[1]);
  CPPUNIT_ASSERT(arp_req->sender_mac[2] == sender_mac[2]);
  CPPUNIT_ASSERT(arp_req->sender_mac[3] == sender_mac[3]);
  CPPUNIT_ASSERT(arp_req->sender_mac[4] == sender_mac[4]);
  CPPUNIT_ASSERT(arp_req->sender_mac[5] == sender_mac[5]);
  // sender ip
  CPPUNIT_ASSERT(arp_req->sender_ip[0] == sender_ip[0]);
  CPPUNIT_ASSERT(arp_req->sender_ip[1] == sender_ip[1]);
  CPPUNIT_ASSERT(arp_req->sender_ip[2] == sender_ip[2]);
  CPPUNIT_ASSERT(arp_req->sender_ip[3] == sender_ip[3]);
  // target mac
  CPPUNIT_ASSERT(arp_req->target_mac[0] == 0xff);
  CPPUNIT_ASSERT(arp_req->target_mac[1] == 0xff);
  CPPUNIT_ASSERT(arp_req->target_mac[2] == 0xff);
  CPPUNIT_ASSERT(arp_req->target_mac[3] == 0xff);
  CPPUNIT_ASSERT(arp_req->target_mac[4] == 0xff);
  CPPUNIT_ASSERT(arp_req->target_mac[5] == 0xff);
  // target ip
  CPPUNIT_ASSERT(arp_req->target_ip[0] == target_ip[0]);
  CPPUNIT_ASSERT(arp_req->target_ip[1] == target_ip[1]);
  CPPUNIT_ASSERT(arp_req->target_ip[2] == target_ip[2]);
  CPPUNIT_ASSERT(arp_req->target_ip[3] == target_ip[3]);

  free(arp_req);
}

void ArpTest::ip_char_to_octets_test() {
  const std::string ip_str = "172.17.0.2";
  char ip_char[ip_str.length()];
  strcpy(ip_char, ip_str.c_str());

  uint8_t ip[PROTOCOL_SIZE_IPV4];

  int ret = ip_char_to_octets(ip_char, ip);

  CPPUNIT_ASSERT(ret == 0);

  CPPUNIT_ASSERT(ip[0] == 0xac);
  CPPUNIT_ASSERT(ip[1] == 0x11);
  CPPUNIT_ASSERT(ip[2] == 0x00);
  CPPUNIT_ASSERT(ip[3] == 0x02);
}

CPPUNIT_TEST_SUITE_REGISTRATION(ArpTest);

int main () {
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest(registry.makeTest());

  bool wasSuccessful = runner.run();
  return !wasSuccessful;
}
