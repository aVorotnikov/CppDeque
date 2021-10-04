/**
 * @file
 * @brief Allocation strategy base header file
 * @authors Vorotnikov Andrey
 *
 * Contains allocation strategy base virtual class
 */

#pragma once

#ifndef __ALLOC_STRATEGY_H_INCLUDED
#define __ALLOC_STRATEGY_H_INCLUDED

/**
 * @brief Allocation strategy base virtual class.
 *
 * Base class for allocation strategies.
 */
class alloc_strategy_t {
public:
  /**
   * Allocation with memory block size function.
   * @param[in] numOfBytes number of bytes to push
   * @return pointer to allocated memory
   */
  virtual void *alloc(size_t numOfBytes) = 0;

  /**
   * Dealocation with pointer function.
   * @param[in] ptr pointer to block
   */
  virtual void dealloc(void *ptr) = 0;

  /**
   * Destructor.
   */
  virtual ~alloc_strategy_t(void) = 0 {
  }
};

#endif /* __ALLOC_STRATEGY_H_INCLUDED */
