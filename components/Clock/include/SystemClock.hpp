/**
 * @file SystemClock.hpp
 * @author Zach Hannum
 * @brief
 *
 * @copyright Copyright (c) 2022
 */

#ifndef SYSTEM_CLOCK_H
#define SYSTEM_CLOCK_H

#include <time.h>

namespace Clocks {
class SystemClock {
 public:
  SystemClock();
  ~SystemClock() = default;

  /**
   * @brief Get the Local Time
   *
   * @return time_t Time object adjusted for local timezone
   */
  time_t GetLocalTime();

  /**
   * @brief Set the local timezone
   *
   * @param aTzString local timezone string
   */
  void SetTz(const char* aTzString);

  /**
   * @brief Initialize the System Clock
   *
   * This function should be called if the System Clock has not been synced with
   * the SNTP server
   *
   */
  void Initialize();

  /**
   * @brief Checks if System Clock is initialized
   *
   * @return true
   * @return false
   */
  bool IsTimeSet();

 private:
  void InitializeSntp();
  bool IsTimeInitialized();
  bool mIsTimeSet;
};
}  // namespace Clocks

#endif  // SYSTEM_CLOCK_H