#include "arpch.h"
#include "CommandQueue.h"

namespace ar
{
	CommandQueue::CommandQueue()
		: m_Current(m_Queue.end())
	{

	}
	void CommandQueue::Execute(std::unique_ptr<Command> command)
	{
		command->Execute();
		PushCommand(std::move(command));
	}

	bool CommandQueue::CanUndo()
	{
		return !m_Queue.empty() && m_Current != m_Queue.begin();
	}

	void CommandQueue::Undo()
	{
		if (CanUndo())
		{
			--m_Current;
			(*m_Current)->Undo();
		}
		AR_TRACE("Command undone. Current command queue size: {0}", m_Queue.size());
	}

	bool CommandQueue::CanRedo()
	{
		return !m_Queue.empty() && m_Current != m_Queue.end();
	}

	void CommandQueue::Redo()
	{
		if (CanRedo())
		{
			(*m_Current)->Execute();
			++m_Current;
		}
		AR_TRACE("Command redone. Current command queue size: {0}", m_Queue.size());
	}
	void CommandQueue::PushCommand(std::unique_ptr<Command> command)
	{
		if (m_Current != m_Queue.end())
		{
			m_Queue.erase(m_Current, m_Queue.end());
		}

		// Add the new command
		m_Queue.push_back(std::move(command));

		// Set current to end (meaning "all commands have been executed")
		m_Current = m_Queue.end();

		AR_TRACE("PushCommand called. Current count: {0}", m_Queue.size());
	}
}

