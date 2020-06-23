#pragma once

#include "Delegates.h"

namespace Vct
{
    template<typename R, typename...Args>
    using Delegate = std::function<R(Args...)>;

    /* Thread safe queue of delegates that are polled continously by the application on the main thread */
    class DelegateQueue
    {
        
    };
}