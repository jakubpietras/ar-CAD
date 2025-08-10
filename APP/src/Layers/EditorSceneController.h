#pragma once

#include "Tools/EditorState.h"

class EditorSceneController
{
public:
	void ProcessStateChanges(EditorState& state);

private:
	void DeleteEntities();

};