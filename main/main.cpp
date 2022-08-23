/* pthread/std::thread example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "ClockEvent.hpp"
#include "EspI2CBus.hpp"
#include "Event.hpp"
#include "EventDispatcher.hpp"
#include "EventQueue.hpp"
#include "HT16K33ClockDisplay.hpp"
#include "SystemClock.hpp"
#include "app_server.hpp"
#include "driver/i2c.h"
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
  // TODO
}

void app_clock_display(Events::EventDispatcher &aEventDispatcher) {
  static auto constexpr CLOCK_DISPLAY = "app_clock_display";
  i2c_config_t conf = {.mode = I2C_MODE_MASTER,
                       .sda_io_num = GPIO_NUM_33,
                       .scl_io_num = GPIO_NUM_32,
                       .sda_pullup_en = GPIO_PULLUP_ENABLE,
                       .scl_pullup_en = GPIO_PULLUP_ENABLE,
                       .master = {
                           400000,
                       }};
  I2C::EspI2CBus i2cBus(conf, 0x70, 0);
  Clocks::HT16K33ClockDisplay clockDisplay(i2cBus);
  aEventDispatcher.Listen(
      Clocks::ClockEvent::Id, [&clockDisplay](Events::Event &aEvent) {
        try {
          auto event = dynamic_cast<Clocks::ClockEvent &>(aEvent);
          auto now = event.GetTime();
          const auto local = localtime(&now);
          if(local->tm_hour < 7 || local->tm_hour > 21) {
            clockDisplay.SetBrightness(0x0);
          }
          else {
            clockDisplay.SetBrightness(0xF);
          }
          clockDisplay.SetTime(now);
        } catch (const std::bad_cast &e) {
          ESP_LOGI(TAG, "Unexpected event type %s", e.what());
        }
      });
  while (1) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

extern "C" void app_main(void) {
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  Events::EventQueue eventQueue;
  Events::EventDispatcher eventDispatcher(eventQueue);

  auto log_thread = std::thread(app_log, std::ref(eventDispatcher));
  auto clock_display_thread =
      std::thread(app_clock_display, std::ref(eventDispatcher));

  /* This helper function configures Wi-Fi or Ethernet, as selected in
   * menuconfig. Read "Establishing Wi-Fi or Ethernet Connection" section in
   * examples/protocols/README.md for more information about this function.
   */
  ESP_ERROR_CHECK(example_connect());

  auto clock_thread = std::thread(app_clock, std::ref(eventDispatcher));
  auto server_thread = std::thread(app_server, std::ref(eventDispatcher));

  // Process event queue
  while (true) {
    eventQueue.Pop();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
