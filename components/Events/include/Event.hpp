#ifndef EVENT_H
#define EVENT_H

#include <string>

namespace Events {
class Event {
 public:
  Event(const char *aEventName) : mEventName(aEventName) {}
  virtual ~Event() = default;

  const char *GetName() { return mEventName; }

 private:
  const char *mEventName;
};
}  // namespace Events

#endif