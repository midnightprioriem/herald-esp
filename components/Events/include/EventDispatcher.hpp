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
class EventDispatcher {
 public:
  EventDispatcher(EventQueue &aEventQueue);
  ~EventDispatcher() = default;

  void Dispatch(std::unique_ptr<Event> aEvent);
  void Listen(std::string aEventType,
              std::function<void(Event &)> aEventCallback);

 private:
  std::unordered_map<std::string, std::function<void(Event &)>> mCallbacks;
  EventQueue &mEventQueue;
  std::mutex mMutex;
};
}  // namespace Events

#endif