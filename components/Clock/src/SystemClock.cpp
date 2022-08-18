/**
 * @file SystemClock.cpp
 * @author Zach hannum
 * @brief
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "SystemClock.hpp"

#include "esp_log.h"
#include "esp_sntp.h"

namespace Clocks {
static const char* TAG = "SystemClock";
static void time_sync_notification_cb(struct timeval* tv) {
  ESP_LOGI(TAG, "Notification of a time synchronization event");
}
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 48
#endif

SystemClock::SystemClock() : mIsTimeSet(IsTimeInitialized()) {
  if (!mIsTimeSet) {
    ESP_LOGI(
        TAG,
        "Time is not initialized, SysteClock::Initialize should be called.");
  }
}

time_t SystemClock::GetLocalTime() {
  time_t now;
  time(&now);
  return now;
}

void SystemClock::SetTz(const char* aTzString) {
  setenv("TZ", aTzString, 1);
  tzset();
}

bool SystemClock::IsTimeSet() { return mIsTimeSet; }

bool SystemClock::IsTimeInitialized() {
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);
  // Is time set? If not, tm_year will be (1970 - 1900).
  return !(timeinfo.tm_year < (2016 - 1900));
}

void SystemClock::Initialize() {
  /**
   * NTP server address could be aquired via DHCP,
   * see following menuconfig options:
   * 'LWIP_DHCP_GET_NTP_SRV' - enable STNP over DHCP
   * 'LWIP_SNTP_DEBUG' - enable debugging messages
   *
   * NOTE: This call should be made BEFORE esp aquires IP address from DHCP,
   * otherwise NTP option would be rejected by default.
   */
#ifdef LWIP_DHCP_GET_NTP_SRV
  sntp_servermode_dhcp(1);  // accept NTP offers from DHCP server, if any
#endif

  InitializeSntp();

  int retry = 0;
  const int retry_count = 15;
  while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET &&
         ++retry < retry_count) {
    ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry,
             retry_count);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }

  mIsTimeSet = IsTimeInitialized();
}

void SystemClock::InitializeSntp() {
  ESP_LOGI(TAG, "Initializing SNTP");
  sntp_setoperatingmode(SNTP_OPMODE_POLL);

/*
 * If 'NTP over DHCP' is enabled, we set dynamic pool address
 * as a 'secondary' server. It will act as a fallback server in case that
 * address provided via NTP over DHCP is not accessible
 */
#if LWIP_DHCP_GET_NTP_SRV && SNTP_MAX_SERVERS > 1
  sntp_setservername(1, "pool.ntp.org");

#if LWIP_IPV6 && \
    SNTP_MAX_SERVERS > 2  // statically assigned IPv6 address is also possible
  ip_addr_t ip6;
  if (ipaddr_aton("2a01:3f7::1", &ip6)) {  // ipv6 ntp source "ntp.netnod.se"
    sntp_setserver(2, &ip6);
  }
#endif /* LWIP_IPV6 */

#else /* LWIP_DHCP_GET_NTP_SRV && (SNTP_MAX_SERVERS > 1) */
  // otherwise, use DNS address from a pool
  sntp_setservername(0, "time.windows.com");

  sntp_setservername(1,
                     "pool.ntp.org");  // set the secondary NTP server (will be
                                       // used only if SNTP_MAX_SERVERS > 1)
#endif

  sntp_set_time_sync_notification_cb(time_sync_notification_cb);
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
  sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
  sntp_init();

  ESP_LOGI(TAG, "List of configured NTP servers:");

  for (uint8_t i = 0; i < SNTP_MAX_SERVERS; ++i) {
    if (sntp_getservername(i)) {
      ESP_LOGI(TAG, "server %d: %s", i, sntp_getservername(i));
    } else {
      // we have either IPv4 or IPv6 address, let's print it
      char buff[INET6_ADDRSTRLEN];
      ip_addr_t const* ip = sntp_getserver(i);
      if (ipaddr_ntoa_r(ip, buff, INET6_ADDRSTRLEN) != NULL)
        ESP_LOGI(TAG, "server %d: %s", i, buff);
    }
  }
}
}  // namespace Clocks