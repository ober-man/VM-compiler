#pragma once

#include <iostream>

namespace compiler
{

#define CONCAT(A, B) A##B
#define SELECT(NAME, NUM) CONCAT(NAME##_, NUM)
#define COMPOSE(NAME, ARGS) NAME ARGS
#define GET_COUNT(_0, _1, _2, _3, _4, _5, _6, COUNT, ...) COUNT
#define EXPAND() , , , , , , // 6 commas (or 7 empty tokens)
#define VA_SIZE(...) COMPOSE(GET_COUNT, (EXPAND __VA_ARGS__(), 0, 6, 5, 4, 3, 2, 1))
#define VA_SELECT(NAME, ...) SELECT(NAME, VA_SIZE(__VA_ARGS__))(__VA_ARGS__)

#define ASSERT(...) VA_SELECT(ASSERT, __VA_ARGS__)

#define ASSERT_1(cond)                                                                             \
    if (!(cond))                                                                                   \
    {                                                                                              \
        ASSERT_PRINT("");                                                                          \
    }

#define ASSERT_2(cond, message)                                                                    \
    if (!(cond))                                                                                   \
    {                                                                                              \
        ASSERT_PRINT(message);                                                                     \
    }

#define ASSERT_PRINT(message)                                                                      \
    std::cerr << "Assertion failed: " << message << std::endl;                                     \
    std::cerr << "in " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << std::endl;         \
    std::abort();

#define UNREACHABLE()                                                                              \
    do                                                                                             \
    {                                                                                              \
        ASSERT_PRINT("This line should be unreachable");                                           \
        __builtin_unreachable();                                                                   \
    } while (0)
} // namespace compiler