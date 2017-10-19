#ifndef BINOMIAL_HEAP_H
#define BINOMIAL_HEAP_H

#include "IPriorityQueue.h"
#include <list>

DECLARE_PTR(BinomTreeNode)
DECLARE_PTR(BinomialHeap)

class BinomTreeNode {
public:
  BinomTreeNode(int key);
  void Merge(BinomTreeNodePtr root);
  int Degree() const;
  BinomialHeapPtr RemoveRoot();
  int Key() const;
  std::string ToString() const;
  BinomTreeNodePtr Find(int key);

private:
  int key_;
  BinomTreeNodePtr left_child_;
  BinomTreeNodePtr right_sibling_;
  std::weak_ptr<BinomTreeNode> parent_;

  friend class BinomialHeap;
};

class BinomialHeap : public IPriorityQueue {
public:
  BinomialHeap();

  virtual void Insert(int key);

  virtual bool ExtractMin(int& key);

  virtual bool Peek(int& key) const;

  virtual bool Delete(int key);

  virtual bool DecreaseKey(int key, int new_key);

  virtual bool Merge(IPriorityQueuePtr other);

  virtual int  Size() const;

  virtual std::string ToString() const;

private:
  void Union(BinomialHeapPtr other);
  void Heapify();
  void InsertTree(BinomTreeNodePtr root);
  BinomTreeNodePtr GetMin() const;
  BinomTreeNodePtr Find(int key);

private:
  typedef std::list<BinomTreeNodePtr> Trees;
  Trees trees_;

  friend class BinomTreeNode;
};

#endif //BINOMIAL_HEAP_H