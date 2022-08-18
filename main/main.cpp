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

#include "ClockEvent.hpp"
#include "Event.hpp"
#include "EventDispatcher.hpp"
#include "EventQueue.hpp"
#include "SystemClock.hpp"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

static const char *TAG = "app_main";

void time_sync_notification_cb(struct timeval *tv) {
  ESP_LOGI(TAG, "Notification of a time synchronization event");
}

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 48
#endif

void app_clock(Events::EventDispatcher &aEventDispatcher) {
  using namespace Clocks;
  SystemClock clock;
  clock.SetTz("EST5EDT");
  if (!clock.IsTimeSet()) {
    clock.Initialize();
  }
  while (1) {
    if (clock.IsTimeSet()) {
      aEventDispatcher.Dispatch(
          std::make_unique<Clocks::ClockEvent>(clock.GetLocalTime()));
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void app_log(Events::EventDispatcher &aEventDispatcher) {
  aEventDispatcher.Listen(Clocks::ClockEvent::Id, [](Events::Event &aEvent) {
    try {
      auto event = dynamic_cast<Clocks::ClockEvent &>(aEvent);
      auto now = event.GetTime();
      struct tm timeinfo;
      char strftime_buf[64];
      localtime_r(&now, &timeinfo);
      strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
      ESP_LOGI(TAG, "The time is %s.", strftime_buf);
    } catch (const std::bad_cast &e) {
      ESP_LOGI(TAG, "Unexpected event type %s", e.what());
    }
  });
}

extern "C" void app_main(void) {
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  /* This helper function configures Wi-Fi or Ethernet, as selected in
   * menuconfig. Read "Establishing Wi-Fi or Ethernet Connection" section in
   * examples/protocols/README.md for more information about this function.
   */
  ESP_ERROR_CHECK(example_connect());

  Events::EventQueue eventQueue;
  Events::EventDispatcher eventDispatcher(eventQueue);

  auto clock_thread = std::thread(app_clock, std::ref(eventDispatcher));
  auto log_thread = std::thread(app_log, std::ref(eventDispatcher));

  // Process event queue
  while (true) {
    eventQueue.Pop();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
