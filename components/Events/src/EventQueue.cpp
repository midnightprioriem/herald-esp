/**
 * @file EventQueue.cpp
 * @author Zach Hannum
 * @brief 
 * 
 * @copyright Copyright (c) 2022
 */
#include "EventQueue.hpp"

namespace Events {
size_t EventQueue::Size() const {
  std::lock_guard<std::mutex> lock(mMutex);
  return mQueue.size();
}
void EventQueue::Pop() {
  std::lock_guard<std::mutex> lock(mMutex);
  if (!mQueue.empty()) {
    const auto event = std::move(mQueue.front());
    mQueue.pop();
    const auto callbacks = event.second;
    for (const auto &callback : callbacks) {
      callback(*event.first);
    }
  }
}
void EventQueue::Push(std::unique_ptr<Event> aEvent,
                      std::vector<std::function<void(Event &)>> &aCallbacks) {
  std::lock_guard<std::mutex> lock(mMutex);
  mQueue.push(std::pair<std::unique_ptr<Event>,
                        std::vector<std::function<void(Event &)>> &>(
      std::move(aEvent), aCallbacks));
}
}  // namespace Events