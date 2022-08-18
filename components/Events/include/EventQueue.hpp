#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <utility>

#include "Event.hpp"

namespace Events {
typedef std::pair<std::unique_ptr<Event>, std::function<void(Event &)>>
    EventHandler;
class EventQueue {
 public:
  EventQueue() = default;
  ~EventQueue() = default;

  EventQueue(const EventQueue &) = delete;
  EventQueue &operator=(const EventQueue &) = delete;

  size_t Size() const;
  void Pop();
  void Push(std::unique_ptr<Event> aEvent,
            std::function<void(Event &)> aCallback);

 private:
  std::queue<EventHandler> mQueue;
  mutable std::mutex mMutex;
};
}  // namespace Events

#endif  // EVENT_QUEUE_H