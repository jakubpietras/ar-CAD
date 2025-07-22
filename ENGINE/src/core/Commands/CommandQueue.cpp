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
	void CommandQueue::Undo()
	{
		if (m_Current != m_Queue.end())
		{
			(*m_Current)->Undo();
			if (m_Current != m_Queue.begin())
				--m_Current;
		}
	}
	void CommandQueue::Redo()
	{
		if (m_Current != m_Queue.end() && std::next(m_Current) != m_Queue.end())
		{
			++m_Current;
			(*m_Current)->Execute();
		}
	}
	void CommandQueue::PushCommand(std::unique_ptr<Command> command)
	{
		if (std::distance(m_Current, m_Queue.end()) > 1)
		{
			// erase all commands after the current one
			m_Current++;
			m_Queue.erase(m_Current, m_Queue.end());

			// add the new command
			m_Queue.push_back(std::move(command));

			// get to the recently pushed command
			m_Current = m_Queue.end();
			m_Current--;
		}
		else
		{
			// add the new command
			m_Queue.push_back(std::move(command));

			// get to the recently pushed command
			m_Current = m_Queue.end();
			m_Current--;
		}
	}
}

