#pragma once

#include <string>
#include <cstdint>

namespace tweet {

	enum class StatusEnum : std::uint32_t {
		OK,
		UNAUTHENTICATED,
		INVALID_ARGUMENT,
		ALREADY_EXISTS,
		PERMISSION_DENIED,
		INTERNAL
	};

	template<typename T>
	class Status {
	public:
		StatusEnum status = StatusEnum::OK;
		std::string error_message;
		T value;
		bool Ok() { return status == StatusEnum::OK; }
	};

}