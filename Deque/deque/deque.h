/**
 * @file
 * @brief Deque header file
 * @authors Vorotnikov Andrey
 *
 * Contains deque class
 */

#pragma once

#ifndef __DEQUE_H_INCLUDED
#define __DEQUE_H_INCLUDED

#include <exception>
#include <iostream>

#include "../allocator/allocator.h"

/**
 * @brief Template deque class.
 * @tparam T deque elements type
 *
 * Deque on doubly linked list with allocators.
 */
template <typename T>
class deque_t {
  /**
   * Friend operator<< for deque and output.
   * @tparam T1 deque elements type
   * @param[in] stream output stream
   * @param[in] deq deque to output
   * @return reference to stream
   */
  template <typename T1>
  friend std::ostream &operator<<(std::ostream &stream, deque_t<T1> const &deq);

private:
  /**
   * @brief Deque node struct.
   *
   * Doubly linked list node struct with 'T' data type.
   */
  struct node_t {
    node_t
      *next,  ///< pointer to next list element
      *prev;  ///< pointer to previos list element
    T data;   ///< data

    /**
     * Constructor by params.
     * @param[in] newData node data
     * @param[in] newPrev previous node
     * @param[in] newNext next node
     */
    node_t(T const &newData, node_t *newPrev, node_t *newNext) :
      data(newData), prev(newPrev), next(newNext) {
    }
  };
  node_t
    *begin,                              ///< list beginning
    *end;                                ///< list end

  single_allocator_t<node_t> allocator;  ///< allocator for nodes

  /**
   * Free deque node list function.
   * @param[in] begin list begin
   * @param[in] allocator list allocator
   */
  static void FreeList(node_t *begin, single_allocator_t<node_t> &allocator) {
    for (node_t *node = begin; node != nullptr; ) {
      node_t *next = node->next;
      allocator.dealloc(node);
      node = next;
    }
  }

  /**
   * Copy deque node list function.
   * @param[in] beginToCopy list to copy begin
   */
  void CopyList(node_t const *beginToCopy) {
    begin = allocator.alloc(*beginToCopy);
    node_t *prev = begin;
    for (node_t const *node = beginToCopy->next; node != nullptr; node = node->next, prev = prev->next) {
      node_t *tmp = allocator.alloc(node->data, prev, nullptr);
      prev->next = tmp;
    }
    end = prev;
    prev->next = nullptr;
  }

public:
  /**
   * Constructor with strategy function.
   * @param[in] strategy allocation strategy
   */
  deque_t(std::shared_ptr<alloc_strategy_t> const &strategy) :
    begin(nullptr), end(nullptr), allocator(strategy) {
  }

  /**
   * Copy constructor.
   * @param[in] lhs instance to copy
   */
  deque_t(deque_t const &lhs) : allocator(lhs.allocator) {
    if (lhs.begin == nullptr) {
      begin = nullptr;
      end = nullptr;
      return;
    }
    CopyList(lhs.begin);
  }

  /**
   * Copy constructor with strategy function.
   * @param[in] lhs instance to copy
   * @param[in] strategy allocation strategy
   */
  deque_t(deque_t const &lhs, std::shared_ptr<alloc_strategy_t> const &strategy) :
    allocator(strategy) {
    if (lhs.begin == nullptr) {
      begin = nullptr;
      end = nullptr;
      return;
    }
    CopyList(lhs.begin);
  }

  /**
   * Move constructor.
   * @param[in] rhs instance to copy
   */
  deque_t(deque_t &&rhs) :
    begin(rhs.begin), end(rhs.end), allocator(std::move(rhs.allocator)) {
    rhs.begin = nullptr;
    rhs.end = nullptr;
  }

  /**
   * Copy operator =.
   * @param[in] lhs instance to copy
   */
  void operator=(deque_t const &lhs) {
    FreeList(begin, allocator);
    allocator = lhs.allocator;
    if (lhs.begin == nullptr) {
      begin = nullptr;
      end = nullptr;
      return;
    }
    CopyList(lhs.begin);
  }

  /**
   * Move operator =.
   * @param[in] rhs rValue instance to copy
   */
  void operator=(deque_t &&rhs) {
    FreeList(begin, allocator);
    allocator = std::move(rhs.allocator);
    begin = rhs.begin;
    end = rhs.end;
    rhs.end = nullptr;
    rhs.begin = nullptr;
  }

  /**
   * Destructor.
   */
  ~deque_t(void) {
    FreeList(begin, allocator);
  }

  /**
   * Push element back function.
   * @param[in] data data to push
   */
  void PushBack(T const &data) {
    end = allocator.alloc(data, end, nullptr);
    if (begin == nullptr)
      begin = end;
    else
      end->prev->next = end;
  }

  /**
   * Push element front function.
   * @param[in] data data to push
   */
  void PushFront(T const &data) {
    begin = allocator.alloc(data, nullptr, begin);
    if (end == nullptr)
      end = begin;
    else
      begin->next->prev = begin;
  }

  /**
   * Pop element back function.
   * @return poped element
   */
  T PopBack(void) {
    if (end == nullptr)
      throw std::exception("Empty list");
    T data = std::move(end->data);
    node_t *tmp = end->prev;
    allocator.dealloc(end);
    end = tmp;
    if (tmp == nullptr)
      begin = nullptr;
    else
      end->next = nullptr;
    return data;
  }

  /**
   * Pop element front function.
   * @return poped element
   */
  T PopFront(void) {
    if (begin == nullptr)
      throw std::exception("Empty list");
    T data = std::move(begin->data);
    node_t *tmp = begin->next;
    allocator.dealloc(begin);
    begin = tmp;
    if (tmp == nullptr)
      end = nullptr;
    else
      begin->prev = nullptr;
    return data;
  }

  /**
   * Is empty check function.
   * @return true if deque is empty, false - otherwise
   */
  bool IsEmpty(void) const {
    return begin == nullptr;
  }

  /**
   * Clear deque function.
   */
  void Clear(void) {
    FreeList(begin, allocator);
    begin = nullptr;
    end = nullptr;
  }

  /**
   * Change allocator strategy function.
   * @param[in] strategy allocation strategy for deque
   */
  void ChangeAllocator(std::shared_ptr<alloc_strategy_t> const &strategy) {
    auto prevAllocator = allocator;
    node_t
      *prevBegin = begin,
      *prevEnd = end;
    allocator = single_allocator_t<node_t>(strategy);
    CopyList(begin);
    FreeList(begin, prevAllocator);
  }
};

/**
 * Operator<< for deque and output stream.
 * @tparam T deque elements type
 * @param[in] stream output stream
 * @param[in] deq deque to output
 * @return reference to stream
 */
template <typename T>
std::ostream &operator<<(std::ostream &stream, deque_t<T> const &deq) {
  for (deque_t<T>::node_t *node = deq.begin; node != nullptr; node = node->next)
    std::cout << node->data << ", ";
  return stream;
}

#endif /* __DEQUE_H_INCLUDED */
