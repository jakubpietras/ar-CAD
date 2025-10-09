#pragma once
#include <ARMAT.h>

struct MaterialDesc
{
	ar::mat::UInt2 Divisions = { 100, 100 };
	ar::mat::Vec3 Size = { 15, 5, 15 };
	float BaseHeight = 15;
};