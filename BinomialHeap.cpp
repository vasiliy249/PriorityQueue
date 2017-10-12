#include "BinomialHeap.h"
#include <limits>
#include <sstream>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////////////
BinomTreeNode::BinomTreeNode(int key)
  : key_(key), left_child_(NULL), right_sibling_(NULL) {}

void BinomTreeNode::Merge(BinomTreeNodePtr root) {
  root->right_sibling_ = this->left_child_;
  left_child_ = root;
}

int BinomTreeNode::Degree() const {
  int ret = 0;
  BinomTreeNodePtr child = left_child_;
  while (child) {
    child = child->right_sibling_;
    ret++;
  }
  return ret;
}

BinomialHeapPtr BinomTreeNode::RemoveRoot() {
  BinomialHeapPtr ret(new BinomialHeap);
  BinomTreeNodePtr child = left_child_;
  while (child) {
    //iterate via children and push them to new binomial heap in reverse order
    ret->trees_.push_front(child);
    BinomTreeNodePtr next_sibling = child->right_sibling_;
    // remove pointer to sibling, because this node became root
    ret->trees_.front()->right_sibling_ = NULL; 
    child = next_sibling;
  }
  return ret;
}

int BinomTreeNode::Key() const {
  return key_;
}

std::string BinomTreeNode::ToString() const {
  std::stringstream ss;
  if (left_child_) {
    ss << left_child_->key_ << " " << left_child_->ToString();
  }
  BinomTreeNodePtr sibling = right_sibling_;
  while(sibling) {
    ss << sibling->key_ << " ";
    if (sibling->left_child_) {
      ss << sibling->left_child_->key_ << " " << sibling->left_child_->ToString();
    }
    sibling = sibling->right_sibling_;
  }
  return ss.str();
}

BinomTreeNodePtr BinomTreeNode::Find(int key) {
  BinomTreeNodePtr ret;
  if (left_child_) {
    if (left_child_->Key() == key) {
      return left_child_;
    }
    ret = left_child_->Find(key);
    if (ret) {
      return ret;
    }
  }
  BinomTreeNodePtr sibling = right_sibling_;
  while (sibling) {
    if (sibling->Key() == key) {
      return sibling;
    }
    if (sibling->left_child_) {
      if (sibling->left_child_->Key() == key) {
        return sibling->left_child_;
      }
      ret = sibling->left_child_->Find(key);
      if (ret) {
        return ret;
      }
    }
    sibling = sibling->right_sibling_;
  }
  return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
BinomialHeap::BinomialHeap() {}

void BinomialHeap::Insert(int key) {
  BinomTreeNodePtr tmp(new BinomTreeNode(key));
  InsertTree(tmp);
}

bool BinomialHeap::ExtractMin(int& key) {
  BinomTreeNodePtr min_node = GetMin();
  if (!min_node) {
    return false;
  }
  key = min_node->Key();
  trees_.remove(min_node);
  Union(min_node->RemoveRoot());
  Heapify();
  return true;
}

bool BinomialHeap::Peek(int& key) const {
  BinomTreeNodePtr min_node = GetMin();
  if (min_node) {
    key = min_node->Key();
    return true;
  } else {
    return false;
  }
}

bool BinomialHeap::Delete(int key) {
  if (!DecreaseKey(key, std::numeric_limits<int>::min())) {
    return false;
  }
  int min;
  return ExtractMin(min);
}

bool BinomialHeap::DecreaseKey(int key, int new_key) {
  BinomTreeNodePtr node = Find(key);
  if (!node) {
    return false;
  }

  node->key_ = new_key;
  BinomTreeNodePtr parent(node->parent_);
  while (parent && parent->key_ > node->key_) {
    std::swap(parent->key_, node->key_);
    node = parent;
    if (parent->parent_.lock()) {
      parent = BinomTreeNodePtr(parent->parent_);
    } else {
      break;
    }
  }
  return true;
}

void BinomialHeap::Merge(IPriorityQueuePtr other) {
  Union(std::dynamic_pointer_cast<BinomialHeap>(other));
  Heapify();
}

int BinomialHeap::Size() const {
  Trees::const_iterator it = trees_.begin();
  int ret = 0;
  for (; it != trees_.end(); ++it) {
    ret += static_cast<int>(pow(2, (*it)->Degree()));
  }
  return ret;
}

std::string BinomialHeap::ToString() const {
  std::stringstream ret;
  ret << "--------------------------------\n";
  ret << "Heap size: " << Size() << "\n";
  ret << "Tree count: " << trees_.size() << "\n";
  ret << "Trees:\n\n";
  Trees::const_iterator it = trees_.begin();
  for (; it != trees_.end(); ++it) {
    ret << "Degree: " << (*it)->Degree() << ", elements:\n";
    ret << (*it)->Key() << " " << (*it)->ToString();
    ret << "\n\n";
  }
  return ret.str();
}

// ordered union list with other trees
void BinomialHeap::Union(BinomialHeapPtr other) {
  if (trees_.empty()) {
    // this heap is empty, just take the other heap trees
    trees_ = other->trees_;
    return;
  }
  Trees::const_iterator my_it = trees_.begin();
  Trees::const_iterator other_it = other->trees_.begin();
  for (; my_it != trees_.end(); ++my_it) {
    while (other_it != other->trees_.end()) {
      if ((*other_it)->Degree() <= (*my_it)->Degree()) {
        // if degree of other's tree is less than this, insert other's tree before this tree
        // and erase it from other trees
        trees_.insert(my_it, (*other_it));
        other->trees_.erase(other_it++);
      } else {
        break;
      }
    }
  }
  if (!other->trees_.empty()) {
    // if trees in other heap is not empty, just insert them to the end
    trees_.insert(trees_.end(), other->trees_.begin(), other->trees_.end());
  }
}

// heapify a heap to exclude trees with the same degrees
void BinomialHeap::Heapify() {
  if (trees_.size() <= 1) {
    // nothing to heapify
    return;
  }
  Trees::const_iterator prev, curr, next;
  prev = curr = next = trees_.begin();
  curr = (++next)++;

  while (curr != trees_.end()) {
    if ((*prev)->Degree() < (*curr)->Degree()) {
      // all is OK, just move cursors
      prev++;
      curr++;
      if (next != trees_.end()) {
        next++;
      }
    } else if ((*prev)->Degree() == (*curr)->Degree()) {
      if (next != trees_.end() && (*prev)->Degree() == (*next)->Degree()) {
        // three trees have the same degree, merge second with third, erase third and move cursors
        Trees::const_iterator merge_to, merge_from;
        if ((*curr)->Key() <= (*next)->Key()) {
          merge_to = curr;
          merge_from = next;
        } else {
          merge_to = next;
          merge_from = curr;
        }
        (*merge_to)->Merge(*merge_from);
        (*merge_to)->left_child_->parent_ = (*merge_to);
        trees_.erase(merge_from);
        prev = merge_to;
        curr = ++merge_to;
        if (merge_to != trees_.end()) {
          next = ++merge_to;
        }
      } else {
        // only two trees have the same degree, merge first with second, erase second and move cursors
        Trees::const_iterator merge_to, merge_from;
        if ((*prev)->Key() <= (*curr)->Key()) {
          merge_to = prev;
          merge_from = curr;
        } else {
          merge_to = curr;
          merge_from = prev;
        }
        (*merge_to)->Merge(*merge_from);
        (*merge_to)->left_child_->parent_ = (*merge_to);
        trees_.erase(merge_from);
        prev = merge_to;
        if (merge_to != trees_.end()) {
          curr = ++merge_to;
          if (merge_to != trees_.end()) {
            next = ++merge_to;
          }
        }
      }
    }
  }
}

void BinomialHeap::InsertTree(BinomTreeNodePtr root) {
  BinomialHeapPtr temp_heap(new BinomialHeap());
  temp_heap->trees_.push_back(root);
  Union(temp_heap);
  Heapify();
}

BinomTreeNodePtr BinomialHeap::GetMin() const {
  if (trees_.empty()) {
    return BinomTreeNodePtr();
  }

  Trees::const_iterator it = trees_.begin();
  BinomTreeNodePtr min_node = *it;
  for (; it != trees_.end(); ++it) {
    int it_key = (*it)->Key();
    if (it_key < min_node->Key()) {
      min_node = *it;
    }
  }
  return min_node;
}

BinomTreeNodePtr BinomialHeap::Find(int key) {
  BinomTreeNodePtr ret;
  Trees::const_iterator it = trees_.begin();
  for (; it != trees_.end(); ++it) {
    if ((*it)->Key() == key) {
      return (*it);
    } else if((*it)->Key() < key) {
      ret = (*it)->Find(key);
    } else {
      continue;
    }
    if (ret) {
      return ret;
    }
  }
  return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////

IPriorityQueuePtr CreatePriorityQueue() {
  IPriorityQueuePtr ptr(new BinomialHeap);
  return ptr;
}
