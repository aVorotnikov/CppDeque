/**
 * @file
 * @brief Very stupid allocation strategy class header file
 * @authors Vorotnikov Andrey
 *
 * Contains class of strategy that use heap memory with simple malloc
 */

#pragma once

#ifndef __STUPID_STRATEGY_H_INCLUDED
#define __STUPID_STRATEGY_H_INCLUDED

#include <list>

#include "alloc_strategy.h"

/**
 * @brief Very stupid allocation strategy class.
 *
 * Strategy use heap memory with simple malloc.
 * Contains list of allocated blocks to control leaks.
 */
class stupid_strategy_t : public alloc_strategy_t {
private:
  std::list<void *> allocEls;  ///< list of allocated elements

public:
  /**
   * Allocation with memory block size function.
   * @param[in] numOfBytes number of bytes to push
   * @return pointer to allocated memory
   */
  void *alloc(size_t numOfBytes) override final {
    void *el = malloc(numOfBytes);
    allocEls.push_back(el);
    return el;
  }

  /**
   * Dealocation with pointer function.
   * @param[in] ptr pointer to block
   */
  void dealloc(void *ptr) override final {
    auto el = std::find(allocEls.cbegin(), allocEls.cend(), ptr);
    if (el != allocEls.cend()) {
      free(*el);
      allocEls.erase(el);
    }
  }

  /**
   * Destructor.
   */
  ~stupid_strategy_t(void) {
    for (auto el : allocEls)
      free(el);
  }
};

#endif /* __STUPID_STRATEGY_H_INCLUDED */
