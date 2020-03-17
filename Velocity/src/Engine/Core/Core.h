#pragma once
#include <memory>

// Assertions
#define ASSERT(x, ...)
#define VERIFY(x, ...)

// Typedefs

// Helpers
#define BIND_FN(x) std::bind(x, this, std::placeholders::_1)
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
}
