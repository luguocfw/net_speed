#ifndef __NETIF_INFO_H__
#define __NETIF_INFO_H__
#include <stdint.h>
#include <string>
#include <stdio.h>


#define LOGE(fmt,...) 			do { printf("[error]"); printf(fmt,##__VA_ARGS__); printf("\n");} while(0);
//#define LOGI(fmt,...) 			do { printf("[info]"); printf(fmt,##__VA_ARGS__); printf("\n");} while(0);
#define LOGI(fmt,...) 			
//#define LOGD(fmt,...) 			do { printf("[debug]"); printf(fmt,##__VA_ARGS__); printf("\n");} while(0);
#define LOGD(fmt,...) 		


namespace ifm {

typedef struct {
  char          dev_name_[128];
  bool          is_connect_;
  int64_t       flow_;
  int64_t       flow_time_;
  char          mac_addr_[256];
}NetDevData;

class NetIfInfo {
public:
  NetIfInfo(const std::string &if_name);
  ~NetIfInfo();
public:
  std::string GetIfName();
  bool GetTxFlow(uint64_t &flow, uint64_t &time_ms);
  bool GetRxFlow(uint64_t &flow, uint64_t &time_ms);
  uint64_t GetTxSpeed();
  uint64_t GetRxSpeed();
private:
  bool GetCurFlow(uint64_t &tx, uint64_t &rx,uint64_t &cur_time);
  uint64_t GetCurMs();
  std::string     m_ifname;
  uint64_t        m_tx_flow;
  uint64_t        m_tx_flow_base;
  uint64_t        m_tx_flow_base_time;
  uint64_t        m_rx_flow;
  uint64_t        m_rx_flow_base;
  uint64_t        m_rx_flow_base_time;

  int64_t         m_speed_last_tx_flow;
  uint64_t        m_speed_last_tx_flow_time;
  uint64_t        m_speed_tx_last;
  int64_t         m_speed_last_rx_flow;
  uint64_t        m_speed_last_rx_flow_time;
  uint64_t        m_speed_rx_last;
};

}
#endif
