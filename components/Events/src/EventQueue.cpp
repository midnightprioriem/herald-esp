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
    event.second(*event.first);
  }
}
void EventQueue::Push(std::unique_ptr<Event> aEvent,
                      std::function<void(Event &)> aCallback) {
  std::lock_guard<std::mutex> lock(mMutex);
  mQueue.push(std::make_pair(std::move(aEvent), aCallback));
}
}  // namespace Events