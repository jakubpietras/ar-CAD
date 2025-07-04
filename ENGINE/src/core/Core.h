#pragma once

#ifdef AR_PLATFORM_WINDOWS
	#ifdef AR_BUILD_DLL
		#define ARCAD __declspec(dllexport)
	#else
		#define ARCAD __declspec(dllimport)
	#endif
#else
	#error Windows-only support
#endif
