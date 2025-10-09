#pragma once
#include <ARCAD.h>
#include <ARMAT.h>
#include "SimState.h"

class SimApp : public ar::Application
{
public:
	SimApp();
	~SimApp();
private:
	SimState m_State;
};