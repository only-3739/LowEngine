#pragma once

#include <Common/Types.h>
#include <fmt/base.h>
#include <fmt/format.h>
#include <cassert>

// TODO - exception enum class 만들기

#define LOG(context) fmt::print("{}", context);

#define WARRING(kind, context)                                  \
    do {                                                        \
        fmt::print(                                             \
            "Kind: {}\nFile: {}\nLine: {}\nFunc: {}\n\n {}",    \
            (kind),                                             \
            __FILE__,                                           \
            __LINE__,                                           \
            __func__,                                           \
            (context));                                         \
    } while (0)

#define EXCEPTION(kind, context)                                \
    do {                                                        \
        WARRING((kind), (context));                             \
        abort();                                                \
    } while (0)

#define ASSERTEX(expr, kind, context)    \
    do {                                 \
        if (!(expr)) {                   \
            EXCEPTION((kind), (context));\
        }                                \
    } while (0)

#define ASSERT(expr)                                    \
    do {                                                \
        if (!(expr)) {                                  \
            fmt::print("File: {}, Line: {}, Func: {}",  \
            __FILE__,                                   \
            __LINE__,                                   \
            __func__);                                  \
            abort();                                    \
        }                                               \
    } while (0)

#ifdef LE_DEBUG
    #define DEBUG_LOG(context) LOG(context)
    #define DEBUG_WARRING(kind, context) WARRING(kind, context)
    #define DEBUG_EXCEPTION(kind, context) EXCEPTION(kind, context)
    #define DEBUG_ASSERTEX(expr, kind, context) ASSERTEX(expr, kind, context)
    #define DEBUG_ASSERT(expr) ASSERT(expr)
#else
    #define DEBUG_LOG(context) (void(0))
    #define DEBUG_WARRING(kind, context) (void(0))
    #define DEBUG_EXCEPTION(kind, context) (void(0))
    #define DEBUG_ASSERTEX(expr, kind, context) (void(0))
    #define DEBUG_ASSERT(expr) (void(0))
#endif
