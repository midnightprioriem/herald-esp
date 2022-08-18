/**
 * @file EventDispatcher.cpp
 * @author Zach Hannum
 * @brief 
 * 
 * @copyright Copyright (c) 2022
 */
#include "EventDispatcher.hpp"

namespace Events {
EventDispatcher::EventDispatcher(EventQueue &aEventQueue)
    : mEventQueue(aEventQueue) {}

void EventDispatcher::Dispatch(std::unique_ptr<Event> aEvent) {
  std::lock_guard<std::mutex> lock(mMutex);
  const auto eventName = aEvent->GetId();
  if (mCallbacks.find(eventName) != mCallbacks.end()) {
    mEventQueue.Push(std::move(aEvent), mCallbacks[eventName]);
  }
}

void EventDispatcher::Listen(std::string aEventType,
                             std::function<void(Event &)> aEventCallback) {
  std::lock_guard<std::mutex> lock(mMutex);
  mCallbacks[aEventType].push_back(aEventCallback);
}
}  // namespace Events