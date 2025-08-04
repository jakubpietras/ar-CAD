#pragma once
#include "Command.h"
#include "core/Scene/Scene.h"
#include "core/Utils/TorusUtils.h"
#include "core/Scene/Entity.h"
#include <memory>

namespace ar
{
	class AddTorusCommand : public Command 
	{
	public:
		AddTorusCommand(TorusDesc& desc, std::shared_ptr<Scene> scene,
			std::function<void(ar::Entity)> deleteFunc);

		void Execute() override;
		void Undo() override;
	private:
		std::shared_ptr<Scene> m_Scene;
		TorusDesc m_Description;
		Entity m_Entity;
		std::function<void(ar::Entity)> m_DeleteFunction;
	};

	template<typename T>
	class SetPropertyCommand : public ar::Command
	{
	public:
		SetPropertyCommand(T* target, const T& oldValue, const T& newValue, bool* dirtyFlag = nullptr)
			: m_Target(target), m_OldValue(oldValue), m_NewValue(newValue), m_DirtyFlag(dirtyFlag)
		{
			AR_ASSERT(target, "Target pointer is null in SetPropertyCommand");
		}

		void Execute() override
		{
			*m_Target = m_NewValue;
			if (m_DirtyFlag)
				*m_DirtyFlag = true;
		}

		void Undo() override
		{
			*m_Target = m_OldValue;
			if (m_DirtyFlag)
				*m_DirtyFlag = true;
		}

	private:
		T* m_Target = nullptr;
		T m_OldValue{};
		T m_NewValue{};
		bool* m_DirtyFlag = nullptr;
	};

}