/**
 * @file Event.hpp
 * @author Zach Hannum
 * @brief
 *
 * @copyright Copyright (c) 2022
 */
#ifndef EVENT_H
#define EVENT_H

#include <optional>
#include <string>

namespace Events {

/**
 * @brief Base class for Events
 *
 * Specific Event types should implement this class and provide a static Event
 * ID in the constructor. Derived Event types may implement additional member
 * functions for accessing Event data.
 *
 */
class Event {
 public:
  Event(const char *aEventId) : mEventId(aEventId) {}
  virtual ~Event() = default;

  /**
   * @brief Returns the Event ID
   *
   * Used by the Event Dispatcher to match Callbacks with Events.
   *
   * @return const char*
   */
  const char *GetId() { return mEventId; }

 private:
  const char *mEventId;
};
}  // namespace Events

#endif