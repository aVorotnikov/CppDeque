/**
 * @file
 * @brief Main program source file
 * @authors Vorotnikov Andrey
 *
 * Contains main function to start program
 */

/**
 * @mainpage Introduction
 * @author Vorotnikov Andrey
 *
 * Deque with custom allocators project.
 */

#include "deque/deque.h"
#include "allocator/stupid_strategy.h"
#include "allocator/allocator.h"

/**
 * Main program function.
 * @return code from appliction to OS
 */
int main(void) {
  deque_t<int> deq(std::make_shared<stupid_strategy_t>());
  // push demo
  std::cout << "1) " << deq << std::endl;
  deq.PushBack(3);
  deq.PushBack(4);
  std::cout << "2) " << deq << std::endl;
  deq.PushFront(2);
  deq.PushFront(1);

  // change strategy demo
  std::cout << "3) " << deq << std::endl;
  deq.ChangeAllocator(std::make_shared<stupid_strategy_t>());
  std::cout << "4) " << deq << std::endl;

  // pop demo
  std::cout << "5) " << deq.PopFront() << " " << deq.PopBack() << std::endl;
  std::cout << "6) " << deq << std::endl;

  // copy constructor demo
  auto deq1{deq};
  std::cout << "7) " << deq1 << std::endl;
  // copy operator demo
  deq.PushBack(4);
  deq.PushFront(1);
  deq1 = deq;
  std::cout << "8) " << deq1 << std::endl;
  // copy constructor with new allocator strategy demo
  deque_t<int> deq2(deq, std::make_shared<stupid_strategy_t>());
  std::cout << "9) " << deq2 << std::endl;
  // copy function with new allocator strategy demo
  deq.PopBack();
  deq2.Copy(deq, std::make_shared<stupid_strategy_t>());
  std::cout << "10) " << deq2 << std::endl;

  // move constructor demo
  auto deqGenetor = [](int x) -> deque_t<int> {
    deque_t<int> deq(std::make_shared<stupid_strategy_t>());
    deq.PushBack(x);
    return deq;
  };
  auto deq3{deqGenetor(0)};
  std::cout << "11) " << deq3 << std::endl;
  // move operator demo
  deq3 = deqGenetor(1);
  std::cout << "12) " << deq3 << std::endl;

  // clear demo
  deq1.Clear();
  std::cout << "13) " << deq1 << std::endl;

  // is empty demo
  std::cout << "14) " << std::boolalpha << deq3.IsEmpty() << ", " << deq1.IsEmpty() << std::endl;

  return 0;
}
