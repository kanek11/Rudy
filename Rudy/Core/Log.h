#pragma once

#include "Rudy/Core/Base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Rudy {

	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};

}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

// Core log macros
#define RD_CORE_TRACE(...)    ::Rudy::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RD_CORE_INFO(...)     ::Rudy::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RD_CORE_WARN(...)     ::Rudy::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RD_CORE_ERROR(...)    ::Rudy::Log::GetCoreLogger()->error(__VA_ARGS__)
#define RD_CORE_CRITICAL(...) ::Rudy::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define RD_TRACE(...)         ::Rudy::Log::GetClientLogger()->trace(__VA_ARGS__)
#define RD_INFO(...)          ::Rudy::Log::GetClientLogger()->info(__VA_ARGS__)
#define RD_WARN(...)          ::Rudy::Log::GetClientLogger()->warn(__VA_ARGS__)
#define RD_ERROR(...)         ::Rudy::Log::GetClientLogger()->error(__VA_ARGS__)
#define RD_CRITICAL(...)      ::Rudy::Log::GetClientLogger()->critical(__VA_ARGS__)
