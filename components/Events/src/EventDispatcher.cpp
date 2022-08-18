#include "EventDispatcher.hpp"

namespace Events {
EventDispatcher::EventDispatcher(EventQueue &aEventQueue)
    : mEventQueue(aEventQueue) {}

void EventDispatcher::Dispatch(std::unique_ptr<Event> aEvent) {
  std::lock_guard<std::mutex> lock(mMutex);
  const auto eventName = aEvent->GetName();
  if (mCallbacks.find(eventName) != mCallbacks.end()) {
    mEventQueue.Push(std::move(aEvent), mCallbacks[eventName]);
  }
}

void EventDispatcher::Listen(std::string aEventType,
                             std::function<void(Event &)> aEventCallback) {
  std::lock_guard<std::mutex> lock(mMutex);
  mCallbacks.insert({aEventType, aEventCallback});
}
}  // namespace Events