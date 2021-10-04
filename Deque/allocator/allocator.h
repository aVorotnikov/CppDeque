/**
 * @file
 * @brief Single instances allocator header file
 * @authors Vorotnikov Andrey
 *
 * Contains single instances allocator class
 */

#pragma once

#ifndef __ALLOCATOR_H_INCLUDED
#define __ALLOCATOR_H_INCLUDED

#include "alloc_strategy.h"

/**
 * @brief Single instances allocator class.
 * @tparam T type to allocate
 *
 * Class to allocate instances of 'T' type with constructors.
 */
template <typename T>
class single_allocator_t {
private:
  std::shared_ptr<alloc_strategy_t> allocStrategy;  ///< allocation strategy used by allocator

public:
  /**
   * Deleted default constructor.
   */
  single_allocator_t(void) = delete;

  /**
   * Copy constructor.
   * @param[in] lhs instance to copy
   */
  single_allocator_t(single_allocator_t<T> const &lhs) = default;

  /**
   * Move constructor.
   * @param[in] rhs rValue reference to copy
   */
  single_allocator_t(single_allocator_t<T> &&rhs) = default;

  /**
   * Copy operator=.
   * @param[in] lhs instance to copy
   * @return reference to this instance
   */
  single_allocator_t<T>& operator=(single_allocator_t<T> const &lhs) = default;

  /**
   * Move operator=.
   * @param[in] rhs rValue reference to copy
   * @return reference to this instance
   */
  single_allocator_t<T>& operator=(single_allocator_t<T> &&rhs) = default;

  /**
   * Constructor.
   * @param strategy allocation strategy
   */
  single_allocator_t(std::shared_ptr<alloc_strategy_t> const &strategy) :
    allocStrategy(strategy) {
  }

  /**
   * Allocation T type instance function.
   * @tparam Args constructor argument types
   * @param[in] constructorArgs constructor arguments
   * @return pointer to allocated memory
   */
  template <typename... Args>
  T *alloc(Args&&... constructorArgs) {
    T* ptr = static_cast<T*>(allocStrategy->alloc(sizeof(T)));
    new (ptr) T(std::forward<Args>(constructorArgs)...);
    return ptr;
  }

  /**
   * Deallocation with pointer function.
   * @param[in] ptr pointer to block
   */
  void dealloc(T *ptr) {
    ptr->~T();
    allocStrategy->dealloc(ptr);
  }
};

#endif /* __ALLOCATOR_H_INCLUDED */
