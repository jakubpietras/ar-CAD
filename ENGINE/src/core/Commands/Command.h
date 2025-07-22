#pragma once

namespace ar
{
	class Command
	{
	public:
		virtual ~Command() {}
		virtual void Execute() = 0;
		virtual void Undo() = 0;
	};
}