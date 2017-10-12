#include "IPriorityQueue.h"
#include <iostream>

int main() {
  IPriorityQueuePtr prio_queue = CreatePriorityQueue();

  prio_queue->Insert(1);
  prio_queue->Insert(2);
  prio_queue->Insert(3);
  prio_queue->Insert(4);
  prio_queue->Insert(5);
  prio_queue->Insert(6);
  prio_queue->Insert(7);
  prio_queue->Insert(8);
  std::cout << prio_queue->ToString();

  //int min;
  //prio_queue->ExtractMin(min);
  //std::cout << prio_queue->ToString();

  prio_queue->Delete(3);
  std::cout << prio_queue->ToString();

  system("pause");
}