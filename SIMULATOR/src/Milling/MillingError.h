#pragma once

struct MillingError
{
	// 0 - success, 1 - failure
	int NonCuttingContact = 0;
	int OverPlunge = 0;
	int DownMilling = 0;
};