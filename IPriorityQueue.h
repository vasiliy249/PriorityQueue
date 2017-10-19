#ifndef I_PRIORITY_QUEUE_H
#define I_PRIORITY_QUEUE_H

#include <memory>
#include <string>

#define DECLARE_PTR(Interface) \
  class Interface;             \
  typedef std::shared_ptr<Interface> Interface ## Ptr;

DECLARE_PTR(IPriorityQueue)

class IPriorityQueue {
public:
  virtual ~IPriorityQueue() {}

  virtual void Insert(int key) = 0;

  virtual bool ExtractMin(int& key) = 0;

  virtual bool Peek(int& key) const = 0;

  virtual bool Delete(int key) = 0;

  virtual bool DecreaseKey(int key, int new_key) = 0;

  virtual bool Merge(IPriorityQueuePtr other) = 0;

  virtual int  Size() const = 0;

  virtual std::string ToString() const = 0;
};

IPriorityQueuePtr CreatePriorityQueue();

#endif // I_PRIORITY_QUEUE_H
