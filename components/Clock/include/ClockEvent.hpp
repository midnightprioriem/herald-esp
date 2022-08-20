/**
 * @file ClockEvent.hpp
 * @author Zach Hannum
 * @brief
 *
 * @copyright Copyright (c) 2022
 */
#ifndef CLOCK_EVENT_H
#define CLOCK_EVENT_H

#include "Event.hpp"

namespace Clocks {
/**
 * @brief This Event is used to dispatch System Clock updates to other threads
 * in the system
 */
class ClockEvent : public Events::Event {
 public:
  static auto constexpr Id = "ClockEvent";
  ClockEvent(const time_t aNow) : Events::Event(Id), mNow(aNow) {}
  ~ClockEvent() = default;

  /**
   * @brief Get the Time data associated with this ClockEvent
   *
   * @return time_t
   */
   time_t GetTime() { return mNow; }

 private:
  time_t mNow;
};
}  // namespace Clocks

#endif