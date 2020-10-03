#pragma once

#include <iostream>
#include <chrono>

#define _STR(S) #S
#define STR(S) _STR(S)

#define MEASURE_BEGIN(NAME)                                             \
auto timeStart##NAME{ std::chrono::high_resolution_clock::now() };      \

#define MEASURE_END(NAME)                                               \
auto timeEnd##NAME{ std::chrono::high_resolution_clock::now() };        \
auto timeDuration##NAME                                                 \
{                                                                       \
  std::chrono::duration_cast<std::chrono::milliseconds>                 \
  (                                                                     \
    timeEnd##NAME - timeStart##NAME                                     \
  )                                                                     \
};                                                                      \
std::cout << STR(NAME) << " " << timeDuration##NAME.count() << "ms\n";