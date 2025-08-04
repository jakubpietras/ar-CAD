#pragma once
#include <list>
#include <memory>
#include "SceneCommands.h"

namespace ar
{
	class CommandQueue
	{
	public:
		CommandQueue();
		void Execute(std::unique_ptr<Command> command);
		
		bool CanUndo();
		void Undo();
		
		bool CanRedo();
		void Redo();
	private:
		void PushCommand(std::unique_ptr<Command> command);
		std::list<std::unique_ptr<Command>> m_Queue;
		// points to the first command that has not been executed
		std::list<std::unique_ptr<Command>>::iterator m_Current;
	};
}