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
		AddTorusCommand(TorusDesc& desc, std::shared_ptr<Scene> scene);

		void Execute() override;
		void Undo() override;
	private:
		std::shared_ptr<Scene> m_Scene;
		TorusDesc m_Description;
		Entity m_Entity;
	};

	class SelectObjectCommand : public Command
	{
	public:
		SelectObjectCommand(ar::Entity object);

		void Execute() override;
		void Undo() override;
	private:
		ar::Entity m_Object;
	};

	class DeselectObjectCommand : public Command
	{
	public:
		DeselectObjectCommand(ar::Entity object);

		void Execute() override;
		void Undo() override;
	private:
		ar::Entity m_Object;
	};
}