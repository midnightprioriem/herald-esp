/* pthread/std::thread example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <iostream>
#include <memory>
#include <thread>

#include "Event.hpp"
#include "EventDispatcher.hpp"
#include "EventQueue.hpp"
#include "esp_attr.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

static const char *TAG = "example";
static void obtain_time(void);
static void initialize_sntp(void);

void time_sync_notification_cb(struct timeval *tv) {
  ESP_LOGI(TAG, "Notification of a time synchronization event");
}

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 48
#endif

class TimeEvent : public Events::Event {
 public:
  static auto constexpr Name = "TimeEvent";
  TimeEvent(time_t aTime) : Events::Event(Name), mTime(aTime) {}
  time_t GetData() { return mTime; }

 private:
  time_t mTime;
};

void get_time_thread(Events::EventDispatcher &aEventDispatcher) {
  while (1) {
    time_t now;
    time(&now);
    ESP_LOGI(TAG, "Getting time from time thread.");
    aEventDispatcher.Dispatch(std::make_unique<TimeEvent>(now));
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void print_time_thread(Events::EventDispatcher &aEventDispatcher) {
  aEventDispatcher.Listen(TimeEvent::Name, [](Events::Event &aEvent) {
    TimeEvent &event = static_cast<TimeEvent &>(aEvent);
    auto now = event.GetData();
    struct tm timeinfo;
    char strftime_buf[64];
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The time is %s.", strftime_buf);
  });
}

extern "C" void app_main(void) {
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);
  // Is time set? If not, tm_year will be (1970 - 1900).
  if (timeinfo.tm_year < (2016 - 1900)) {
    ESP_LOGI(
        TAG,
        "Time is not set yet. Connecting to WiFi and getting time over NTP.");
    obtain_time();
    // update 'now' variable with current time
    time(&now);
  }

  char strftime_buf[64];

  // Set timezone to Eastern Standard Time and print local time
  setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
  tzset();
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
  ESP_LOGI(TAG, "The current date/time in New York is: %s", strftime_buf);

  Events::EventQueue eventQueue;
  Events::EventDispatcher eventDispatcher(eventQueue);

  auto get_time_t = std::thread(get_time_thread, std::ref(eventDispatcher));
  auto print_time_t = std::thread(print_time_thread, std::ref(eventDispatcher));

  // Process events
  while (true) {
    eventQueue.Pop();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

static void obtain_time(void) {
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

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

  /* This helper function configures Wi-Fi or Ethernet, as selected in
   * menuconfig. Read "Establishing Wi-Fi or Ethernet Connection" section in
   * examples/protocols/README.md for more information about this function.
   */
  ESP_ERROR_CHECK(example_connect());

  initialize_sntp();

  // wait for time to be set
  time_t now = 0;
  struct tm timeinfo = {0};
  int retry = 0;
  const int retry_count = 15;
  while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET &&
         ++retry < retry_count) {
    ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry,
             retry_count);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
  time(&now);
  localtime_r(&now, &timeinfo);

  ESP_ERROR_CHECK(example_disconnect());
}

static void initialize_sntp(void) {
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
      ip_addr_t const *ip = sntp_getserver(i);
      if (ipaddr_ntoa_r(ip, buff, INET6_ADDRSTRLEN) != NULL)
        ESP_LOGI(TAG, "server %d: %s", i, buff);
    }
  }
}
