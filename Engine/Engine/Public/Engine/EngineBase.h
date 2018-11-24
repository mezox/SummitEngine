#pragma once

#ifndef ENGINE_API
	#ifdef _NOEXPORT
		#define ENGINE_API
	#elif _ENGINE_EXPORT
		#define ENGINE_API __attribute__((visibility("default")))
	#else
		#define ENGINE_API __attribute__((visibility("default")))
	#endif
#endif
