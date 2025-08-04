#pragma once
#include <memory>

namespace ar
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

	struct UInt2
	{
		uint32_t u, v;
	};
}