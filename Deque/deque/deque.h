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

    /**
     * Constructor by params. Data with move.
     * @param[in] newData node data
     * @param[in] newPrev previous node
     * @param[in] newNext next node
     */
    node_t(T &&newData, node_t *newPrev, node_t *newNext) :
      data(std::move(newData)), prev(newPrev), next(newNext) {
    }
  };
  node_t
    *start,                              ///< list beginning
    *tail;                               ///< list end

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
    if (beginToCopy == nullptr) {
      start = nullptr;
      tail = nullptr;
      return;
    }
    start = allocator.alloc(*beginToCopy);
    node_t *prev = start;
    for (node_t const *node = beginToCopy->next; node != nullptr; node = node->next, prev = prev->next) {
      node_t *tmp = allocator.alloc(node->data, prev, nullptr);
      prev->next = tmp;
    }
    tail = prev;
    prev->next = nullptr;
  }

  /**
   * @brief Deque iterator class.
   *
   * Class for deque iteration with data modification ability.
   */
  class iterator_t {
  private:
    node_t *node;  ///< current node
  public:
    /**
     * Constructor by node.
     * @param[in] n node for iterator
     */
    iterator_t(node_t *n) : node(n) {
    }

    /**
     * Deleted default constructor.
     */
    iterator_t(void) = delete;

    /**
     * Default copy constructor.
     * @param[in] lhs iterator to copy
     */
    iterator_t(iterator_t const &lhs) = default;

    /**
     * Default move constructor.
     * @param[in] rhs iterator to move
     */
    iterator_t(iterator_t &&rhs) = default;

    /**
     * Equality operator.
     * @param[in] lhs iterator to compare
     */
    bool operator==(iterator_t const &lhs) {
      return node == lhs.node;
    }

    /**
     * Inequality operator.
     * @param[in] lhs iterator to compare
     */
    bool operator!=(iterator_t const &lhs) {
      return node != lhs.node;
    }

    /**
     * Operator * to provide pointer semantics.
     * @return data pointer
     */
    T &operator*(void) {
      if (node == nullptr)
        throw std::exception("Try to use end iterator");
      return node->data;
    }

    /**
     * Prefix increment.
     * @return current iterator value
     */
    iterator_t &operator++(void) {
      if (node == nullptr)
        throw std::exception("Try to use end iterator");
      node = node->next;
      return *this;
    }

    /**
     * Postfix increment.
     * @param[in] unusedInteger unused argument
     * @return previous iterator value
     */
    iterator_t &operator++(int unusedInteger) {
      if (node == nullptr)
        throw std::exception("Try to use end iterator");
      iterator_t tmp{*this};
      node = node->next;
      return tmp;
    }
  };

public:
  /**
   * Constructor with strategy function.
   * @param[in] strategy allocation strategy
   */
  deque_t(std::shared_ptr<alloc_strategy_t> const &strategy) :
    start(nullptr), tail(nullptr), allocator(strategy) {
  }

  /**
   * Copy constructor.
   * @param[in] lhs instance to copy
   */
  deque_t(deque_t const &lhs) : allocator(lhs.allocator) {
    if (lhs.start == nullptr) {
      start = nullptr;
      tail = nullptr;
      return;
    }
    CopyList(lhs.start);
  }

  /**
   * Copy constructor with strategy function.
   * @param[in] lhs instance to copy
   * @param[in] strategy allocation strategy
   */
  deque_t(deque_t const &lhs, std::shared_ptr<alloc_strategy_t> const &strategy) :
    allocator(strategy) {
    CopyList(lhs.start);
  }

  /**
   * Move constructor.
   * @param[in] rhs instance to copy
   */
  deque_t(deque_t &&rhs) :
    start(rhs.start), tail(rhs.tail), allocator(std::move(rhs.allocator)) {
    rhs.start = nullptr;
    rhs.tail = nullptr;
  }

  /**
   * Copy operator =.
   * @param[in] lhs instance to copy
   */
  void operator=(deque_t const &lhs) {
    FreeList(start, allocator);
    allocator = lhs.allocator;
    CopyList(lhs.start);
  }

  /**
   * Copy with another allocator.
   * @param[in] lhs instance to copy
   * @param[in] strategy allocation strategy for deque
   */
  void Copy(deque_t const &lhs, std::shared_ptr<alloc_strategy_t> const &strategy) {
    FreeList(start, allocator);
    allocator = single_allocator_t<node_t>(strategy);
    CopyList(lhs.start);
  }

  /**
   * Move operator =.
   * @param[in] rhs rValue instance to copy
   */
  void operator=(deque_t &&rhs) {
    FreeList(start, allocator);
    allocator = std::move(rhs.allocator);
    start = rhs.start;
    tail = rhs.tail;
    rhs.tail = nullptr;
    rhs.start = nullptr;
  }

  /**
   * Destructor.
   */
  ~deque_t(void) {
    FreeList(start, allocator);
  }

  /**
   * Push element back function.
   * @param[in] data data to push
   */
  void PushBack(T const &data) {
    tail = allocator.alloc(data, tail, nullptr);
    if (start == nullptr)
      start = tail;
    else
      tail->prev->next = tail;
  }

  /**
   * Push element back with move function.
   * @param[in] data data to push
   */
  void PushBack(T &&data) {
    tail = allocator.alloc(std::move(data), tail, nullptr);
    if (start == nullptr)
      start = tail;
    else
      tail->prev->next = tail;
  }

  /**
   * Push element front function.
   * @param[in] data data to push
   */
  void PushFront(T const &data) {
    start = allocator.alloc(data, nullptr, start);
    if (tail == nullptr)
      tail = start;
    else
      start->next->prev = start;
  }

  /**
   * Push element front with move function.
   * @param[in] data data to push
   */
  void PushFront(T &&data) {
    start = allocator.alloc(std::move(data), nullptr, start);
    if (tail == nullptr)
      tail = start;
    else
      start->next->prev = start;
  }

  /**
   * Pop element back function.
   * @return poped element
   */
  T PopBack(void) {
    if (tail == nullptr)
      throw std::exception("Empty list");
    T data = std::move(tail->data);
    node_t *tmp = tail->prev;
    allocator.dealloc(tail);
    tail = tmp;
    if (tmp == nullptr)
      start = nullptr;
    else
      tail->next = nullptr;
    return data;
  }

  /**
   * Pop element front function.
   * @return poped element
   */
  T PopFront(void) {
    if (start == nullptr)
      throw std::exception("Empty list");
    T data = std::move(start->data);
    node_t *tmp = start->next;
    allocator.dealloc(start);
    start = tmp;
    if (tmp == nullptr)
      tail = nullptr;
    else
      start->prev = nullptr;
    return data;
  }

  /**
   * Is empty check function.
   * @return true if deque is empty, false - otherwise
   */
  bool IsEmpty(void) const {
    return start == nullptr;
  }

  /**
   * Clear deque function.
   */
  void Clear(void) {
    FreeList(start, allocator);
    start = nullptr;
    tail = nullptr;
  }

  /**
   * Change allocator strategy function.
   * @param[in] strategy allocation strategy for deque
   */
  void ChangeAllocator(std::shared_ptr<alloc_strategy_t> const &strategy) {
    auto prevAllocator = allocator;
    node_t
      *prevBegin = start,
      *prevEnd = tail;
    allocator = single_allocator_t<node_t>(strategy);
    if (start == nullptr) {
      start = nullptr;
      tail = nullptr;
      return;
    }
    start = allocator.alloc(std::move(*start));
    node_t *prev = start;
    for (node_t const *node = start->next; node != nullptr; node = node->next, prev = prev->next) {
      node_t *tmp = allocator.alloc(std::move(node->data), prev, nullptr);
      prev->next = tmp;
    }
    tail = prev;
    prev->next = nullptr;
    FreeList(start, prevAllocator);
  }

  /**
   * Get begin iterator function.
   * @return begin iterator
   */
  iterator_t begin(void) {
    return iterator_t(start);
  }

  /**
   * Get end iterator function.
   * @return end iterator
   */
  iterator_t end(void) {
    return iterator_t(nullptr);
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
  for (deque_t<T>::node_t *node = deq.start; node != nullptr; node = node->next)
    std::cout << node->data << ", ";
  return stream;
}

#endif /* __DEQUE_H_INCLUDED */
