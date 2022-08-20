/**
 * @file ClockDisplay.hpp
 * @author Zach Hannum
 * @brief
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CLOCK_DISPLAY_H
#define CLOCK_DISPLAY_H

#include <time.h>

namespace Clocks {
class ClockDisplay {
 public:
  /**
   * @brief Sets the Clock Display time
   *
   * @param aTime the time to set the clock to
   */
  virtual void SetTime(time_t aTime) = 0;

  /**
   * @brief Clears the display
   *
   */
  virtual void ClearDisplay() = 0;
};
}  // namespace Clocks

#endif