#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sstream>
#include "netif_info.h"


void Usage() {
  printf("Usage ./commond [net if name]\n");
}

std::string get_speed_str(uint64_t speed){
  std::stringstream ss;
  if(speed / (1024 * 1024 * 1024) != 0) {
    ss << speed / (1024 * 1024 * 1024);
    if(speed % (1024 * 1024 * 1024) != 0){
      ss << ".";
      ss << ((speed % (1024 * 1024 * 1024)) * 100) / (1024 * 1024 * 1024);
    }
    ss << " Gb/s";
    return ss.str();
  }
  if(speed / (1024 * 1024) != 0) {
    ss << speed / (1024 * 1024);
    if(speed % (1024 * 1024) != 0){
      ss << ".";
      ss << ((speed % (1024 * 1024)) * 100) / (1024 * 1024);
    }
    ss << " Mb/s";
    return ss.str();
  }
  if(speed / (1024) != 0) {
    ss << speed / (1024);
    if(speed % (1024) != 0){
      ss << ".";
      ss << ((speed % (1024)) * 100) / 1024;
    }
    ss << " Kb/s";
    return ss.str();
  }
  ss << speed;
  ss << " byte/s";
  return ss.str();
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    Usage();
    return -1;
  }
  ifm::NetIfInfo netif_info(argv[1]);
  while (1) {
    sleep(1);
    printf("ifname:%s tx_speed:%s rx_speed:%s\n",
      netif_info.GetIfName().c_str(), get_speed_str(netif_info.GetTxSpeed()).c_str(), get_speed_str(netif_info.GetRxSpeed()).c_str());
  }
}
