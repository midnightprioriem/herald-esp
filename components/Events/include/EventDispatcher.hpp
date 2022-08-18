/**
 * @file EventDispatcher.hpp
 * @author Zach Hannum
 * @brief 
 * 
 * @copyright Copyright (c) 2022
 */
#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "Event.hpp"
#include "EventQueue.hpp"

namespace Events {
/**
 * @brief Provides functionality for Dispatching Events to the Event Queue and
 * registering callbacks for Events
 *
 */
class EventDispatcher {
 public:
  EventDispatcher(EventQueue &aEventQueue);
  ~EventDispatcher() = default;

  /**
   * @brief Dispatches an Event to the Event Queue
   *
   * This thread-safe function adds an Event to the Event queue to be triggered.
   *
   * @param aEvent Event to be dispatched. Generally this should be a derived
   * Event that Listeners will expect for a given event type.
   */
  void Dispatch(std::unique_ptr<Event> aEvent);

  /**
   * @brief Registers a listener with the Event Queue to trigger when Events are
   * dispatched
   *
   * @param aEventId Event ID to listen for. Derived Event classes should
   * provide a static event id string that can be referenced.
   * @param aEventCallback Callback to be run when an Event of the given ID is
   * dispatched. Callers should dynamic_cast the Event to the expected derived
   * Event class.
   */
  void Listen(std::string aEventId,
              std::function<void(Event &)> aEventCallback);

 private:
  std::unordered_map<std::string, std::vector<std::function<void(Event &)>>>
      mCallbacks;
  EventQueue &mEventQueue;
  std::mutex mMutex;
};
}  // namespace Events

#endif