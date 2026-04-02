#pragma once

#include <memory>

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace DefinitelyEngine {
    class DEFINITELY_ENGINE_API Log
    {
        public:
            static void Init();

            inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
            inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
        private:
            static std::shared_ptr<spdlog::logger> s_CoreLogger;
            static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}

#define DE_CORE_ERROR(...)::DefinitelyEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define DE_CORE_WARN(...)::DefinitelyEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define DE_CORE_INFO(...)::DefinitelyEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define DE_CORE_TRACE(...)::DefinitelyEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define DE_CORE_FATAL(...)::DefinitelyEngine::Log::GetCoreLogger()->fatal(__VA_ARGS__)


#define DE_CLIENT_ERROR(...)::DefinitelyEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define DE_CLIENT_WARN(...)::DefinitelyEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define DE_CLIENT_INFO(...)::DefinitelyEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define DE_CLIENT_TRACE(...)::DefinitelyEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define DE_CLIENT_FATAL(...)::DefinitelyEngine::Log::GetClientLogger()->fatal(__VA_ARGS__)

#ifdef DE_ENABLE_ASSERTS
    #define DE_CORE_ASSERT(x, ...) { if(!(x)) { DE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); DE_DEBUGBREAK(); } }
    #define DE_ASSERT(x, ...)      { if(!(x)) { DE_CLIENT_ERROR("Assertion Failed: {0}", __VA_ARGS__); DE_DEBUGBREAK(); } }
#else
    #define DE_CORE_ASSERT(x, ...)
    #define DE_ASSERT(x, ...)
#endif
