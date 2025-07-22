#pragma once
#include "Command.h"

namespace ar
{
	class AddTorusCommand : public Command 
	{

	public:
		void Execute() override;
		void Undo() override;
	};
}