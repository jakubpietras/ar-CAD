#pragma once
#include <ARMAT.h>

struct MaterialDesc
{
	ar::mat::UInt2 Samples = { 1500, 1500 };
	ar::mat::Vec3 Size = { 15, 5, 15 };
	float BaseHeight = 1.5;
};