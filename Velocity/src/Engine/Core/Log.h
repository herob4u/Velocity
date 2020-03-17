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

#ifdef VCT_DEBUG
    #define VCT_INFO(...)                    Vct::Log::GetLogger()->info(__VA_ARGS__);
    #define VCT_WARN(...)                    Vct::Log::GetLogger()->warn(__VA_ARGS__);
    #define VCT_ERROR(...)                   Vct::Log::GetLogger()->error(__VA_ARGS__);
    #define VCT_TRACE(...)                   Vct::Log::GetLogger()->trace(__VA_ARGS__);
#else
    #define VCT_INFO(...) 
    #define VCT_WARN(...) 
    #define VCT_ERROR(...)
    #define VCT_TRACE(...)
#endif

