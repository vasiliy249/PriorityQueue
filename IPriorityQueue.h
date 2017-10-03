#ifndef I_PRIORITY_QUEUE_H 
#define I_PRIORITY_QUEUE_H 

class IPriorityQueue {
public:
  virtual ~IPriorityQueue() {}

  virtual bool Insert(int key) = 0;

  virtual bool ExtractMin(int& key) = 0;

  virtual bool Peek(int& key) const = 0;

  virtual bool Delete(int& key) = 0;
};

IPriorityQueue* CreatePriorityQueue();

#endif // I_PRIORITY_QUEUE_H
