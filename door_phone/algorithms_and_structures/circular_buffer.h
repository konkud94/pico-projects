#pragma once

#include <stdlib.h>
/*
    TODO: Unit Test it
*/
template <class Type, size_t CAPACITY>
class CurcularBuffer {
 public:
  CurcularBuffer(bool allow_overwrite_when_full = true)
      : allow_overwrite_when_full_(allow_overwrite_when_full) {}
  bool Push(const Type& item) {
    const bool is_full = IsFull();
    if (is_full && !allow_overwrite_when_full_) {
      return false;
    }
    buffer_[head_] = item;
    if (is_full) {
      IncrementIndex(head_);
      IncrementIndex(tail_);
    } else {
      IncrementIndex(head_);
      current_size_++;
    }
    return true;
  }
  bool Pop(Type& out_item) {
    if (IsEmpty()) {
      return false;
    }
    out_item = buffer_[tail_];
    IncrementIndex(tail_);
    current_size_--;
    return true;
  }
  bool Peek(Type& out_item) {
    if (IsEmpty()) {
      return false;
    }
    out_item = buffer_[tail_];
    return true;
  }
  bool IsEmpty() const { return current_size_ == 0; }
  bool IsFull() const { return current_size_ == CAPACITY; }
  size_t GetCapacity() const { return CAPACITY; }
  size_t GetCurrentSize() const { return current_size_; }

 private:
  size_t IncrementIndex(size_t& index) {
    index++;
    if (index == CAPACITY) {
      index = 0;
    }
    return index;
  }
  const bool allow_overwrite_when_full_;
  size_t head_ = 0;
  size_t tail_ = 0;
  Type buffer_[CAPACITY];
  size_t current_size_ = 0;
};