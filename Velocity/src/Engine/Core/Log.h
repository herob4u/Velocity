#pragma once
#include "Core.h"

#include "spdlog/spdlog.h"
#include <spdlog/fmt/ostr.h>

namespace Vct
{
    class Log
    {
    public:
        static void Init();
        static Ref<spdlog::logger> GetLogger() { return s_Logger; }

    private:
        static Ref<spdlog::logger> s_Logger;
    };
}
