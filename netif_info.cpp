#include "netif_info.h"
#include <fstream>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>

namespace ifm {
NetIfInfo::NetIfInfo(const std::string &if_name){
  m_ifname = if_name;
  m_tx_flow = 0;
  m_tx_flow_base = 0;
  m_tx_flow_base_time = 0;
  m_rx_flow = 0;
  m_rx_flow_base = 0;
  m_rx_flow_base_time = 0;

  m_speed_last_tx_flow = -1;
  m_speed_last_tx_flow_time = 0;
  m_speed_last_rx_flow = -1;
  m_speed_last_rx_flow_time = 0;
  m_speed_tx_last = 0;
  m_speed_rx_last = 0;
  uint64_t cur_time = 0;
  GetCurFlow(m_tx_flow_base, m_rx_flow_base, cur_time);
  m_tx_flow_base_time = cur_time;
  m_rx_flow_base_time = cur_time;
  m_speed_last_tx_flow = m_tx_flow_base;
  m_speed_last_rx_flow = m_rx_flow_base;
  m_speed_last_rx_flow_time = cur_time;
  m_speed_last_tx_flow_time = cur_time;
}
NetIfInfo::~NetIfInfo() {
}
std::string NetIfInfo::GetIfName() {
  return m_ifname;
}
bool NetIfInfo::GetTxFlow(uint64_t & flow, uint64_t & time_ms) {
  bool ret = false;
  flow = 0;
  time_ms = 0;
  uint64_t cur_time = 0;
  ret = GetCurFlow(m_tx_flow, m_rx_flow, cur_time);
  if (!ret) {
    return false;
  }
  flow = m_tx_flow - m_tx_flow_base;
  time_ms = cur_time - m_tx_flow_base_time;
  return true;
}
bool NetIfInfo::GetRxFlow(uint64_t & flow, uint64_t & time_ms) {
  bool ret = false;
  flow = 0;
  time_ms = 0;
  uint64_t cur_time = 0;
  ret = GetCurFlow(m_tx_flow, m_rx_flow, cur_time);
  if (!ret) {
    return false;
  }
  flow = m_rx_flow - m_rx_flow_base;
  time_ms = cur_time - m_rx_flow_base_time;
  return true;
}
uint64_t NetIfInfo::GetTxSpeed() {
  uint64_t cur_time = 0;
  uint64_t tx_flow, rx_flow;
  if (!GetCurFlow(tx_flow, rx_flow, cur_time)) {
    return 0;
  }
  if (cur_time == m_speed_last_tx_flow_time) {
    return m_speed_tx_last;
  }
  uint64_t speed = 0;
  if(m_speed_last_tx_flow  != -1) {
    speed = ((tx_flow - m_speed_last_tx_flow) * 1000) / (cur_time - m_speed_last_tx_flow_time);
  } 
  m_speed_tx_last = speed;
  m_speed_last_tx_flow = tx_flow;
  m_speed_last_tx_flow_time = cur_time;
  return speed;
}
uint64_t NetIfInfo::GetRxSpeed() {
  uint64_t cur_time = 0;
  uint64_t tx_flow, rx_flow;
  if (!GetCurFlow(tx_flow, rx_flow, cur_time)) {
    return 0;
  }
  if (cur_time == m_speed_last_rx_flow_time) {
    return m_speed_rx_last;
  }
  uint64_t speed = 0;
  if(m_speed_last_tx_flow  != -1) {
    speed = ((rx_flow - m_speed_last_rx_flow) * 1000) / (cur_time - m_speed_last_rx_flow_time);
  } 
  m_speed_rx_last = speed;
  m_speed_last_rx_flow = rx_flow;
  m_speed_last_rx_flow_time = cur_time;
  return speed;
}
bool NetIfInfo::GetCurFlow(uint64_t & tx, uint64_t & rx, uint64_t &cur_time) {
  std::ifstream ifs("/proc/net/dev");
  if (!ifs.is_open()) {
    LOGE("open file: /proc/net/dev failed");
    return false;
  }
  std::string flow_data = "";
  while (!ifs.eof()) {
    std::string line;
    std::getline(ifs, line);
    if (line.find(m_ifname) == std::string::npos) {
      continue;
    }
    flow_data = line;
    break;
  }
  if (flow_data.empty()) {
    LOGE("not found ifname:%s info",m_ifname.c_str());
    return false;
  }
  flow_data = flow_data.substr(flow_data.find(m_ifname) + m_ifname.length());
  LOGD("flow data:%s",flow_data.c_str());
  cur_time = GetCurMs();
  uint32_t i;
  uint32_t num_idx = 0;
  uint32_t num_st_addr = 0;
  uint32_t num_end_addr = 0;
  char last_data = '\0';
  for (i = 0; i < flow_data.size(); i++) {
    if (flow_data[i]  >= '0' && flow_data[i] <= '9' && 
      (last_data < '0' || last_data > '9')) {
      LOGD("found number start addr:%d",i);
      num_st_addr = i;
    }
    if (last_data >= '0' && last_data <= '9' &&
      (flow_data[i] < '0' || flow_data[i] > '9')) {
      LOGD("found number end addr:%d",i);
      num_end_addr = i;
      if (num_idx == 0) {
        rx = std::atol(flow_data.substr(num_st_addr, num_end_addr - num_st_addr).c_str());
        LOGD("get rx flow:%lld",rx);
      } else if (num_idx == 8) {
        tx = std::atol(flow_data.substr(num_st_addr, num_end_addr - num_st_addr).c_str());
        LOGD("get tx flow:%lld",tx);
        break;
      }
      num_idx++;
    }
    last_data = flow_data[i];
  }
  return true;
}
uint64_t NetIfInfo::GetCurMs() {
  struct timespec tp;
  clock_gettime(CLOCK_MONOTONIC, &tp);
  return (((uint64_t)tp.tv_sec * 1000) + ((uint64_t)tp.tv_nsec / (1000 * 1000)));
}
}
