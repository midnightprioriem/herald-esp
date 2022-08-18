/**
 * @file EventQueue.hpp
 * @author Zach Hannum
 * @brief
 *
 * @copyright Copyright (c) 2022
 */
#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <utility>

#include "Event.hpp"

namespace Events {
typedef std::pair<std::unique_ptr<Event>,
                  std::vector<std::function<void(Event &)>> &>
    DispatchEvent;

/**
 * @brief Thread-safe Event Queue for invoking Events
 *
 */
class EventQueue {
 public:
  EventQueue() = default;
  ~EventQueue() = default;

  EventQueue(const EventQueue &) = delete;
  EventQueue &operator=(const EventQueue &) = delete;

  /**
   * @brief Returns the size of the Event Queue
   *
   * @return size_t
   */
  size_t Size() const;

  /**
   * @brief Pops and invokes the next event from the Event Queue
   *
   * This function removes the next event from the Event Queue and invokes its
   * callback function
   */
  void Pop();

  /**
   * @brief Adds an Event and Callback pair to the Event Queue
   *
   * This function is used by the Event Dispatcher to add triggered events to
   * the Event Queue.
   *
   * @param aEvent Event that was dispatched.
   * @param aCallbacks List of callbacks associated with the Event.
   */
  void Push(std::unique_ptr<Event> aEvent,
            std::vector<std::function<void(Event &)>> &aCallbacks);

 private:
  std::queue<DispatchEvent> mQueue;
  mutable std::mutex mMutex;
};
}  // namespace Events

#endif  // EVENT_QUEUE_H