#include "vctPCH.h"

#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <iostream>
using namespace Vct;

Ref<spdlog::logger> Log::s_Logger;

void Log::Init()
{
    std::cout << "Init Log\n";

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("%^[%T] %n: %v%$");

    s_Logger = CreateRef<spdlog::logger>("Console", console_sink);
    spdlog::register_logger(s_Logger);
    s_Logger->set_level(spdlog::level::trace);
    s_Logger->warn("this should appear in both console and file");
    s_Logger->info("this message should not appear in the console, only in the file");
}
