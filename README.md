# arp
Raw socket ARP client library for c.

## Build Demo Code
```bash
$ cd src
$ make
```

## Execute Demo Code
```
$ ./main
usage: ./main SENDER_IP TARGET_IP INTERFACE_NAME
  SENDER_IP:         source ip address.
  TARGET_IP:         target ip address.
  INTERFACE_NAME:    interface name to send arp frame.

$ sudo ./main 172.17.0.2 172.17.0.1 eth0
172.17.0.1 -> 02:42:ac:11:00:02
```
