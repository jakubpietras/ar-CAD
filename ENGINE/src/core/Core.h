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
		UInt2(std::initializer_list<unsigned int> l)
		{
			u = *(l.begin());
			v = *(l.begin() + 1);
		}
		unsigned int u, v;
	};
}