#pragma once
#include <memory>

// Assertions
#ifdef VCT_DEBUG
    #ifdef VCT_PLATFORM_WINDOWS
        #define ASSERT(x, ...)  if(!(x)){VCT_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}
    #elif VCT_PLATFORM_LINUX
        #define ASSERT(x, ...)  if((!x)){VCT_ERROR("Assertion Failed: {0}", __VA_ARGS__); raise(SIGABRT);}
    #endif
#else
    #define ASSERT(x, ...)
#endif // VCT_DEBUG

// Compilter specifics
#if defined _MSC_VER  && _MSC_VER >= 1900
    #define FORCEINLINE __forceinline
#elif defined __GNUG__
    #define FORCEINLINE __attribute__((always_inline))
#else
    #define FORCEINLINE
#endif // VCT_PLATFORM_WINDOWS

// Typedefs

// Helpers
#define BIND_FN(x) std::bind(x, this, std::placeholders::_1)
#define BIND_MFN_1(x, inObject) std::bind(x, inObject, std::placeholders::_1) 
#define BIND_MFN_2(x, inObject) std::bind(x, inObject, std::placeholders::_1, std::placeholders::_2) 
#define BIND_MFN_3(x, inObject) std::bind(x, inObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) 
#define BIND_MFN_4(x, inObject) std::bind(x, inObject, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) 

#define BIT(n) (1 << n)
namespace Vct
{
    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename...Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    extern class Engine* gEngine;
}
